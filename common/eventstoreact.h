#ifndef EVENTSTOREACT_H
#define EVENTSTOREACT_H

class EventsToReact{
public:
    bool readready;
    bool writeready;
    bool exeption;
    EventsToReact():readready(false),writeready(false),exeption(false){}
    bool is_active(){
        return readready||writeready||exeption;
    }
};

#endif // EVENTSTOREACT_H
