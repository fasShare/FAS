#include <SigIgnore.h>
#include <signal.h>

SigIgnore::SigIgnore() {
  ::signal(SIGPIPE, SIG_IGN);
}

SigIgnore SigIgn;
