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
void WatchDogHandler(){
    Logger::critical()<<"dnsproxy server watch dog event"<<Logger::endl;
    exit(1);
}
void ServerCore::start_loop(){
    WatchDogTimer wdtimer(2,WatchDogHandler);
    while(!restart){
        wdtimer.reset_timer();
        pool.pool(1,0,10);
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
        EventHandler *listener=server.hfact.create_listener_handler();
        EventHandler *udp_server=server.hfact.create_udpserver_handler();
        server.pool.register_event_hendler(listener->get_handler(),listener);
        server.pool.register_event_hendler(udp_server->get_handler(),udp_server);
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
    ServerCore::DeleteServer();
    exit(1);

}
EventHandler* ServerCore::create_client_handler(int sd,struct sockaddr addr){
    EventHandler* ev=hfact.create_client_handler(sd,addr);
    pool.register_event_hendler(ev->get_handler(),ev);
    return ev;
}
