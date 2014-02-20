#include "NewConnectionHandler.h"
#include "../servercore.h"
#include "../../common/OptionReader.h"
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
    addr.sin_port = htons((unsigned short)OptionReader::get_int_opt("listen_port"));
    addr.sin_addr.s_addr = inet_addr(OptionReader::get_cstr_opt("listen_addr"));
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        Logger::Instance().critical("Can't bind listen port");
        ServerCore::CriticalError();
    }
    listen(listener, 5);
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
    Logger::Instance().critical("Can't handles new connections!");
    ServerCore::CriticalError();

}
void NewConnectionHandler::fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset){
    (void)writeset;
    FD_SET(sd, readset);
    FD_SET(sd, exceptset);
}
