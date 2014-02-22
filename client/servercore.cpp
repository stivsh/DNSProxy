#include "servercore.h"

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
int ServerCore::start_server(){
    Logger::Instance().message("Start client server");
    ServerCore& server=Instance();
    server.start=true;
    while(server.start){
        Logger::Instance().message("Reading client options file");
        OptionReader::LoadFromFile("/etc/dnsproxy/dnsproxyclient.config");
        Logger::Instance().message(OptionReader::as_string().c_str());
        Logger::Instance().message("Start client loop");
        server.restart=false;
        server.start_loop();
        Logger::Instance().message("Stop client loop");
        if(server.start)
            usleep(120*1000);
    }
    Logger::Instance().message("Stop client server");
    return 0;
}
    std::map<unsigned short,UDPClientData> indent_to_old;
    int const buff_max=2048;
    char _inbuff[buff_max];
    char *inbuff;
    unsigned short inpos;
    char _obuff[buff_max];
    char *obuff;
    unsigned short opos;
    fd_set readset;
    fd_set writeset;
    fd_set exceptset;
    int serv_sock;
    int udp_listener;
    time_t last_check;

    void WatchDogHandler(){
        close(serv_sock);
        close(udp_listener);
        Logger::Instance().critical("dnsproxy client watch dog event");
        exit(1);
    }

void reg_udp_sock(){
    struct sockaddr_in uaddr;
    uaddr.sin_family = AF_INET;
    uaddr.sin_port = htons(3425);
    uaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    udp_listener = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_listener < 0)
    {
        Logger::Instance().critical("udp socket error");
        exit(1);
    }
    fcntl(udp_listener, F_SETFL, O_NONBLOCK);
    if(bind(udp_listener, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0)
    {
        Logger::Instance().critical("udp bind error");
        exit(2);
    }
}
unsigned short get_new_ident(unsigned short old){
    srand(old);
    for(int i=0; i!=10;++i){
        unsigned short new_id=rand()%std::numeric_limits<unsigned short>::max();
        if(!indent_to_old.count(new_id)&&new_id>0){
            return new_id;
        }
    }
    return 0;
}

void reg_tcp_sock(){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080); //TODO опт. или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock < 0)
    {
        Logger::Instance().critical("reg tcp socket error");
        exit(1);
    }
    if(connect(serv_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        Logger::Instance().critical("connection error");
        exit(2);
    }
    if(fcntl(serv_sock, F_SETFL, O_NONBLOCK)==-1){
        Logger::Instance().critical("fcntl error. nonblock");
        exit(2);
    }
}
void zero_sets(){
    FD_ZERO(&readset);
    FD_ZERO(&writeset);
    FD_ZERO(&exceptset);
}
void fill_sets(){
    FD_SET(serv_sock, &readset);
    if(opos)FD_SET(serv_sock, &writeset);
    FD_SET(serv_sock, &exceptset);
    FD_SET(udp_listener, &readset);
    FD_SET(udp_listener, &exceptset);
}

void tcp_error(){
    Logger::Instance().critical("tcp error");
    exit(4);
}
void udp_error(){
    Logger::Instance().critical("udp error");
    exit(5);
}
void try_to_send_response(){
    if(inpos<=sizeof(short)){
        Logger::Instance().message("not enoth bytes in input buffer");
        return;
    }
    unsigned short len;
    memcpy(&len,inbuff,sizeof(short));
    len=ntohs(len);
    if(len>(inpos-sizeof(short))){
        Logger::Instance().message("len of message greater than inbuff size");
        return;
    }
    unsigned short new_id;
    memcpy(&new_id,inbuff+sizeof(short),sizeof(short));
    new_id=ntohs(new_id);
    if(!indent_to_old.count(new_id)){
        Logger::Instance().message("there is no id for message in map");
        return;
    }
    UDPClientData client_data=indent_to_old[new_id];
    indent_to_old.erase(new_id);
    client_data.mess_id=htons(client_data.mess_id);
    memcpy(inbuff+sizeof(short),&(client_data.mess_id),sizeof(short));
    ssize_t bytes_writen=sendto(udp_listener,inbuff+sizeof(short),len,0,&client_data.from,client_data.fromlen);
    if(bytes_writen<len){
        char buf[1024];
        sprintf(buf,"mess size:%d writen:%l",len,bytes_writen);
        Logger::Instance().message(buf);
    }
    memmove(inbuff,inbuff+sizeof(short)+len,inpos-(sizeof(short)+len));
    inpos-=sizeof(short)+len;
}
void tct_ready_read(){
    int bytesr=recv(serv_sock,inbuff+inpos,buff_max-inpos,0);
    if(bytesr==0){
        perror("server drop connection");
        reg_tcp_sock();
    }else if(bytesr<0){
        perror("tcp reading error");
        exit(9);
    }else if(bytesr>0){
        printf("new message");
        inpos+=bytesr;
        try_to_send_response();
    }

}
void try_to_write_tcp(){
    if(opos<=sizeof(short))return;
    int bytesw=send(serv_sock,obuff,opos,0);
    if(bytesw==0){
        Logger::Instance().message("0 bytes writen to server");
    }else if(bytesw<0){
        Logger::Instance().message("TCP write error");
        exit(7);
    }else if(bytesw>0){
        memmove(obuff,obuff+bytesw,opos-bytesw);
        opos-=bytesw;
    }
}
void tcp_ready_write(){
    try_to_write_tcp();
}
void udp_ready_read(){
    UDPClientData cl_data;
    char _tm_buff[buff_max];
    char *tm_buff=_tm_buff;
    int bytesc=recvfrom(udp_listener,tm_buff,buff_max,0,&cl_data.from,&cl_data.fromlen);
    if(bytesc==-1){
        Logger::Instance().message("udp read error");
        exit(6);
    }else if(bytesc>(int)sizeof(short)){
        if((int)(opos+sizeof(short)+bytesc)>buff_max){
            Logger::Instance().message("input buffer overflow");
            return;
        }
        memcpy(&(cl_data.mess_id),tm_buff,sizeof(short));
        cl_data.mess_id=ntohs(cl_data.mess_id);
        unsigned short new_id=get_new_ident(cl_data.mess_id);
        if(new_id==0){
            Logger::Instance().message("Can't create new id for message");
            return;
        }
        indent_to_old[new_id]=cl_data;
        unsigned short netnew_id=htons(new_id);
        memcpy(tm_buff,&netnew_id,sizeof(short));
        unsigned short nbytesc=htons(bytesc);
        memcpy(obuff+opos,(char*)(&nbytesc),sizeof(short));
        opos+=sizeof(short);
        memcpy(obuff+opos,tm_buff,bytesc);
        opos+=bytesc;
        try_to_write_tcp();
    }
}
void evalute_handlers(){
    if(FD_ISSET(serv_sock, &readset))tct_ready_read();
    if(FD_ISSET(udp_listener, &readset))udp_ready_read();
    if(FD_ISSET(serv_sock, &writeset))tcp_ready_write();
    if(FD_ISSET(serv_sock, &exceptset))tcp_error();
    if(FD_ISSET(udp_listener, &exceptset))udp_error();
}

void check_time_stamps(){
    time_t now;
    time(&now);
    if(difftime(now,last_check)<12)return;
    std::vector<unsigned short> ids_to_delete;
    for(std::map<unsigned short,UDPClientData>::iterator it=indent_to_old.begin();it!=indent_to_old.end();++it){
        if(difftime(now,it->second.time_stamp)>10){
            ids_to_delete.push_back(it->first);
        }
    }
    for(std::vector<unsigned short>::iterator it=ids_to_delete.begin();it!=ids_to_delete.end();++it){
        indent_to_old.erase(*it);
    }
    time(&last_check);
}
void ServerCore::start_loop(){
    WatchDogTimer wdtimer(2,WatchDogHandler);
    inbuff=_inbuff;
    obuff=_obuff;
    indent_to_old.clear();
    inpos=0;
    opos=0;
    time(&last_check);
    //сокет сервера
    reg_tcp_sock();
    //сокет сервера
    reg_udp_sock();
    while(!restart){
        wdtimer.reset_timer();
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        zero_sets();
        fill_sets();
        int select_ret=select(((serv_sock>udp_listener)?serv_sock:udp_listener)+1,&readset,&writeset,&exceptset,&timeout);
        if(select_ret<0){
            perror("select");
            exit(3);
        }else if(select_ret>0){
            evalute_handlers();
        }
        check_time_stamps();
    }
    close(serv_sock);
    close(udp_listener);
}
