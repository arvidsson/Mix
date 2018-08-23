#pragma once

#include <vector>
#include <cassert>

namespace Mix
{

// Required to have a vector of pools containing different object types.
class AbstractPool
{
public:
    virtual ~AbstractPool() {}
    virtual void clear() = 0;
};

// A pool is just a vector (contiguous data) of objects of type T.
template <typename T>
class Pool : public AbstractPool
{
public:
    Pool(int size = DEFAULT_POOL_SIZE)
    {
        resize(size);
    }

    virtual ~Pool() {}

    bool isEmpty() const
    {
        return data.empty();
    }

    unsigned int getSize() const
    {
        return data.size();
    }

    void resize(int n)
    {
        data.resize(n);
    }

    void clear()
    {
        data.clear();
    }

    bool set(unsigned int index, T object)
    {
        assert(index < getSize());
        data[index] = object;
        return true;
    }

    T& get(unsigned int index)
    {
        assert(index < getSize());
        return static_cast<T&>(data[index]);
    }

    void add(T object)
    {
        data.push_back(object);
    }

    T& operator[](unsigned int index)
    {
        return data[index];
    }

    const T& operator[](unsigned int index) const
    {
        return data[index];
    }

    std::vector<T> getData()
    {
        return data;
    }

private:
    std::vector<T> data;
};

}
