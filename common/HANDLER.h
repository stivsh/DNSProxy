#ifndef HANDLER_H
#define HANDLER_H

class HANDLER{
    int sd;
public:
    int get_descriptor() const;
    void set_descriptor(int _sd);
    void close_d();
};

#endif // HANDLER_H
