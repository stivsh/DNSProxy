#include "ClientHandler.h"
#include "../servercore.h"
ClientHandler::ClientHandler(int sd_,UDPServerHandler* _uservh,struct sockaddr addr_):
    uservh(_uservh),addr(addr_),inbuffer(inbuffer_arr),outbuffer(outbuffer_arr),inpos(0),outpos(0){
    sd=sd_;
    time(&last_reply_to_client);
    Logger::Instance().client_message("New connection",&addr);
}
struct sockaddr* ClientHandler::get_addr(){
    return &addr;
}
void ClientHandler::drop_connection(){
    errorflag=true;
    ServerCore::Instance().delete_handler_next_itration(this);
}

void ClientHandler::send_request_to_server(){
    while(true){
        if(inpos<=sizeof(short))return;
        unsigned short mess_len=ntohs(*reinterpret_cast<unsigned short*>(inbuffer));
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
    if(outpos>0){
        int bytes_writen=send(sd, outbuffer, outpos, 0);
        if(bytes_writen>0){
            outpos-=bytes_writen;
            memcpy(outbuffer,outbuffer+bytes_writen,outpos);
            if(outpos==0)
                time(&last_reply_to_client);
        }else if(bytes_writen<0){
            drop_connection();
            Logger::Instance().client_error("Connection write error",&addr);
        }else if(bytes_writen==0){

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
    send_reply_to_client();
}
void ClientHandler::exeption(){
    if(errorflag)return;
    drop_connection();

}
void ClientHandler::time_out(){
    if(errorflag)return;
    if(difftime(time(0),last_reply_to_client)>3000){//TODO настройки decrice
        drop_connection();
        Logger::Instance().client_message("Drop connection. no activity",&addr);
    }

}
void ClientHandler::fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset){
    FD_SET(sd, readset);
    FD_SET(sd, exceptset);
    if(outpos)
        FD_SET(sd, writeset);
}
