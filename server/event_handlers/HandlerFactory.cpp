#include "HandlerFactory.h"
EventHandler* HandlerFactory::create_client_handler(int sd,struct sockaddr addr){
    if(!uservh){
        Logger::error()<<"Creating of client sock but dinn't create UDP con. ERRNO:"<<errno<<Logger::endl;
        return 0;
    }
    EventHandler* handler=new ClientHandler(sd,uservh,addr);
    handlers.insert(handler);
    return handler;
}
EventHandler* HandlerFactory::create_listener_handler(){
    EventHandler* handler=new NewConnectionHandler();
    handlers.insert(handler);
    return handler;
}
EventHandler* HandlerFactory::create_udpserver_handler(){
    if(uservh)delete_ev_handler(uservh);
    uservh=new UDPServerHandler();
    EventHandler* handler=uservh;
    handlers.insert(handler);
    return handler;
}
void HandlerFactory::delete_ev_handler(EventHandler* ev){
    if(handlers.count(ev)){
        if(uservh==ev)uservh=0;
        if(uservh)uservh->handler_removed(ev);
        handlers.erase(ev);
        delete ev;
    }
}
void HandlerFactory::revoke_all_handlers(){
    uservh=0;
    for(std::set<EventHandler*>::iterator it=handlers.begin();
        it!=handlers.end();++it){
            delete *it;
        }
    handlers.clear();
}
HandlerFactory::~HandlerFactory(){
    revoke_all_handlers();
}
bool HandlerFactory::is_valid_pointer(EventHandler* ev){
    if(handlers.count(ev))
        return true;
    return false;
}
