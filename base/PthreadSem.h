#ifndef FAS_PTHREADSEM_H
#define FAS_PTHREADSEM_H
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <semaphore.h>

namespace fas {

class PthreadSem {
public:
    PthreadSem(const char *name, int oflag = O_CREAT, mode_t mode = O_RDWR, unsigned int value = 1);
    ~PthreadSem();
    bool init(unsigned int value);
    bool increment();
    bool decrement();
    bool trydecrement();

    std::string getName() const;
    bool isVaild() const;
private:
    bool vaild_;
    sem_t *sem_;
    std::string name_;
    //If pshared has the value 0  
    //then the semaphore is shared between the threads of a process, and should be located at some address that is  visi‐ ble  to  all  threads
    //If pshared is nonzero 
    //then the semaphore is shared between processes, and should  be located in a region of shared memory
    unsigned int value_;
};

}

#endif //FAS_POSIXSEM_H
