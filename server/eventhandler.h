#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include <map>
#include <set>
#include <unistd.h>
#include "logger.h"
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "messidentconteiner.h"
const unsigned int max_buff=1024*2;
class EventHandler{
 protected:
    bool errorflag;
    int sd;
 public:
    EventHandler():errorflag(false){}
    int get_handler(){return sd;}
    virtual void ready_read(){}
    virtual void ready_write(){}
    virtual void exeption(){}
    virtual void time_out(){}
    virtual ~EventHandler(){}
};
class NewConnectionHandler:public EventHandler{
 public:
    NewConnectionHandler();
    void ready_read();
    void exeption();

    virtual ~NewConnectionHandler(){}
};
class ClientHandler;
class UDPServerHandler:public EventHandler{
    MessIdentConteiner mescont;
 public:
    UDPServerHandler();
    void new_request(ClientHandler* ch,char* request,unsigned short len);
    void handler_removed(EventHandler* handler);
    void ready_read();
    void exeption();
    void time_out();
    virtual ~UDPServerHandler(){}
};
class ClientHandler:public EventHandler{
    time_t last_reply_to_client;
    UDPServerHandler* uservh;
    struct sockaddr addr;
    bool write_ready;
    char* inbuffer;
    size_t inpos;
    char* outbuffer;
    size_t outpos;
    void drop_connection();
    void send_reply_to_client();
    void send_request_to_server();
 public:
    ClientHandler(int sd_,UDPServerHandler* _uservh,struct sockaddr addr_):
        uservh(_uservh),addr(addr_),write_ready(false),inpos(0),outpos(0){
        sd=sd_;
        inbuffer=new char[max_buff];
        outbuffer=new char[max_buff];
        time(&last_reply_to_client);
        Logger::Instance().client_message("New connection",&addr);
    }
    struct sockaddr* get_addr(){
        return &addr;
    }
    void new_reply_from_server(char* reply, unsigned short len);
    void ready_read();
    void ready_write();
    void exeption();
    void time_out();
    virtual ~ClientHandler(){
        delete[] inbuffer;
        delete[] outbuffer;
    }
};
class HandlerFactory{
    std::set<EventHandler*> handlers;
    std::map<int,EventHandler*> skToHandler;
    UDPServerHandler* uservh;
    public:
    EventHandler* create_client_handler(int sd,struct sockaddr addr){
        if(!uservh){
            Logger::Instance().error("Creating of client sock but dinn't create UDP con.");
            return 0;
        }
        EventHandler* handler=new ClientHandler(sd,uservh,addr);
        handlers.insert(handler);
        skToHandler.insert(std::make_pair(sd,handler));
        return handler;
    }
    EventHandler* create_listener_handler(){
        EventHandler* handler=new NewConnectionHandler();
        int sd=handler->get_handler();
        handlers.insert(handler);
        skToHandler.insert(std::make_pair(sd,handler));
        return handler;
    }
    EventHandler* create_udpserver_handler(){
        if(uservh)remove_handler(uservh);
        uservh=new UDPServerHandler();
        int sd=uservh->get_handler();
        EventHandler* handler=uservh;
        handlers.insert(handler);
        skToHandler.insert(std::make_pair(sd,handler));
        return handler;
    }
    void remove_handler(EventHandler* hand){
        if(handlers.count(hand)){
            if(uservh)uservh->handler_removed(hand);
            if(uservh==hand)uservh=0;
            handlers.erase(hand);
            skToHandler.erase(hand->get_handler());
            close(hand->get_handler());
            delete hand;
        }
    }
    std::set<EventHandler*>& get_handlers_set(){
        return handlers;
    }
    std::map<int,EventHandler*>& get_handler_map(){
        return skToHandler;
    }
    void revoke_all_handlers(){
        uservh=0;
        skToHandler.clear();
        for(std::set<EventHandler*>::iterator it=handlers.begin();
            it!=handlers.end();++it){
                close((*it)->get_handler());
                delete *it;
            }
        handlers.clear();
    }

    ~HandlerFactory(){
        revoke_all_handlers();
    }

};

#endif // EVENTHANDLER_H
