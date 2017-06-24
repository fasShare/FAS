#include <SigIgnore.h>
#include <signal.h>

fas::SigIgnore::SigIgnore() {
    ::signal(SIGPIPE, SIG_IGN);
}

fas::SigIgnore SigIgn;
