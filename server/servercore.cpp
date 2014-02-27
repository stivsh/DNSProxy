#include "servercore.h"
#include "../common/OptionReader.h"
ServerCore* ServerCore::server=0;
void ServerCore::external_command(ServerCommands command){
    switch(command){
        case STOP:
            ServerCore::Instance().start=false;
            ServerCore::Instance().restart=true;
            break;
        case RESTART:
            ServerCore::Instance().restart=true;
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
void WatchDogHandler(){
    Logger::critical()<<"dnsproxy server watch dog event"<<Logger::endl;
    exit(1);
}
void ServerCore::start_loop(){
    WatchDogTimer wdtimer(2,WatchDogHandler);
    time(&last_time_check);
    while(!restart){
        wdtimer.reset_timer();
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
int ServerCore::start_server(){
    Logger::message()<<"Start server"<<Logger::endl;
    ServerCore& server=Instance();
    server.start=true;
    while(server.start){
        Logger::message()<<"Reading server options file"<<Logger::endl;
        OptionReader::LoadFromFile("/etc/dnsproxy/dnsproxyserver.config");
        Logger::message()<<OptionReader::as_string()<<Logger::endl;
        Logger::message()<<"Create standart handlers"<<Logger::endl;
        server.hfact.create_listener_handler();
        server.hfact.create_udpserver_handler();
        server.restart=false;
        Logger::message()<<"Start server loop"<<Logger::endl;
        server.start_loop();
        Logger::message()<<"Stop server loop"<<Logger::endl;
        server.hfact.revoke_all_handlers();
        if(server.start)
            usleep(120*1000);
    }
    Logger::message()<<"Stop server"<<Logger::endl;
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
    Logger::message()<<"Stoping vie critical error"<<Logger::endl;
    ServerCore::Instance().hfact.revoke_all_handlers();
    ServerCore::DeleteServer();
    exit(1);

}
