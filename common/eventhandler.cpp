#include "eventhandler.h"
EventsToReact EventHandler::events_to_react(){
    EventsToReact ev;
    ev.exeption=ev.readready=ev.writeready=true;
    return ev;
}
