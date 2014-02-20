#ifndef OPTION_READER_H
#define OPTION_READER_H
#include <map>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
class OptionReader{
    typedef std::map<std::string,unsigned int> int_optst;
    typedef std::map<std::string,std::string>  str_optst;
    int_optst int_opt;
    str_optst str_opt;
    static OptionReader& Instance(){
                    static OptionReader theSingleInstance;
                    return theSingleInstance;
            }
    void default_opt(){
        str_opt["dns_server"]="8.8.8.8";
        str_opt["listen_addr"]="0.0.0.0";
        int_opt["client_no_reply_time"]=8000;
        int_opt["listen_port"]=80;
    }
    OptionReader(){
        default_opt();
    }
    OptionReader(const OptionReader& root){(void)root;}
    OptionReader& operator=(const OptionReader& s){(void)s;return *this;}
    void add_param(std::string &name,std::string &val){
        if(str_opt.count(name)){
            str_opt[name]=val;
        }
        if(int_opt.count(name)){
            int_opt[name]=atoi(val.c_str());

        }
    }
    void parce_pars_line(std::string &str){
       std::string name;
       std::string val;
       str.erase(std::remove_if(str.begin(), str.end(), isspace),str.end());
       if(str[0]=='#')return;
       std::stringstream ss(str);
       if(!getline(ss,name,'='))return;
       if(!getline(ss,val,'='))return;
       add_param(name,val);
    }


public:
    static std::string as_string(){
        std::stringstream ss;
        str_optst &str_opt=Instance().str_opt;
        int_optst &int_opt=Instance().int_opt;
        ss<<"string options:";
        for(str_optst::iterator it=str_opt.begin();it!=str_opt.end();++it){
            ss<<it->first<<'='<<it->second<<';';
        }
        ss<<"int options:";
        for(int_optst::iterator it=int_opt.begin();it!=int_opt.end();++it){
            ss<<it->first<<'='<<it->second<<';';
        }
        return ss.str();
    }
    static void LoadFromFile(const char* file_name){
        Instance().default_opt();
        std::ifstream ifs;
        ifs.open(file_name);
        while(ifs.good()&&!ifs.eof()){
            std::string line;
            std::getline(ifs,line);
            Instance().parce_pars_line(line);
        }

    }
    static std::string& get_str_opt(const char* key){
        return Instance().str_opt[key];
    }
    static const char* get_cstr_opt(const char* key){
        return get_str_opt(key).c_str();
    }
    static int get_int_opt(const char* key){
        return Instance().int_opt[key];
    }
};

#endif
