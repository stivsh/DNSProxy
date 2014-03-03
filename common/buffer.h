#ifndef BUFFER_H
#define BUFFER_H
#include <sys/socket.h>
#include <cstring>
#include "HANDLER.h"
#include <vector>
const size_t buff_def_size=1024;
class Buffer{
    size_t position;
    std::vector<char> buffer_arr;
    char* buff;
public:
    Buffer(size_t size=buff_def_size);
    void clear();
    const char* pointer()const;
    size_t pos() const;
    size_t max() const;
    void delete_front(size_t bytes);
    void delete_back(size_t bytes);
    ssize_t write_to_buffer(const void* pointer, size_t len);
    int recvh(HANDLER &hand);
    int sendh(HANDLER &hand);
};

#endif // BUFFER_H
