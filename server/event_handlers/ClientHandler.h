#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include "../common/eventhandler.h"
#include "../common/buffer.h"
#include "../common/dnspack.h"

class UDPServerHandler;
class ClientHandler:public EventHandler{
    time_t last_reply_to_client;
    UDPServerHandler* uservh;
    struct sockaddr addr;
    Buffer input_buf;
    Buffer output_buf;
    void drop_connection();
    void send_reply_to_client();
    void send_request_to_server();
 public:
    ClientHandler(int sd,UDPServerHandler* _uservh,struct sockaddr addr_);
    virtual bool delete_this_handler();
    EventsToReact events_to_react();
    void new_reply_from_server(DNSPack &reply);
    void ready_read();
    void ready_write();
    void exeption();
    void time_out();
};
#endif // CLIENTHANDLER_H
