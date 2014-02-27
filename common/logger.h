#ifndef LOGGER_H
#define LOGGER_H
#include <syslog.h>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <arpa/inet.h>
#ifdef DEBUGSERV
    const bool log_debug_mess=true;
#else
    const bool log_debug_mess=false;
#endif
class Logger{
    enum LOGGER_CONTROL{CRITICAL_M,ERROR_M,WARNING_M,MESSAGE_M,DEBUG_M,NULL_M};
    template<class T> friend Logger& operator<<(Logger& logger,T t);
    std::stringstream ss;
    LOGGER_CONTROL m_type;
    void flush(){
        switch(m_type){
            case CRITICAL_M:syslog(4,"Critical:%s",ss.str().c_str()); break;
            case ERROR_M:syslog(3,"Error:%s",ss.str().c_str()); break;
            case WARNING_M:syslog(2,"Warning:%s",ss.str().c_str()); break;
            case MESSAGE_M:syslog(1,"Message:%s",ss.str().c_str());break;
            case DEBUG_M:syslog(0,"Debug:%s",ss.str().c_str()); break;
            default: break;
        }
        ss.str("");
    }
public:
    static Logger& endl(Logger& logger){
        if( (log_debug_mess&&logger.m_type==DEBUG_M) || (logger.m_type!=DEBUG_M) )
            logger.flush();
        return logger;
    }

    Logger& operator<<(Logger& (*func)(Logger& logger)){

        return (*func)(*this);
    }
    Logger& operator<<(struct sockaddr addr){
        if(!log_debug_mess && m_type==Logger::DEBUG_M)
            return *this;
        char str[INET6_ADDRSTRLEN];
        in_port_t port;
        if(addr.sa_family==AF_INET){
            struct sockaddr_in* addr_in=(struct sockaddr_in*)&addr;
            inet_ntop(AF_INET, &(addr_in->sin_addr), str, INET_ADDRSTRLEN);
            port=addr_in->sin_port;
        }else if(addr.sa_family==AF_INET6){
            struct sockaddr_in6* addr_in=(struct sockaddr_in6*)&addr;
            inet_ntop(AF_INET6, &(addr_in->sin6_addr), str, INET6_ADDRSTRLEN);
            port=addr_in->sin6_port;
        }else{
            memset(str,0,INET6_ADDRSTRLEN);
        }
        ss<<" IP:"<<str<<" PORT:"<<port;
      return *this;
    }
    static Logger& Instance()
    {
            static Logger theSingleInstance;
            return theSingleInstance;
    }
    static Logger& critical(){
        Instance().m_type=CRITICAL_M;
        return Instance();
    }
    static Logger& error(){
        Instance().m_type=ERROR_M;
        return Instance();
    }
    static Logger& warning(){
        Instance().m_type=WARNING_M;
        return Instance();
    }
    static Logger& message(){
        Instance().m_type=MESSAGE_M;
        return Instance();
    }
    static Logger& debug(){
        Instance().m_type=DEBUG_M;
        return Instance();
    }
private:
     Logger():m_type(NULL_M){
        openlog("DNSProxy",0,LOG_DAEMON);
     }
     Logger(const Logger& root){(void)root;}
     Logger& operator=(const Logger& s){(void)s;return *this;}
};
template<class T>
Logger& operator<<(Logger& logger,T t){
    if( (log_debug_mess&&logger.m_type==Logger::DEBUG_M) || (logger.m_type!=Logger::DEBUG_M) )
        logger.ss<<t;
    return logger;
}
#endif // LOGGER_H
