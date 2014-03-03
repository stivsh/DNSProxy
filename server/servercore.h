#ifndef SERVERCORE_H
#define SERVERCORE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <map>
#include <set>
#include <cstdlib>
#include <limits>
#include "../common/eventhandler.h"
#include "../common/pool.h"
#include "event_handlers/HandlerFactory.h"
#include "../common/commondefines.h"
#include "../common/watchdogtimer.h"
class ServerCore{
    static ServerCore* server;
    volatile bool start;
    volatile bool restart;
    HandlerFactory hfact;
    Pool pool;
    void start_loop();
 public:
    static void external_command(ServerCommands command);
    EventHandler* create_client_handler(int sd,struct sockaddr addr);
    static int start_server();
    static void CriticalError();
    static void DeleteServer();
    HandlerFactory& get_hfact();
    static ServerCore& Instance();
private:
        ServerCore():pool(hfact){}
        ServerCore(const ServerCore& root):pool(hfact){(void)&root;}
        ServerCore& operator=(const ServerCore& s){(void)&s;return *this;}
};
#endif // SERVERCORE_H
