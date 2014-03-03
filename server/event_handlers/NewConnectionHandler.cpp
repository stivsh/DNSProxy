#include "NewConnectionHandler.h"
#include "../servercore.h"
#include "../../common/OptionReader.h"
#include <fcntl.h>

NewConnectionHandler::NewConnectionHandler(){
    int listener;
    struct sockaddr_in addr;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listener, F_SETFL, O_NONBLOCK);
    if(listener < 0){
        Logger::critical()<<"Can't create listen ERRNO:"<<errno<<Logger::endl;
        ServerCore::CriticalError();
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons((unsigned short)OptionReader::get_int_opt("listen_port"));
    addr.sin_addr.s_addr = inet_addr(OptionReader::get_cstr_opt("listen_addr"));
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        Logger::critical()<<"Can't bind listen port ERRNO:"<<errno<<Logger::endl;
        ServerCore::CriticalError();
    }
    listen(listener, 5);
    handler.set_descriptor(listener);
}

void NewConnectionHandler::ready_read(){
    struct sockaddr addr;
    socklen_t addrlen=sizeof(addr);
    int sock=accept(handler.get_descriptor(),&addr,&addrlen);
    if(sock!=-1){
        fcntl(sock, F_SETFL, O_NONBLOCK);
        ServerCore::Instance().create_client_handler(sock,addr);
    }else{
        Logger::error()<<"Can't handle new connection ERRNO:"<<errno<<addr<<Logger::endl;
    }
}

void NewConnectionHandler::exeption(){
    Logger::critical()<<"Can't handles new connections! ERRNO:"<<errno<<Logger::endl;
    ServerCore::CriticalError();

}
EventsToReact NewConnectionHandler::events_to_react(){
    EventsToReact ev_react;
    ev_react.readready=true;
    ev_react.exeption=true;
    return ev_react;
}
bool NewConnectionHandler::delete_this_handler(){
    return false;
}
