#ifndef EMUSHELL_INCLUDED_FREELIST_H
#define EMUSHELL_INCLUDED_FREELIST_H

#include <vector>
#include <mutex>

namespace EmuShell
{

template <typename T>
class FreeList
{
public:
    FreeList() : _count{} {}

    T alloc()
    {
        std::lock_guard<std::mutex> lock{_mutex};

        if (_free.empty())
        {
            return T(_count++);
        }
        else
        {
            T tmp = _free.back();
            _free.pop_back();
            return tmp;
        }
    }

    void dealloc(T value)
    {
        std::lock_guard<std::mutex> lock{_mutex};

        _free.push_back(value);
    }

private:
    std::mutex      _mutex;
    std::size_t     _count;
    std::vector<T>  _free;
};

}

#endif
