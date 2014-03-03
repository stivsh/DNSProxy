#ifndef POOL_H
#define POOL_H
#include "eventhandler.h"
#include "HANDLER.h"
#include <map>
#include <sys/select.h>
#include <ctime>
#include "AbstractEventHandlerFactory.h"

typedef std::map<int, EventHandler*> EvHandlerMap;
class Pool{
    EvHandlerMap handlersmap;
    AbstractEventHandlerFactory &hfactory;
    time_t last_time_check;
    fd_set readset;
    fd_set writeset;
    fd_set exceptset;
    int fill_fd_sets();
    void evalute_events();
    void evalute_time_out_events();
    void delete_unactive_handlers();
    void clear();
public:
    void register_event_hendler(HANDLER h, EventHandler* ev);
    void pool(unsigned short second,unsigned short usecond=0,unsigned short timeout_check=10);
    Pool(AbstractEventHandlerFactory &hfactory_);
    ~Pool();
};

#endif // POOL_H
