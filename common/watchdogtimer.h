#ifndef WATCHDOGTIMER_H
#define WATCHDOGTIMER_H

#include <map>
#include <time.h>
#include <signal.h>

typedef void (*WatchDogTimerNotifyFunc)();
void timer_handler(int signo, siginfo_t *info,void *context);
class WDTInfo{
    public:
    int id;
    unsigned int time_to_death;
    WatchDogTimerNotifyFunc call_back_func;
    bool run;
    time_t last_reset;
};
typedef void (*WatchDogTimerNotifyFunc)();
class WatchDogTimer{
    friend void timer_handler(int signo, siginfo_t *info,void *context);
    static bool sync_var;
    static std::map<int,WDTInfo> WDtimersInfo;
    static int nextid;
    static bool timer_registred;
    static timer_t tid;
    int id;
    void register_timer();
    void un_register_timer();
public:
    WatchDogTimer(unsigned int time_to_death,WatchDogTimerNotifyFunc call_back_func, bool start=true);
    void reset_timer();
    void stop_timer();
    void start_timer();
    ~WatchDogTimer();
};
#endif // WATCHDOGTIMER_H
