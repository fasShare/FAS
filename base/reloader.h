#ifndef __COMMON_UTIL_RELOADER_H__
#define __COMMON_UTIL_RELOADER_H__

#include <sys/stat.h>
#include <string>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>



#define RELOADER_MEMORY_BARRIER() asm volatile("" ::: "memory")
const size_t MEM_SIZE = 1024ll * 1024 * 1024 * 3;

template < class T , class... Rest>
class reloader_t
{
private:
    time_t _tm;
    unsigned int _using_no;
    std::string _fname;
    T* _p_content[ 2 ];

public:
    reloader_t(const char* fname);
    ~reloader_t();
    int need_reload() const;
    int reload(Rest...);
    int load(Rest...);
    T* get_content();
    const T* get_content() const;
    std::string get_fname() const { return _fname; }
};

template < class T , class... Rest>
reloader_t< T , Rest...>::reloader_t(const char* fname)
{
    _tm = 0;
    _using_no = 1;
    _fname.clear();
    
    if (fname != NULL) {
        _fname = fname;
    }
    _p_content[ 0 ] = NULL;
    _p_content[ 1 ] = NULL;
}

template < class T , class... Rest>
reloader_t< T , Rest...>::~reloader_t()
{
    if (_p_content[ 0 ] != NULL) {
        delete _p_content[ 0 ];
        _p_content[ 0 ] = NULL;
    }
    if (_p_content[ 1 ] != NULL) {
        delete _p_content[ 1 ];
        _p_content[ 1 ] = NULL;
    }
}

template < class T , class... Rest>
int reloader_t< T , Rest...>::need_reload() const
{
    struct stat curr_stat;
    if ((stat(_fname.c_str(), &curr_stat ) != 0) 
            || ((curr_stat.st_mode & S_IFREG ) == 0)) {
        return -1;
    }

    if (curr_stat.st_mtime <= _tm ) {
        return 0;
    }
    return 1;
}
#include <malloc.h>
template < class T , class... Rest>
int reloader_t< T , Rest...>::load(Rest... para)
{
    if (_using_no != 0 && _using_no != 1) {
        return -1;
    }
    
    struct stat curr_stat;
    if ((stat(_fname.c_str(), &curr_stat) != 0)
            || ((curr_stat.st_mode & S_IFREG) == 0)) {
        return -1;
    }
    
    T* p_t = new ( std::nothrow ) T(para...);
    if (p_t == NULL) {
        return -1;
    }
    
    if (p_t->load(_fname) < 0) {
        delete p_t;
        return -1;
    }
 
    _tm = curr_stat.st_mtime;
    _p_content[ _using_no ] = p_t;

    return 1;
}


template < class T , class... Rest>
int reloader_t< T , Rest...>::reload(Rest... para)
{
    if (_using_no != 0 && _using_no != 1) {
        return -1;
    }
    
    struct stat curr_stat;
    if ((stat(_fname.c_str(), &curr_stat) != 0)
            || ((curr_stat.st_mode & S_IFREG) == 0)) {
        return -1;
    }
    
    T* p_t = new ( std::nothrow ) T(para...);
    if (p_t == NULL) {
        return -1;
    }
    
    if (p_t->load(_fname) < 0) {
        delete p_t;
        return -1;
    }

    int reload_no = 1 - _using_no;
    _tm = curr_stat.st_mtime;
    _p_content[ reload_no ] = p_t;
    
    RELOADER_MEMORY_BARRIER();
    __sync_synchronize();

    _using_no = reload_no;

    // 释放前一个使用中的buffer, 保证reload完成后_p_content[1 - using_no]为空
    int previous_no = 1 - _using_no;
    if (NULL != _p_content[previous_no]) {
        sleep(2);
        delete _p_content[previous_no];
        _p_content[previous_no] = NULL;
    }

    malloc_trim(MEM_SIZE);
    return 1;
}

template < class T , class... Rest>
T* reloader_t< T , Rest...>::get_content()
{
    //这段代码有并发问题
    //_using_no != 1 && _using_no !=0 不是原子的
    //而且这个检测意义不大,即使进了if分支也是return NULL,也会core
    /*if (_tm == 0 || (_using_no != 1 && _using_no != 0)) {
        return NULL;
    }*/
    return _p_content[ _using_no ];
}

template < class T , class... Rest>
const T* reloader_t< T , Rest...>::get_content() const
{    
    return get_content();
}

#endif
