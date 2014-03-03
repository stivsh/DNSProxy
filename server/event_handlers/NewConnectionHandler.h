#ifndef NEWCONNECTIONHANDLER_H
#define NEWCONNECTIONHANDLER_H

#include "../common/eventhandler.h"

class NewConnectionHandler:public EventHandler{
 public:
    NewConnectionHandler();
    bool delete_this_handler();
    EventsToReact events_to_react();
    void ready_read();
    void exeption();

};
#endif // NEWCONNECTIONHANDLER_H
