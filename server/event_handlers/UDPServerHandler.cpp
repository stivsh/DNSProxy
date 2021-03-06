#include "UDPServerHandler.h"
#include "../servercore.h"
#include "../../common/OptionReader.h"
#include "../../common/buffer.h"
#include <fcntl.h>

UDPServerHandler::UDPServerHandler(){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock<0){
        errorflag=true;
        Logger::critical()<<"Can't create UDP sock ERRNO:"<<errno<<Logger::endl;
        ServerCore::CriticalError();
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = inet_addr(OptionReader::get_cstr_opt("dns_server"));
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr))==-1){
        errorflag=true;
        Logger::critical()<<"Can't create UDP connection ERRNO:"<<errno<<Logger::endl;
        ServerCore::CriticalError();
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    handler.set_descriptor(sock);
}
void UDPServerHandler::new_request(ClientHandler* ch,DNSPack request){
    uint16_t new_mess_id=mescont.gen_new_id(request.get_header().get_ident(),ch);
    if(new_mess_id==0){
        Logger::error()<<"Can't create new mess id"<<Logger::endl;
        return;
    }
    request.get_header().set_ident(new_mess_id);
    Buffer outbuff;
    if(request.write_to_buff(outbuff,true)!=OK){
        Logger::critical()<<"DNS output buffer overflow"<<Logger::endl;
        return;
    }
    outbuff.sendh(handler);
}
void UDPServerHandler::handler_removed(EventHandler* handler){
    mescont.delete_all_id_ralated_to(handler);
}

void UDPServerHandler::ready_read(){
    Buffer inbuff;
    int bytes_read =inbuff.recvh(handler);
    if(bytes_read<0){
        Logger::error()<<"Can't read UDP reply ERRNO:"<<errno<<Logger::endl;
    }
    if(bytes_read>0){
        DNSPack reply;
        PARCER_RETURN pret=reply.load_from_buff(inbuff,true);
        if(pret!=OK){
            Logger::error()<<"Can't read DNS reply:"<<PARCER_RETURN_AS_STR(pret)<<Logger::endl;
            return;
        }
        uint16_t message_id=reply.get_header().get_ident();
        uint16_t old_id=mescont.get_orig_id(message_id);
        ClientHandler* handler=static_cast<ClientHandler*>(mescont.get_handler(message_id));
        if(handler==0 || old_id==0 || !ServerCore::Instance().get_hfact().is_valid_pointer(handler)){
            return;
        }

        mescont.release_id(message_id);
        reply.get_header().set_ident(old_id);
        handler->new_reply_from_server(reply);
    }
}

void UDPServerHandler::exeption(){
    Logger::error()<<"UDP Handler exection ERRNO:"<<errno<<Logger::endl;
}

void UDPServerHandler::time_out(){
    mescont.check_time_out();
}
EventsToReact UDPServerHandler::events_to_react(){
    EventsToReact ev_react;
    ev_react.readready=true;
    ev_react.exeption=true;
    return ev_react;
}
bool UDPServerHandler::delete_this_handler(){
    return false;
}
