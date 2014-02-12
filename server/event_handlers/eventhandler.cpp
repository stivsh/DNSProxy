#include "eventhandler.h"
void EventHandler::fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset){
    FD_SET(sd, readset);
    FD_SET(sd, writeset);
    FD_SET(sd, exceptset);
}
