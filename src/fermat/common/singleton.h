#ifndef FERMAT_COMMON_SINGLETON_H_
#define FERMAT_COMMON_SINGLETON_H_

#include <cstdlib>
#include <fermat/common/mutex.h>

namespace fermat {

template <typename Instance, bool destroy_on_exit = true>
class Singleton {
    typedef Instance instance_type;
    typedef  instance_type* instance_pointer;
    typedef volatile instance_pointer volatile_instance_pointer;
public:
    Singleton() {}

    instance_pointer instance()
    {
        return get_instance();
    }
    
    /// non-copyable: delete copy-constructor
    inline static instance_pointer get_instance() {
        if (!_instance)
            return create_instance();

        return _instance;
    }
private:
    Singleton(const Singleton&);;
    /// non-copyable: delete assignment operator
    Singleton& operator = (const Singleton&);
private:
    static volatile_instance_pointer _instance;

    static instance_pointer create_instance();
    static void destroy_instance();
};

template <typename Instance, bool destroy_on_exit>
typename Singleton<Instance, destroy_on_exit>::instance_pointer
Singleton<Instance, destroy_on_exit>::create_instance() 
{
    static Mutex create_mutex;
    ScopedMutex lock(create_mutex);
    if (!_instance) {
        _instance = new instance_type();
        if (destroy_on_exit)
            atexit(destroy_instance);
    }
    return _instance;
}

template <typename Instance, bool destroy_on_exit>
void Singleton<Instance, destroy_on_exit>::destroy_instance() 
{
    instance_pointer inst = _instance;
    // instance = nullptr;
    _instance = reinterpret_cast<instance_pointer>(size_t(-1));     // bomb if used again
    delete inst;
}

template <typename Instance, bool destroy_on_exit>
typename Singleton<Instance, destroy_on_exit>::volatile_instance_pointer Singleton<Instance, destroy_on_exit>::_instance = NULL;

}

#endif 
