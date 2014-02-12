#include "servercore.h"
ServerCore* ServerCore::server=0;
void ServerCore::external_command(COMMANDS command){
    switch(command){
        case STOP:
            start=false;
            restart=true;
            break;
        case RESTART:
            restart=true;
            break;
    }
}

void ServerCore::delete_handler_next_itration(EventHandler* handler){
    handlers_to_delete.insert(handler);
}
int fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset,std::set<EventHandler*> &hset){
    FD_ZERO(readset);
    FD_ZERO(writeset);
    FD_ZERO(exceptset);
    int maxfd=0;
    for(std::set<EventHandler*>::iterator it=hset.begin();
                it!=hset.end();++it){
        (*it)->fill_fd_sets(readset,writeset,exceptset);
        if((*it)->get_handler()>maxfd)maxfd=(*it)->get_handler();
    }
    return maxfd;
}
void evalute_time_out_events(std::set<EventHandler*> &hset){
    for(std::set<EventHandler*>::iterator it=hset.begin();
        it!=hset.end();++it)
        (*it)->time_out();
}
void evalute_events(fd_set *readset,fd_set *writeset,fd_set *exceptset,std::map<int,EventHandler*> &skToHandler){
    for(std::map<int,EventHandler*>::iterator it=skToHandler.begin();
        it!=skToHandler.end();++it){
            if(FD_ISSET(it->first, exceptset))it->second->exeption();
            if(FD_ISSET(it->first, readset))it->second->ready_read();
            if(FD_ISSET(it->first, writeset))it->second->ready_write();
        }
}
void deferred_handlers_delete(std::set<EventHandler*> &handlers_to_delete,HandlerFactory &hfact){
    for(std::set<EventHandler*>::iterator it=handlers_to_delete.begin();
        it!=handlers_to_delete.end();++it){
        hfact.remove_handler(*it);
    }
    handlers_to_delete.clear();
}

void ServerCore::start_loop(){
    time(&last_time_check);
    while(!restart){
        fd_set readset;
        fd_set writeset;
        fd_set exceptset;
        int maxfd=fill_fd_sets(&readset,&writeset,&exceptset,hfact.get_handlers_set());
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int select_ret=select(maxfd+1,&readset,&writeset,&exceptset,&timeout);
        if(select_ret==0){
        }
        if(select_ret>0){
            evalute_events(&readset,&writeset,&exceptset,hfact.get_handler_map());
        }
        if(select_ret<0){
            Logger::Instance().critical("polling error");
            ServerCore::CriticalError();
        }
        if(difftime(time(0),last_time_check)>10){//TODO мин из параметров
            evalute_time_out_events(hfact.get_handlers_set());
            time(&last_time_check);
        }
        deferred_handlers_delete(handlers_to_delete,hfact);
    }
}
HandlerFactory& ServerCore::get_hfact(){
    return hfact;
}
int ServerCore::start_server(int argc, char *argv[]){
    (void)argc;
    (void)argv;
    Logger::Instance().message("Start server");
    ServerCore& server=Instance();
    server.start=true;
    while(server.start){
        Logger::Instance().message("Create standart handlers");
        server.hfact.create_listener_handler();
        server.hfact.create_udpserver_handler();
        server.restart=false;
        Logger::Instance().message("Start loop");
        server.start_loop();
        Logger::Instance().message("Stop loop");
        server.hfact.revoke_all_handlers();
        if(server.start)
            sleep(120);
    }
    Logger::Instance().message("Stop server");
    DeleteServer();
    return 0;

}
ServerCore& ServerCore::Instance(){
    if(server==0)server = new ServerCore();
        return *server;
    }
void ServerCore::DeleteServer(){
    delete server;
}
void ServerCore::CriticalError(){
    Logger::Instance().critical("Stoping vie critical error");
    ServerCore::Instance().hfact.revoke_all_handlers();
    ServerCore::DeleteServer();
    exit(-1);

}
