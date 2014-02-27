#include "messidentconteiner.h"
MessIdentConteiner::MessIdentConteiner(){
    time(&last_check);
}
uint16_t MessIdentConteiner::gen_new_id(uint16_t id, EventHandler* handler){
    srand(id+rand());
    for(int i=0;i!=10;++i){
        uint16_t n_id=rand()%std::numeric_limits<uint16_t>::max();
        if(!messages_ids.count(n_id)&&n_id>0){
            messages_ids.insert(n_id);
            time_stamps[n_id]=time(0);
            new_id_to_original_id[n_id]=id;
            id_to_handler[n_id]=handler;
            return n_id;
        }
    }
    return 0;
}
void MessIdentConteiner::delete_all_id_ralated_to(EventHandler* handler){
    std::vector<uint16_t> ids_to_remove;
    for(std::map<uint16_t,EventHandler*>::iterator it=id_to_handler.begin();
            it!=id_to_handler.end();++it){
        if(it->second==handler)
            ids_to_remove.push_back(it->first);
    }
    for(std::vector<uint16_t>::iterator it=ids_to_remove.begin();
        it!=ids_to_remove.end();++it){
        release_id(*it);
    }
}
void MessIdentConteiner::release_id(uint16_t id){
    messages_ids.erase(id);
    time_stamps.erase(id);
    new_id_to_original_id.erase(id);
    id_to_handler.erase(id);
}
EventHandler* MessIdentConteiner::get_handler(uint16_t id){
    if(id_to_handler.count(id))
        return id_to_handler[id];
    return 0;
}
uint16_t MessIdentConteiner::get_orig_id(uint16_t id){
    if(new_id_to_original_id.count(id))
        return new_id_to_original_id[id];
    return 0;
}
void MessIdentConteiner::check_time_out(){
    time_t now;
    time(&now);
    if(difftime(now,last_check)<12)return;
    std::vector<uint16_t> ids_to_remove;
    for(std::map<uint16_t,time_t>::iterator it=time_stamps.begin();it!=time_stamps.end();++it){
        if(difftime(now,it->second)>10)
            ids_to_remove.push_back(it->first);
    }
    for(std::vector<uint16_t>::iterator it=ids_to_remove.begin();it!=ids_to_remove.end();++it){
        release_id(*it);
    }
    time(&last_check);
}
