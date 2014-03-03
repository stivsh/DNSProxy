#ifndef HANDLERFACTORY_H
#define HANDLERFACTORY_H

#include "../common/eventhandler.h"
#include "ClientHandler.h"
#include "NewConnectionHandler.h"
#include "UDPServerHandler.h"
#include "../common/AbstractEventHandlerFactory.h"
#include <set>

class HandlerFactory:public AbstractEventHandlerFactory{
    std::set<EventHandler*> handlers;
    UDPServerHandler* uservh;
    public:
    EventHandler* create_client_handler(int sd,struct sockaddr addr);
    EventHandler* create_listener_handler();
    EventHandler* create_udpserver_handler();
    void delete_ev_handler(EventHandler* ev);
    void revoke_all_handlers();
    bool is_valid_pointer(EventHandler* ev);
    ~HandlerFactory();
};
#endif // HANDLERFACTORY_H
