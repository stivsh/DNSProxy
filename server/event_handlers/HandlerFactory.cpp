#include "HandlerFactory.h"
EventHandler* HandlerFactory::create_client_handler(int sd,struct sockaddr addr){
    if(!uservh){
        Logger::error()<<"Creating of client sock but dinn't create UDP con. ERRNO:"<<errno<<Logger::endl;
        return 0;
    }
    EventHandler* handler=new ClientHandler(sd,uservh,addr);
    handlers.insert(handler);
    skToHandler.insert(std::make_pair(sd,handler));
    return handler;
}
EventHandler* HandlerFactory::create_listener_handler(){
    EventHandler* handler=new NewConnectionHandler();
    int sd=handler->get_handler();
    handlers.insert(handler);
    skToHandler.insert(std::make_pair(sd,handler));
    return handler;
}
EventHandler* HandlerFactory::create_udpserver_handler(){
    if(uservh)remove_handler(uservh);
    uservh=new UDPServerHandler();
    int sd=uservh->get_handler();
    EventHandler* handler=uservh;
    handlers.insert(handler);
    skToHandler.insert(std::make_pair(sd,handler));
    return handler;
}
void HandlerFactory::remove_handler(EventHandler* hand){
    if(handlers.count(hand)){
        if(uservh)uservh->handler_removed(hand);
        if(uservh==hand)uservh=0;
        handlers.erase(hand);
        skToHandler.erase(hand->get_handler());
        close(hand->get_handler());
        delete hand;
    }
}
std::set<EventHandler*>& HandlerFactory::get_handlers_set(){
    return handlers;
}
std::map<int,EventHandler*>& HandlerFactory::get_handler_map(){
    return skToHandler;
}
void HandlerFactory::revoke_all_handlers(){
    uservh=0;
    skToHandler.clear();
    for(std::set<EventHandler*>::iterator it=handlers.begin();
        it!=handlers.end();++it){
            close((*it)->get_handler());
            delete *it;
        }
    handlers.clear();
}
HandlerFactory::~HandlerFactory(){
    revoke_all_handlers();
}
