#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H
#include "eventhandler.h"
class UDPServerHandler;
class ClientHandler:public EventHandler{
    time_t last_reply_to_client;
    UDPServerHandler* uservh;
    struct sockaddr addr;
    char inbuffer_arr[max_buff];
    char outbuffer_arr[max_buff];
    char* inbuffer;
    char* outbuffer;
    size_t inpos;
    size_t outpos;
    void drop_connection();
    void send_reply_to_client();
    void send_request_to_server();
 public:
    ClientHandler(int sd_,UDPServerHandler* _uservh,struct sockaddr addr_);
    void fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset);
    struct sockaddr* get_addr();
    void new_reply_from_server(char* reply, unsigned short len);
    void ready_read();
    void ready_write();
    void exeption();
    void time_out();
};
#endif // CLIENTHANDLER_H
