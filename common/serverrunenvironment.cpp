#include "serverrunenvironment.h"
#include <execinfo.h>
void crit_sighandler(int sig, siginfo_t *info, void *context){
    void* ErrorAddr;
    void* Trace[16];
    int    x;
    int    TraceSize;
    char** Messages;
    char buff[1024];

    // запишем в лог что за сигнал пришел
    sprintf(buff,"Signal: %s, %p\n", strsignal(sig), info->si_addr);
    Logger::Instance().critical(buff);

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
    if (Messages)
    {
        Logger::Instance().message("== Backtrace ==");

        // запишем в лог
        for (x = 1; x < TraceSize; x++)
        {
            sprintf(buff,"%s\n", Messages[x]);
            Logger::Instance().message(buff);
        }

        Logger::Instance().message("== End Backtrace ==\n");
        free(Messages);
    }
    exit(8);
}
