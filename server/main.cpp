#include "servercore.h"
#include <signal.h>
#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "logger.h"
void sighendler(int sig){
    if(SIGTERM==sig||sig==SIGINT){//заверщение
        Logger::Instance().message("Signal SIGTERM|SIGINT cath");
        ServerCore::Instance().external_command(ServerCore::STOP);

    }else if(SIGHUP==sig){//перезагрузка
        Logger::Instance().message("Signal SIGHUP cath");
        ServerCore::Instance().external_command(ServerCore::RESTART);
    }
}
void critical_sighandler(int sig){
    (void)sig;
    exit(1);
    //TODO stack trace to log
}

int start_server(){
    //critical signals
    signal(SIGFPE,critical_sighandler);
    signal(SIGILL,critical_sighandler);
    signal(SIGSEGV,critical_sighandler);
    signal(SIGBUS,critical_sighandler);
    //control signals
    signal(SIGTERM, sighendler);
    signal(SIGINT, sighendler);
    signal(SIGHUP, sighendler);
    return ServerCore::start_server();
}
pid_t start_server_in_other_process(){
    pid_t pid=fork();
    if(pid==-1){
        Logger::Instance().critical("Can't create thread!");
    }else if(!pid){
        exit(start_server());
    }else if(pid>0){
        FILE* f;
        f = fopen("/var/run/dnsproxyserver.pid", "w");
        if(f){
            fprintf(f, "%u", pid);
            fclose(f);
        }
    }
    return pid;

}

int run_control(){
    for(;;){
        remove("/var/run/dnsproxyserver.pid");
        Logger::Instance().message("Running Demon");
        pid_t pid=start_server_in_other_process();
        if(pid>0){
            int status;
            waitpid(pid,&status,0);
            if(WIFEXITED(status)&&!WEXITSTATUS(status)){
                Logger::Instance().message("Demon closed sucsessful");
                return 0;
            }
            sleep(60);
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
        exit(run_control());
    }else if(pid>0){
    }
}
int main(int argc, char *argv[]){
    bool deamon=true;
    if(argc>1){
        for(int i=1;i!=argc;++i){
            char* par=argv[i];
            if(strcmp("-c",par)){
                deamon=false;
            }
        }
    }
    if(deamon){
        run_as_demon();
    }else{
        return start_server();
    }


}
