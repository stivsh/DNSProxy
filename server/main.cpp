#include "servercore.h"
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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
int main(int argc, char *argv[]){
    Logger::Instance().message("Starting service--------------------");
    signal(SIGTERM, sighendler);
    signal(SIGINT, sighendler);
    signal(SIGHUP, sighendler);
    return ServerCore::start_server(argc,argv);
}
