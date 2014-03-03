#include "HANDLER.h"
#include <unistd.h>
int HANDLER::get_descriptor()const{
    return sd;
}
void HANDLER::set_descriptor(int _sd){
    sd=_sd;
}

void HANDLER::close_d(){
    close(sd);
}
