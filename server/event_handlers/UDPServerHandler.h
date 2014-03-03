#ifndef UDPSERVERHANDLER_H
#define UDPSERVERHANDLER_H

#include "messidentconteiner.h"
#include "../common/eventhandler.h"
#include "../../common/dnspack.h"

class ClientHandler;
class UDPServerHandler:public EventHandler{
    MessIdentConteiner mescont;
 public:
    UDPServerHandler();
    EventsToReact events_to_react();
    bool delete_this_handler();
    void new_request(ClientHandler* ch,DNSPack request);
    void handler_removed(EventHandler* handler);
    void ready_read();
    void exeption();
    void time_out();
};

#endif // UDPSERVERHANDLER_H
