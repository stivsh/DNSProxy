#ifndef SERVERCORE_H
#define SERVERCORE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <map>
#include <set>
#include <cstdlib>
#include <limits>
#include "eventhandler.h"
class ServerCore{
    static ServerCore* server;
    volatile bool start;
    volatile bool restart;
    HandlerFactory hfact;
    std::set<EventHandler*> handlers_to_delete;
    void start_loop();
 public:
    enum COMMANDS{STOP=0,RESTART=1};
    void external_command(COMMANDS command);
    void delete_handler_next_itration(EventHandler* handler);
    static int start_server(int argc, char *argv[]);
    static void CriticalError();
    static void DeleteServer();
    HandlerFactory& get_hfact();
    static ServerCore& Instance();
private:
        ServerCore(){}
        ServerCore(const ServerCore& root){(void)&root;}
        ServerCore& operator=(const ServerCore& s){(void)&s;return *this;}
};
#endif // SERVERCORE_H