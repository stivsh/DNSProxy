#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include <map>
#include <set>
#include <unistd.h>
#include "../common/logger.h"
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "cstring"
#include <arpa/inet.h>
#include "messidentconteiner.h"
const unsigned int max_buff=1024*2;
class EventHandler{
 protected:
    bool errorflag;
    int sd;
 public:
    EventHandler():errorflag(false){}
    int get_handler(){return sd;}
    virtual void fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset);
    virtual void ready_read(){}
    virtual void ready_write(){}
    virtual void exeption(){}
    virtual void time_out(){}
    virtual ~EventHandler(){}
};
#endif // EVENTHANDLER_H
