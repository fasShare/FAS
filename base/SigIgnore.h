#ifndef FAS_SIGIGNORE
#define FAS_SIGIGNORE

namespace fas {
/*!
 * \brief The SigIgnore class
 * We can define a global SigIgnore variable to ignore the signal.
 * see ::signal(SIGPIPE, SIG_IGN) in SigIgnore ctor.
 */
class SigIgnore {
public:
  SigIgnore();
};

}
#endif // FAS_SIGIGNORE

