#ifndef MESSIDENTCONTEINER_H
#define MESSIDENTCONTEINER_H
#include <map>
#include <set>
#include <cstdlib>
#include <limits>
//TODO возможно забивание
class EventHandler;
class MessIdentConteiner{
    std::set<unsigned short> messages_ids;
    std::map<unsigned short,unsigned short> new_id_to_original_id;
    std::map<unsigned short,EventHandler*> id_to_handler;
public:
    unsigned short get_new_id(unsigned short id, EventHandler* handler);
    void delete_all_id_ralated_to(EventHandler* handler);
    void release_id(unsigned short id);
    EventHandler* get_handler(unsigned short id);
    unsigned short get_orig_id(unsigned short id);
};

#endif // MESSIDENTCONTEINER_H
