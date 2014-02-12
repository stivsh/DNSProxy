#ifndef MESSIDENTCONTEINER_H
#define MESSIDENTCONTEINER_H
#include <map>
#include <set>
#include <cstdlib>
#include <limits>
#include <ctime>
#include <vector>
class EventHandler;
class MessIdentConteiner{
    time_t last_check;
    std::set<unsigned short> messages_ids;
    std::map<unsigned short,time_t> time_stamps;
    std::map<unsigned short,unsigned short> new_id_to_original_id;
    std::map<unsigned short,EventHandler*> id_to_handler;
public:
    MessIdentConteiner();
    unsigned short get_new_id(unsigned short id, EventHandler* handler);
    void delete_all_id_ralated_to(EventHandler* handler);
    void release_id(unsigned short id);
    EventHandler* get_handler(unsigned short id);
    unsigned short get_orig_id(unsigned short id);
    void check_time_out();

};

#endif // MESSIDENTCONTEINER_H
