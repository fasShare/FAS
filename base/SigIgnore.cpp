#include <SigIgnore.h>
#include <signal.h>

using fas::SigIgnore;

SigIgnore::SigIgnore() {
  ::signal(SIGPIPE, SIG_IGN);
}

SigIgnore SigIgn;
