#ifndef NEWCONNECTIONHANDLER_H
#define NEWCONNECTIONHANDLER_H
#include "eventhandler.h"
class NewConnectionHandler:public EventHandler{
 public:
    NewConnectionHandler();
    void fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset);
    void ready_read();
    void exeption();

    virtual ~NewConnectionHandler(){}
};
#endif // NEWCONNECTIONHANDLER_H
