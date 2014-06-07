#ifndef __LIST_WRAPPER_H__
#define __LIST_WRAPPER_H__

#include <vector>

template <typename T>
class ListWrapper
{
    public:
        ListWrapper(T& t) : _list( {t} ) {}
        ListWrapper() {}

        unsigned int size() { return _list.size(); }
        T at(int i) { return _list[i]; }

        std::vector<T>& list() { return _list; }
        ListWrapper<T>* add(T& t) { _list.push_back(t); return this; }

    private:
        std::vector<T> _list;
};

#endif //__LIST_WRAPPER_H__
