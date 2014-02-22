#ifndef SERVERCORE_H
#define SERVERCORE_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <map>
#include <limits>
#include <fcntl.h>
#include "../common/commondefines.h"
#include "../common/logger.h"
#include "../common/OptionReader.h"
#include "../common/watchdogtimer.h"
class UDPClientData{
    public:
    unsigned short mess_id;
    struct sockaddr from;
    socklen_t fromlen;
    time_t time_stamp;
    UDPClientData(){time(&time_stamp);}
};
class ServerCore{
    bool start;
    bool restart;
    ServerCore(){}
    ServerCore(const ServerCore& root){(void)&root;}
    ServerCore& operator=(const ServerCore& s){(void)&s;return *this;}
public:
   void start_loop();
   static void external_command(ServerCommands command);
   static int start_server();
    static ServerCore& Instance(){
        static ServerCore theSingleInstance;
        return theSingleInstance;
       }
};

#endif // SERVERCORE_H
