#include "watchdogtimer.h"
bool WatchDogTimer::sync_var=false;
std::map<int,WDTInfo> WatchDogTimer::WDtimersInfo;
int WatchDogTimer::nextid=1;
bool WatchDogTimer::timer_registred=false;
timer_t WatchDogTimer::tid=0;

void timer_handler(int signo, siginfo_t *info,void *context){
  (void)signo;
  (void)context;
  if(info->si_code == SI_TIMER && !WatchDogTimer::sync_var){
      time_t now_t;time(&now_t);
      for(std::map<int,WDTInfo>::iterator it=WatchDogTimer::WDtimersInfo.begin();it!=WatchDogTimer::WDtimersInfo.end();++it){
          WDTInfo& wdinf=it->second;
          if(!wdinf.run)continue;
          if(difftime(now_t,wdinf.last_reset)>wdinf.time_to_death){
              (*wdinf.call_back_func)();
          }
      }
  }
}
WatchDogTimer::WatchDogTimer(unsigned int time_to_death,WatchDogTimerNotifyFunc call_back_func, bool start){
    WDTInfo new_wdinf;
    new_wdinf.id=id=++nextid;
    new_wdinf.time_to_death=time_to_death;
    new_wdinf.call_back_func=call_back_func;
    new_wdinf.last_reset=time(0);
    new_wdinf.run=false;
    sync_var=true;
    WDtimersInfo[id]=new_wdinf;
    if(start){
        start_timer();
    }
    sync_var=false;
}
void WatchDogTimer::reset_timer(){
    sync_var=true;
    if(WDtimersInfo.count(id)){
        WDtimersInfo[id].last_reset=time(0);
    }
    sync_var=false;
}
void WatchDogTimer::stop_timer(){
    sync_var=true;
    if(WDtimersInfo.count(id)){
        WDtimersInfo[id].run=false;
    }
    sync_var=false;
}
void WatchDogTimer::start_timer(){
    sync_var=true;
    if(WDtimersInfo.count(id)){
        WDtimersInfo[id].run=true;
    }
    if(!timer_registred){
        register_timer();
    }
    sync_var=false;
}
WatchDogTimer::~WatchDogTimer(){
    sync_var=true;
    WDtimersInfo.erase(id);
    if(!WDtimersInfo.size()){
        un_register_timer();
    }
    sync_var=false;
}
void WatchDogTimer::un_register_timer(){
    timer_registred=false;
    //отключаем таймер
    struct itimerspec ival;
    memset(&ival, 0, sizeof (ival));
    timer_settime(tid, 0, &ival, NULL);

}
void WatchDogTimer::register_timer(){
    timer_registred=true;
    sigset_t mask;
    struct sigevent sigev;
    struct sigaction sa;
    struct itimerspec ival;
    /* Мы не хотим никаких блокированных сигналов */
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    /* Регистрируем обработчик для SIGRTMIN */
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = timer_handler;
    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
      Logger::Instance().critical("WatchDog sigaction failed");
      exit(5);
    }
    /*По завершении работы таймера должен быть послан
      сигнал SIGRTMIN с произвольным значением 1
     */
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = SIGRTMIN;
    sigev.sigev_value.sival_int = 1;
    //Создаём таймер.
    if(timer_create(CLOCK_MONOTONIC, &sigev, &tid) == -1){
      Logger::Instance().critical("WatchDog timer_create");
      exit(6);
    }
    ival.it_value.tv_sec = 4;
    ival.it_value.tv_nsec = 0;
    ival.it_interval.tv_sec = 2;
    ival.it_interval.tv_nsec = 0;
    if (timer_settime(tid, 0, &ival, NULL) == -1){
      Logger::Instance().critical("WatchDog timer_settime");
      exit(7);
    }
}
