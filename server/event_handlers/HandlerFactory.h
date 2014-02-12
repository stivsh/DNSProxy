#ifndef HANDLERFACTORY_H
#define HANDLERFACTORY_H
#include "eventhandler.h"
#include "ClientHandler.h"
#include "NewConnectionHandler.h"
#include "UDPServerHandler.h"
class HandlerFactory{
    std::set<EventHandler*> handlers;
    std::map<int,EventHandler*> skToHandler;
    UDPServerHandler* uservh;
    public:
    EventHandler* create_client_handler(int sd,struct sockaddr addr);
    EventHandler* create_listener_handler();
    EventHandler* create_udpserver_handler();
    void remove_handler(EventHandler* hand);
    std::set<EventHandler*>& get_handlers_set();
    std::map<int,EventHandler*>& get_handler_map();
    void revoke_all_handlers();
    ~HandlerFactory();
};
#endif // HANDLERFACTORY_H
