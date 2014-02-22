#include "servercore.h"
#include "../common/serverrunenvironment.h"
int main(int argc, char *argv[]){
    ServerRunEnvironment<ServerCore> serv_env;
    return serv_env.main(argc,argv,"dnsproxyserver");
    return 0;
}
