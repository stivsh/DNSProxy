#include "pool.h"
#include <list>

void Pool::register_event_hendler(HANDLER h, EventHandler* ev){
    handlersmap[h.get_descriptor()]=ev;
}
void Pool::pool(unsigned short second,unsigned short usecond,unsigned short timeout_check){
    delete_unactive_handlers();

    int maxfd=fill_fd_sets();
    timeval timeout;
    timeout.tv_sec = second;
    timeout.tv_usec = usecond;

    int select_ret=select(maxfd+1,&readset,&writeset,&exceptset,&timeout);
    if(select_ret==0){

    }
    if(select_ret>0){
        evalute_events();
    }
    if(select_ret<0){

    }
    if(difftime(time(0),last_time_check)>timeout_check){
        evalute_time_out_events();
        time(&last_time_check);
    }
}
Pool::Pool(AbstractEventHandlerFactory &hfactory_):hfactory(hfactory_){
    time(&last_time_check);
}

Pool::~Pool(){
    clear();
}
int Pool::fill_fd_sets(){
    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    FD_ZERO(&exceptset);
    int maxfd=0;
    for(EvHandlerMap::iterator evhand=handlersmap.begin();evhand!=handlersmap.end();++evhand){
        if(!evhand->second->delete_this_handler()){
            if(evhand->first>maxfd)
                maxfd=evhand->first;
            EventsToReact react_ev=evhand->second->events_to_react();
            if(react_ev.readready)FD_SET(evhand->first,&readset);
            if(react_ev.writeready)FD_SET(evhand->first,&writeset);
            if(react_ev.exeption)FD_SET(evhand->first,&exceptset);
        }
    }
    return maxfd;
}
void Pool::evalute_events(){
    for(EvHandlerMap::iterator it=handlersmap.begin();
        it!=handlersmap.end();++it){
            if(FD_ISSET(it->first, &exceptset))it->second->exeption();
            if(FD_ISSET(it->first, &readset))it->second->ready_read();
            if(FD_ISSET(it->first, &writeset))it->second->ready_write();
        }
}
void Pool::evalute_time_out_events(){
    for(EvHandlerMap::iterator it=handlersmap.begin();
        it!=handlersmap.end();++it){
            it->second->time_out();
        }
}

void Pool::delete_unactive_handlers(){
    std::list<int> descriptors_to_delete;
    for(EvHandlerMap::iterator evhand=handlersmap.begin();evhand!=handlersmap.end();++evhand){
        if(evhand->second->delete_this_handler()){
            descriptors_to_delete.push_back(evhand->first);
        }
    }
    for(std::list<int>::iterator it=descriptors_to_delete.begin();
            it!=descriptors_to_delete.end();++it){
        hfactory.delete_ev_handler(handlersmap[*it]);
        handlersmap.erase(*it);
    }
}
void Pool::clear(){
    for(EvHandlerMap::iterator it=handlersmap.begin();it!=handlersmap.end();++it){
        hfactory.delete_ev_handler(it->second);
    }
    handlersmap.clear();
}
