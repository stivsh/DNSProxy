#ifndef BUFFER_H
#define BUFFER_H
#include <sys/socket.h>
#include <cstring>
//TODO проверка переполнений
const size_t buff_def_size=1024;
class Buffer{
    size_t max_size;
    size_t position;
    char buffer_arr[buff_def_size];
    char* buff;
public:
    //throw; //
    Buffer():max_size(buff_def_size),position(0),buff(buffer_arr){}
    void clear(){
        position=0;
    }

    const char* pointer()const{
        return buff;
    }
    size_t pos() const{
        return position;
    }
    size_t max() const{
        return max_size;
    }
    void delete_front(size_t bytes){
        position-=bytes;
        memcpy(buff,buff+bytes,position);
    }
    void delete_back(size_t bytes){
        position-=bytes;
    }
    ssize_t write_to_buffer(const void* pointer, size_t len){
        if(max_size-position>len)
            return -1;
        memcpy(buff+position,pointer,len);
        position+=len;
        return len;
    }
    int recvsd(int sd){
        int bytes_read=recv(sd, buff+position, max_size-position, 0);
        if(bytes_read>0)position+=bytes_read;
        return bytes_read;
    }
    int sendsd(int sd){
      int bytes_send=send(sd, buff, position, 0);
      if(bytes_send>0){
          if(static_cast<size_t>(bytes_send)==position){
              position=0;
          }else{
            delete_front(bytes_send);
          }
      }
      return bytes_send;

    }
};

#endif // BUFFER_H
