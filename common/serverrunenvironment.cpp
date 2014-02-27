#include "serverrunenvironment.h"
#include <execinfo.h>
void crit_sighandler(int sig, siginfo_t *info, void *context){
    void* ErrorAddr;
    void* Trace[16];
    int    x;
    int    TraceSize;
    char** Messages;
    // запишем в лог что за сигнал пришел
    Logger::critical()<<"Signal:"<<strsignal(sig)<<", "<<info->si_addr<<Logger::endl;

    #if __WORDSIZE == 64 // если дело имеем с 64 битной ОС
        // получим адрес инструкции которая вызвала ошибку
        ErrorAddr = (void*)((ucontext_t*)context)->uc_mcontext.gregs[REG_RIP];
    #else
        // получим адрес инструкции которая вызвала ошибку
        ErrorAddr = (void*)((ucontext_t*)context)->uc_mcontext.gregs[REG_EIP];
    #endif

    // произведем backtrace чтобы получить весь стек вызовов
    TraceSize = backtrace(Trace, 16);
    Trace[1] = ErrorAddr;

    // получим расшифровку трасировки
    Messages = backtrace_symbols(Trace, TraceSize);
    if (Messages){
        Logger::message()<<"== Backtrace =="<<Logger::endl;
        for (x = 1; x < TraceSize; x++)
            Logger::message()<<Messages[x]<<Logger::endl;
        Logger::message()<<"== End Backtrace =="<<Logger::endl;
        free(Messages);
    }
    exit(8);
}
