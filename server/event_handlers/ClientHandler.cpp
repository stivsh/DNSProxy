#include "ClientHandler.h"
#include "../servercore.h"
#include "../../common/OptionReader.h"
ClientHandler::ClientHandler(int sd_,UDPServerHandler* _uservh,struct sockaddr addr_):
    uservh(_uservh),addr(addr_){
    sd=sd_;
    time(&last_reply_to_client);
    Logger::message()<<"New connection"<<addr<<Logger::endl;
}
struct sockaddr* ClientHandler::get_addr(){
    return &addr;
}
void ClientHandler::drop_connection(){
    errorflag=true;
    Logger::debug()<<"Drop connection"<<addr<<Logger::endl;
    ServerCore::Instance().delete_handler_next_itration(this);
}

void ClientHandler::send_request_to_server(){
    DNSPack dns_query;
    PARCER_RETURN pret=OK;
    while(pret==OK){
      pret=dns_query.load_from_buff(input_buf);
      switch(pret){
        case WRONG_FORMAT:
            Logger::error()<<"input buffer wrong format. "<<addr<<Logger::endl;
            drop_connection();
            break;
        case OVERFLOW:
            Logger::error()<<"input buffer overflow. "<<addr<<Logger::endl;
            drop_connection();
            break;
        case OK:
            uservh->new_request(this,dns_query);
            break;
        default:
            break;
      }
    }
}
void ClientHandler::ready_read(){
    if(errorflag)return;
    int read_code = input_buf.recvsd(sd);
    if(read_code>0){
        Logger::debug()<<"receive data from client"<<addr<<Logger::endl;
        send_request_to_server();
    }else if(read_code==0){
        drop_connection();
    }else if(read_code<0){
        drop_connection();
        Logger::error()<<"Connection read error ERRNO:"<<errno<<addr<<Logger::endl;
    }
}

void ClientHandler::send_reply_to_client(){
    if(output_buf.pos()){
        int bytes_writen=output_buf.sendsd(sd);
        if(bytes_writen>0){
            Logger::debug()<<"Send reply to client"<<addr<<Logger::endl;
            if(!output_buf.pos()){
                time(&last_reply_to_client);
                Logger::debug()<<"All clients responses was transmited"<<addr<<Logger::endl;
            }
        }else if(bytes_writen<0){
            Logger::error()<<"Connection write error ERRNO:"<<errno<<addr<<Logger::endl;
            drop_connection();
        }else if(bytes_writen==0){

        }
    }
}
void ClientHandler::new_reply_from_server(DNSPack &reply){
    if(errorflag)return;
    PARCER_RETURN pret=reply.write_to_buff(output_buf);
    if(pret!=OK){
        Logger::error()<<"Can't write client reply to out buff:"<<PARCER_RETURN_AS_STR(pret)<<addr<<Logger::endl;
        drop_connection();
        return;
    }
    send_reply_to_client();
}
void ClientHandler::ready_write(){
    if(errorflag)return;
    send_reply_to_client();
}
void ClientHandler::exeption(){
    if(errorflag)return;
    drop_connection();

}
void ClientHandler::time_out(){
    if(errorflag)return;
    if(difftime(time(0),last_reply_to_client)>OptionReader::get_int_opt("client_no_reply_time")){
        Logger::message()<<"Drop connection. no activity"<<addr<<Logger::endl;
        drop_connection();
    }

}
void ClientHandler::fill_fd_sets(fd_set *readset,fd_set *writeset,fd_set *exceptset){
    FD_SET(sd, readset);
    FD_SET(sd, exceptset);
    if(output_buf.pos())
        FD_SET(sd, writeset);
}
