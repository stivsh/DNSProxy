#ifndef ABSTRACTEVENTHANDLERFACTORY_H
#define ABSTRACTEVENTHANDLERFACTORY_H
#include "eventhandler.h"
class AbstractEventHandlerFactory
{
public:
    virtual void delete_ev_handler(EventHandler* ev)=0;
    virtual ~AbstractEventHandlerFactory(){}
};

#endif // ABSTRACTEVENTHANDLERFACTORY_H
