#ifndef _BLING_WEAKCALLBACK_H_
#define _BLING_WEAKCALLBACK_H_

#include <functional>
#include <memory>

namespace bling
{

template<typename T, typename... Args>
class WeakCallback 
{
public:
    WeakCallback(const std::weak_ptr<T>& object, const std::function<void (T* , Args...)>& func)
        : _object(object), _function(func)
    {
    }

    void operator()(Args&&... args) const
    {
        std::shared_ptr<T> ptr(_object.lock());
        if (ptr)
        {
            _function(ptr.get(), std::forward<Args>(args)...);
        }
    }

private:
    std::weak_ptr<T> _object;
    std::function<void (T*, Args...)> _function;
}; // class WeakCallback

template<typename T, typename... Args>
WeakCallback<T, Args...> makeWeakCallback(const std::shared_ptr<T>& object, void (T::*func)(Args...))
{
    return WeakCallback<T, Args...>(object, func);
}

template<typename T, typename... Args>
WeakCallback<T, Args...> makeWeakCallback(const std::shared_ptr<T>& object, void (T::*func)(Args...) const)
{
    return WeakCallback<T, Args...>(object, func);
}

}; //ns bling


#endif // _BLING_WEAKCALLBACK_H_
