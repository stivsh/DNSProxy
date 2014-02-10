#include "eventhandler.h"
#include "servercore.h"
#include <cstring>
NewConnectionHandler::NewConnectionHandler(){
    int listener;
    struct sockaddr_in addr;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listener, F_SETFL, O_NONBLOCK);
    if(listener < 0){
        Logger::Instance().critical("Can't create listen");
        ServerCore::CriticalError();
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        Logger::Instance().critical("Can't bind listen port");
        ServerCore::CriticalError();
    }
    listen(listener, 100000);
    sd=listener;
}

void NewConnectionHandler::ready_read(){
    struct sockaddr addr;
    socklen_t addrlen=sizeof(addr);
    int sock=accept(sd,&addr,&addrlen);
    if(sock!=-1){
        fcntl(sock, F_SETFL, O_NONBLOCK);
        ServerCore::Instance().get_hfact().create_client_handler(sock,addr);
    }else{
        Logger::Instance().client_error("Can't handle new connection", &addr);
    }
}
void NewConnectionHandler::exeption(){

}
void ClientHandler::drop_connection(){
    errorflag=true;
    ServerCore::Instance().delete_handler_next_itration(this);
}

void ClientHandler::send_request_to_server(){
    while(true){
        if(inpos<=sizeof(short))return;
        unsigned short mess_len;
        mess_len=ntohs(*reinterpret_cast<unsigned short*>(inbuffer));
        if(mess_len>inpos-sizeof(short)){
            return;
        }else{
            char* message=inbuffer+sizeof(short);
            uservh->new_request(this,message,mess_len);
            inpos-=sizeof(short)+mess_len;
            memcpy(inbuffer,inbuffer+sizeof(short)+mess_len,inpos);
        }
    }
}
void ClientHandler::ready_read(){
    if(errorflag)return;
    int bytes_read = recv(sd, inbuffer+inpos, max_buff-inpos, 0);
    if(bytes_read>0){
        inpos+=bytes_read;
        send_request_to_server();
    }else if(bytes_read==0){
        drop_connection();
    }else if(bytes_read<0){
        drop_connection();
        Logger::Instance().client_error("Connection read error",&addr);
    }
}

void ClientHandler::send_reply_to_client(){
    if(outpos>0 && write_ready){
        int bytes_writen=send(sd, outbuffer, outpos, 0);
        if(bytes_writen>0){
            outpos-=bytes_writen;
            memcpy(outbuffer,outbuffer+bytes_writen,outpos);
            time(&last_reply_to_client);
        }else if(bytes_writen<0){
            drop_connection();
            Logger::Instance().client_error("Connection write error",&addr);
        }else if(bytes_writen==0){
            write_ready=false;
        }
    }
}
void ClientHandler::new_reply_from_server(char* reply, unsigned short len){
    if(errorflag)return;
    if(outpos+len+sizeof(short)>max_buff){
        drop_connection();
        Logger::Instance().client_error("Connection buffer overload",&addr);
        return;
    }
    unsigned short net_len=htons(len);
    memcpy(outbuffer+outpos,&net_len,sizeof(short));
    memcpy(outbuffer+outpos+sizeof(short),reply,len);
    outpos+=len+sizeof(short);
    send_reply_to_client();
}
void ClientHandler::ready_write(){
    if(errorflag)return;
    write_ready=true;
    send_reply_to_client();
}
void ClientHandler::exeption(){
    if(errorflag)return;
    drop_connection();

}
void ClientHandler::time_out(){
    if(errorflag)return;
    time_t now;
    time(&now);
    if(difftime(now,last_reply_to_client)>10){//TODO incrice
        drop_connection();
        Logger::Instance().client_message("Drop connection. no activity",&addr);
    }

}
UDPServerHandler::UDPServerHandler(){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock<0){
        errorflag=true;
        Logger::Instance().critical("Can't create UDP sock");
        ServerCore::CriticalError();
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = inet_addr("8.8.8.8");
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr))==-1){
        errorflag=true;
        Logger::Instance().critical("Can't create UDP connection");
        ServerCore::CriticalError();
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sd=sock;
}
void UDPServerHandler::new_request(ClientHandler* ch,char* request, unsigned short len){
    if(len>65527){
        Logger::Instance().client_message("To long UDP request",ch->get_addr());
        return;
    }
    if(len<sizeof(unsigned short)+1)return;
    unsigned short message_id=ntohs(*reinterpret_cast<unsigned short*>(request));
    unsigned short new_id=mescont.get_new_id(message_id,ch);
    if(new_id==0)
        return;
    new_id=htons(new_id);
    *reinterpret_cast<unsigned short*>(request)=new_id;
    send(sd, request,len, 0);
}
void UDPServerHandler::handler_removed(EventHandler* handler){
    mescont.delete_all_id_ralated_to(handler);

}

void UDPServerHandler::ready_read(){
    char* buf=new char[1024];
    int bytes_read = recv(sd, buf, 1024, 0);
    if(bytes_read<0){
        Logger::Instance().error("Can't read UDP reply");
    }
    if(bytes_read>0){
        if(!((unsigned short)bytes_read<sizeof(unsigned short)+1)){
            unsigned short message_id=ntohs(*reinterpret_cast<unsigned short*>(buf));
            unsigned short old_id=mescont.get_orig_id(message_id);
            ClientHandler* handler=static_cast<ClientHandler*>(mescont.get_handler(message_id));
            if(handler!=0 && old_id!=0){
                mescont.release_id(message_id);
                *reinterpret_cast<unsigned short*>(buf)=htons(old_id);
                handler->new_reply_from_server(buf, bytes_read);
            }
        }
    }
    delete[] buf;
}

void UDPServerHandler::exeption(){
    Logger::Instance().error("UDP Handler exection");
}

void UDPServerHandler::time_out(){
    //TODO проверять что ответы приходят
}
