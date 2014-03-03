#ifndef MESSIDENTCONTEINER_H
#define MESSIDENTCONTEINER_H

#include <ctime>
#include <map>
#include <set>
#include <stdint-gcc.h>

class EventHandler;
class MessIdentConteiner{
    time_t last_check;
    std::set<uint16_t> messages_ids;
    std::map<uint16_t,time_t> time_stamps;
    std::map<uint16_t,uint16_t> new_id_to_original_id;
    std::map<uint16_t,EventHandler*> id_to_handler;
public:
    MessIdentConteiner();
    uint16_t gen_new_id(uint16_t id, EventHandler* handler);
    void delete_all_id_ralated_to(EventHandler* handler);
    void release_id(uint16_t id);
    EventHandler* get_handler(uint16_t id);
    uint16_t get_orig_id(uint16_t id);
    void check_time_out();

};

#endif // MESSIDENTCONTEINER_H
