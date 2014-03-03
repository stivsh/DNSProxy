#ifndef DNSPack_H
#define DNSPack_H
#include <stdint-gcc.h>
#include <netinet/in.h>
#include <cstring>
#include "../common/logger.h"
#include "buffer.h"
enum PARCER_RETURN{OK,NOT_ENOTH_DATA,WRONG_FORMAT,OVERFLOW};
const char* PARCER_RETURN_AS_STR(PARCER_RETURN pr);
uint16_t getU16FromNBuff(const char* buff);

typedef struct dns_header{
    private:
    uint16_t Identification;
    uint16_t QR:1;
    uint16_t Opcode:4;
    uint16_t AA:1;
    uint16_t TC:1;
    uint16_t RD:1;
    uint16_t RA:1;
    uint16_t Z:1;
    uint16_t AD:1;
    uint16_t CD:1;
    uint16_t Rcode:4;
    uint16_t Questions;
    uint16_t Answers;
    uint16_t Authority;
    uint16_t Additional;
    public:
    uint16_t get_ident(){
        return ntohs(Identification);
    }
    void set_ident(uint16_t i){
        Identification=htons(i);
    }
} DNSHeader;

class DNSPack{
        DNSHeader header;
        Buffer data;
        PARCER_RETURN load_mes(const char* pointer,size_t size){
            data.clear();
            if(size<sizeof(header))
                return NOT_ENOTH_DATA;
            memcpy(&header,pointer,sizeof(header));
            if(size>sizeof(header)){
                if(data.write_to_buffer(pointer+sizeof(header),size-sizeof(header))<0)
                    return OVERFLOW;
            }
            return OK;
        }

public:
    dns_header& get_header(){
        return header;
    }
    PARCER_RETURN load_from_buff(Buffer& buff, bool unic_massage_in_buf=false){
        if(unic_massage_in_buf){
            PARCER_RETURN pret=load_mes(buff.pointer(),buff.pos());
            if(pret==OK)
                buff.clear();
            return pret;
        }
        if(buff.pos()<sizeof(uint16_t)+sizeof(dns_header))
            return NOT_ENOTH_DATA;
        uint16_t dns_total_len=getU16FromNBuff(buff.pointer());
        if(dns_total_len<sizeof(dns_header)||dns_total_len>buff.max()-sizeof(uint16_t))
            return WRONG_FORMAT;

        PARCER_RETURN pret=load_mes(buff.pointer()+sizeof(uint16_t),dns_total_len);
        if(pret==OK)
            buff.delete_front(sizeof(uint16_t)+dns_total_len);
        return pret;
    }
    PARCER_RETURN write_to_buff(Buffer &buff, bool unic_massage_in_buf=false){
        size_t bytes_writen=0;
        if(!unic_massage_in_buf){
            uint16_t dns_total_len=sizeof(header)+data.pos();
            dns_total_len=htons(dns_total_len);
            if(buff.write_to_buffer(&dns_total_len,sizeof(dns_total_len))<0){
                return OVERFLOW;
            }
            bytes_writen+=sizeof(dns_total_len);
        }
        if(buff.write_to_buffer(&header,sizeof(header))<0){
            buff.delete_back(bytes_writen);
            return OVERFLOW;
        }
        bytes_writen+=sizeof(header);
        if(buff.write_to_buffer(data.pointer(),data.pos())<0){
            buff.delete_back(bytes_writen);
            return OVERFLOW;
        }
        return OK;
    }
    DNSPack(){
        Logger::message()<<"header size:"<<sizeof(DNSHeader)<<Logger::endl;
    }
};

#endif
