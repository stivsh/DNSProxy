#include "UDPServerHandler.h"
#include "../servercore.h"
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
    addr.sin_addr.s_addr = inet_addr("10.152.0.3");//TODO брать из параметров
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr))==-1){
        errorflag=true;
        Logger::Instance().critical("Can't create UDP connection");
        ServerCore::CriticalError();
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    sd=sock;
}
void UDPServerHandler::new_request(ClientHandler* ch,char* request, unsigned short len){
    if(len>65527){//TODO точный размер
        Logger::Instance().client_message("To long UDP request",ch->get_addr());
        return;
    }
    if(len<sizeof(unsigned short)+1)
        return;
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
    mescont.check_time_out();
}
void UDPServerHandler::fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset){
    (void)writeset;
    FD_SET(sd, readset);
    FD_SET(sd, exceptset);
}