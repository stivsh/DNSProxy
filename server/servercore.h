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
#include "event_handlers/eventhandler.h"
#include "event_handlers/HandlerFactory.h"
#include "../common/commondefines.h"
#include "../common/watchdogtimer.h"
class ServerCore{
    static ServerCore* server;
    volatile bool start;
    volatile bool restart;
    HandlerFactory hfact;
    time_t last_time_check;
    std::set<EventHandler*> handlers_to_delete;
    void start_loop();
 public:
    static void external_command(ServerCommands command);
    void delete_handler_next_itration(EventHandler* handler);
    static int start_server();
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
