#include "dnspack.h"
const char* PARCER_RETURN_AS_STR(PARCER_RETURN pr){
    switch(pr){
        case OK: return "Successful execution"; break;
        case NOT_ENOTH_DATA: return "Not enoth data"; break;
        case WRONG_FORMAT: return "Wrong format"; break;
        case OVERFLOW: return "Buffer overflow"; break;
    }
    return "No value description";
}
uint16_t getU16FromNBuff(const char* buff){
    uint16_t val;
    memcpy(&val,buff,sizeof(uint16_t));
    return ntohs(val);
}
