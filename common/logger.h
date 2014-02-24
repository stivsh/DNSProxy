#ifndef LOGGER_H
#define LOGGER_H
#include <syslog.h>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
class Logger{
     void client_log(int pri,const char* prefix,const char* mes,struct sockaddr *addr,bool errnob=false){
         if(addr==0){
             if(errnob)
                syslog(pri,"%s:%s ERRNO:%d",prefix,mes,errno);
             else
                 syslog(pri,"%s:%s",prefix,mes);
         }else{
             if(addr->sa_family==AF_INET){
                 struct sockaddr_in* addr_in=(struct sockaddr_in*)addr;
                 char str[INET_ADDRSTRLEN];
                 inet_ntop(AF_INET, &(addr_in->sin_addr), str, INET_ADDRSTRLEN);
                 if(errnob)
                    syslog(pri,"%s:%s IP:%s ERRNO:%d",prefix,mes,str,errno);
                 else
                    syslog(pri,"%s:%s IP:%s",prefix,mes,str);
             }else if(addr->sa_family==AF_INET6){
                 struct sockaddr_in6* addr_in=(struct sockaddr_in6*)addr;
                 char str[INET6_ADDRSTRLEN];
                 inet_ntop(AF_INET6, &(addr_in->sin6_addr), str, INET6_ADDRSTRLEN);
                 if(errnob)
                    syslog(pri,"%s:%s IP:%s ERRNO:%d",prefix,mes,str,errno);
                 else
                    syslog(pri,"%s:%s IP:%s",prefix,mes,str);
             }else{
                 if(errnob)
                    syslog(pri,"%s:%s ERRNO:%d",prefix,mes,errno);
                 else
                    syslog(pri,"%s:%s",prefix,mes);

             }
         }
     }
     public:
     void critical(const char* mes){
        syslog(3,"CRITICAL:%s ERRNO:%d",mes,errno);
     }
     void error(const char* mes){
        syslog(2,"ERROR:%s ERRNO:%d",mes,errno);
     }
     void warning(const char* mes){
        syslog(1,"WARNING:%s",mes);
     }
     void message(const char* mes){
        syslog(0,"MESS:%s",mes);
     }
     void debug(const char* mes){
        (void)mes;
        #ifdef DEBUGSERV
        syslog(0,"DEBUG:%s",mes);
        #endif
     }
     void client_error(const char* mes,struct sockaddr *addr){
        client_log(2,"ERROR",mes,addr,true);
     }
     void client_message(const char* mes,struct sockaddr *addr){
         client_log(0,"MESS",mes,addr);
     }
     void client_debug(const char* mes,struct sockaddr *addr){
        (void)mes;
        (void)addr;
        #ifdef DEBUGSERV
        client_log(0,"DEBUG",mes,addr);
        #endif
     }
     static Logger& Instance()
        {
                static Logger theSingleInstance;
                return theSingleInstance;
        }
private:
     Logger(){
        openlog("DNSProxy",0,LOG_DAEMON);
     }
     Logger(const Logger& root){(void)root;}
     Logger& operator=(const Logger& s){(void)s;return *this;}
};

#endif // LOGGER_H
