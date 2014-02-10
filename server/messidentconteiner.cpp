#include "messidentconteiner.h"
unsigned short MessIdentConteiner::get_new_id(unsigned short id, EventHandler* handler){
    srand(id+rand());
    for(int i=0;i!=10;++i){
        unsigned short n_id=rand()%std::numeric_limits<unsigned short>::max();
        if(!messages_ids.count(n_id)&&n_id>0){
            new_id_to_original_id[n_id]=id;
            id_to_handler[n_id]=handler;
            return n_id;
        }
    }
    return 0;
}
void MessIdentConteiner::delete_all_id_ralated_to(EventHandler* handler){
    std::set<unsigned short> ids_to_remove;
    for(std::map<unsigned short,EventHandler*>::iterator it=id_to_handler.begin();
            it!=id_to_handler.end();++it){
        if(it->second==handler)
            ids_to_remove.insert(it->first);
    }
    for(std::set<unsigned short>::iterator it=ids_to_remove.begin();
        it!=ids_to_remove.end();++it){
        release_id(*it);
    }
}
void MessIdentConteiner::release_id(unsigned short id){
    messages_ids.erase(id);
    new_id_to_original_id.erase(id);
    id_to_handler.erase(id);
}
EventHandler* MessIdentConteiner::get_handler(unsigned short id){
    if(id_to_handler.count(id))
        return id_to_handler[id];
    return 0;
}
unsigned short MessIdentConteiner::get_orig_id(unsigned short id){
    if(new_id_to_original_id.count(id))
        return new_id_to_original_id[id];
    return 0;
}
