//nslookup -port=3425 mail.ru 127.0.0.1
#include "servercore.h"
#include "../common/serverrunenvironment.h"
int main(int argc, char *argv[]){
    ServerRunEnvironment<ServerCore> serv_env;
    return serv_env.main(argc,argv,"dnsproxyclient");
    return 0;
}
