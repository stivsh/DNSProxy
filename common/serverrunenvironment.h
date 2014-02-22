#ifndef SERVERRUNENVIRONMENT_H
#define SERVERRUNENVIRONMENT_H
#include <signal.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/wait.h>
#include "logger.h"
#include "commondefines.h"
template<class Server>
void sighendler(int sig){
    if(SIGTERM==sig||sig==SIGINT){//заверщение
        Logger::Instance().message("Signal SIGTERM|SIGINT cath");
        Server::external_command(STOP);

    }else if(SIGHUP==sig){//перезагрузка
        Logger::Instance().message("Signal SIGHUP cath");
        Server::external_command(RESTART);
    }
}
/*void crit_sighandler(int sig){
    (void)sig;
    exit(1);
    //TODO stack trace to log
}*/
template<class Server>
class ServerRunEnvironment{
    std::string pid_file;
    int start_server(){
        //critical signals
       // signal(SIGFPE,crit_sighandler);
       // signal(SIGILL,crit_sighandler);
        //signal(SIGSEGV,crit_sighandler);
        //signal(SIGBUS,crit_sighandler);
        //control signals
        signal(SIGTERM, sighendler<Server>);
        signal(SIGINT, sighendler<Server>);
        signal(SIGHUP, sighendler<Server>);
        return Server::start_server();
    }

    pid_t start_server_in_other_thread(){
        pid_t pid=fork();
        if(pid==-1){
            Logger::Instance().critical("Can't create thread!");
        }else if(!pid){
            exit(start_server());
        }else if(pid>0){
            FILE* f;
            f = fopen(pid_file.c_str(), "w");//server name
            if(f){
                fprintf(f, "%u", pid);
                fclose(f);
            }
        }
        return pid;
    }
    int run_control_thread(){
        for(;;){
            remove(pid_file.c_str());
            Logger::Instance().message("Running Demon");
            pid_t pid=start_server_in_other_thread();
            if(pid>0){
                int status;
                waitpid(pid,&status,0);
                if(WIFEXITED(status)&&!WEXITSTATUS(status)){
                    Logger::Instance().message("Demon closed sucsessful");
                    return 0;
                }
                usleep(60*1000);
            }else{
                return -1;
            }
        }
        return 0;
    }
    void run_as_demon(){
        int pid = fork();
        if(pid==-1){
            perror("Error: Start Daemon failed");
            Logger::Instance().critical("Can't create thread!");
            exit(1);
        }else if(!pid){
            umask(0);
            setsid();
            chdir("/");
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            exit(run_control_thread());
        }else if(pid>0){
        }
    }
    void build_server_strings(const char* server_mame){
        std::stringstream ss;
        ss<<"/var/run/"<<server_mame<<".pid";
        pid_file=ss.str();

    }

public:
    int main(int argc, char *argv[],const char* server_mame){
        build_server_strings(server_mame);
        bool deamon=true;
        if(argc>1){
            for(int i=1;i!=argc;++i){
                char* par=argv[i];
                if(strcmp("-c",par)==0){
                    deamon=false;
                }
            }
        }
        if(deamon){
            run_as_demon();
        }else{
            return start_server();
        }
        return 0;
    }
};
#endif // SERVERRUNENVIRONMENT_H
