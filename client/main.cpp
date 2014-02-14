#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <map>
#include <limits>
#include <fcntl.h>
//nslookup -port=3425 mail.ru 127.0.0.1
class UDPClientData{
    public:
    unsigned short mess_id;
    struct sockaddr from;
    socklen_t fromlen;
    time_t time_stamp;
    UDPClientData(){time(&time_stamp);}
};
std::map<unsigned short,UDPClientData> indent_to_old;
int const buff_max=2048;
char *inbuff;
unsigned short inpos;
char *obuff;
unsigned short opos;
fd_set readset;
fd_set writeset;
fd_set exceptset;

int serv_sock;
int udp_listener;
void reg_udp_sock(){
    struct sockaddr_in uaddr;
    uaddr.sin_family = AF_INET;
    uaddr.sin_port = htons(3425);
    uaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    udp_listener = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_listener < 0)
    {
        perror("udp socket");
        exit(1);
    }
    fcntl(udp_listener, F_SETFL, O_NONBLOCK);
    if(bind(udp_listener, (struct sockaddr *)&uaddr, sizeof(uaddr)) < 0)
    {
        perror("udp bind");
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
    addr.sin_port = htons(8080); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock < 0)
    {
        perror("socket");
        exit(1);
    }
    if(connect(serv_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    if(fcntl(serv_sock, F_SETFL, O_NONBLOCK)==-1){
        perror("fcntl error. nonblock");
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
    perror("tcp error");
    exit(4);
}
void udp_error(){
    perror("udp error");
    exit(5);
}
void try_to_send_response(){
    if(inpos<=sizeof(short))return;
    unsigned short len=ntohs(*reinterpret_cast<unsigned short*>(inbuff));
    if(len>(inpos-sizeof(short)))return;
    unsigned short new_id=ntohs(*reinterpret_cast<unsigned short*>(inbuff+sizeof(short)));
    if(!indent_to_old.count(new_id))return;
    UDPClientData client_data=indent_to_old[new_id];
    *reinterpret_cast<unsigned short*>(inbuff+sizeof(short))=htons(client_data.mess_id);
    sendto(udp_listener,inbuff+sizeof(short),len,0,&client_data.from,client_data.fromlen);
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
        inpos+=bytesr;
        try_to_send_response();
    }

}
void try_to_write_tcp(){
    if(opos<=sizeof(short))return;
    int bytesw=send(serv_sock,obuff,opos,0);
    if(bytesw==0){
    }else if(bytesw<0){
        perror("TCP write error");
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
    char *tm_buff=new char[buff_max];
    int bytesc=recvfrom(udp_listener,tm_buff,buff_max,0,&cl_data.from,&cl_data.fromlen);
    if(bytesc==-1){
        perror("udp read error");
        exit(6);
    }else if(bytesc>(int)sizeof(short)){
        if((int)(opos+sizeof(short)+bytesc)>buff_max)return;
        //обработка
        cl_data.mess_id=ntohs(*reinterpret_cast<unsigned short*>(tm_buff));
        unsigned short new_id=get_new_ident(cl_data.mess_id);
        if(new_id==0){
            delete [] tm_buff;
            return;
        }
        indent_to_old[new_id]=cl_data;
        *reinterpret_cast<unsigned short*>(tm_buff)=htons(new_id);
        unsigned short nbytesc=htons(bytesc);
        memcpy(obuff+opos,(char*)(&nbytesc),sizeof(short));
        opos+=sizeof(short);
        memcpy(obuff+opos,tm_buff,bytesc);
        opos+=bytesc;
        try_to_write_tcp();
    }
    delete [] tm_buff;
}
void evalute_handlers(){
    if(FD_ISSET(serv_sock, &readset))tct_ready_read();
    if(FD_ISSET(udp_listener, &readset))udp_ready_read();
    if(FD_ISSET(serv_sock, &writeset))tcp_ready_write();
    if(FD_ISSET(serv_sock, &exceptset))tcp_error();
    if(FD_ISSET(udp_listener, &exceptset))udp_error();
}
time_t last_check;
void check_time_stamps(){
    time_t now;
    time(&now);
    if(difftime(now,last_check)<12)return;
    std::vector<unsigned short> ids_to_delete;
    for(std::map<unsigned short,UDPClientData>::iterator it=indent_to_old.begin();it!=indent_to_old.end();++it){
        if(difftime(now,it->second.time_stamp)>10)
            ids_to_delete.push_back(it->first);
    }
    for(std::vector<unsigned short>::iterator it=ids_to_delete.begin();it!=ids_to_delete.end();++it){
        indent_to_old.erase(*it);
    }
    time(&last_check);
}

int main(int argc, char *argv[]){
    (void)argc;
    (void)argv;
    time(&last_check);
    inbuff=new char[buff_max];inpos=0;
    obuff=new char[buff_max];opos=0;
    //сокет сервера
    reg_tcp_sock();
    //сокет сервера
    reg_udp_sock();
    for(;;){
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

    delete [] inbuff;
    delete [] obuff;
    return 0;
}
