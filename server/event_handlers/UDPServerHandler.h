#ifndef UDPSERVERHANDLER_H
#define UDPSERVERHANDLER_H
#include "eventhandler.h"
#include "../../common/OptionReader.h"
#include "../../common/buffer.h"
#include "../../common/dnspack.h"
class ClientHandler;
class UDPServerHandler:public EventHandler{
    MessIdentConteiner mescont;
 public:
    UDPServerHandler();
    void fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset);
    void new_request(ClientHandler* ch,DNSPack request);
    void handler_removed(EventHandler* handler);
    void ready_read();
    void exeption();
    void time_out();
    virtual ~UDPServerHandler(){}
};
#endif // UDPSERVERHANDLER_H
