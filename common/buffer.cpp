#include "buffer.h"
// TODO throw; //
Buffer::Buffer(size_t size):position(0){
    buffer_arr.resize(size);
    buff=&buffer_arr[0];
}
void Buffer::clear(){
    position=0;
}

const char* Buffer::pointer()const{
    return buff;
}
size_t Buffer::pos() const{
    return position;
}
size_t Buffer::max() const{
    return buffer_arr.size();
}
void Buffer::delete_front(size_t bytes){
    position-=bytes;
    memcpy(buff,buff+bytes,position);
}
void Buffer::delete_back(size_t bytes){
    position-=bytes;
}
ssize_t Buffer::write_to_buffer(const void* pointer, size_t len){
    if(len>max()-position)
        return -1;
    memcpy(buff+position,pointer,len);
    position+=len;
    return len;
}
int Buffer::recvh(HANDLER &hand){
    int bytes_read=recv(hand.get_descriptor(), buff+position, max()-position, 0);
    if(bytes_read>0)position+=bytes_read;
    return bytes_read;
}
int Buffer::sendh(HANDLER &hand){
  int bytes_send=send(hand.get_descriptor(), buff, position, 0);
  if(bytes_send>0){
      if(static_cast<size_t>(bytes_send)==position){
          position=0;
      }else{
        delete_front(bytes_send);
      }
  }
  return bytes_send;

}
