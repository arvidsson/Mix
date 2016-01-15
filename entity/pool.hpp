#pragma once

#include <vector>
#include <cassert>

namespace entity
{

// Base class so we can have a vector of pools containing different object types.
class BasePool
{
public:
    virtual ~BasePool() {}
    virtual void Clear() = 0;
};

// A pool is just a vector (contiguous data) of objects of type T.
template <typename T>
class Pool : public BasePool
{
public:
    Pool(int size = 100)
    {
        Resize(size);
    }

    virtual ~Pool() {}

    bool IsEmpty() const
    {
        return data.empty();
    }

    unsigned int GetSize() const
    {
        return data.size();
    }

    void Resize(int n)
    {
        data.resize(n);
    }

    void Clear()
    {
        data.clear();
    }

    bool Set(unsigned int index, T object)
    {
        assert(index < GetSize());
        data[index] = object;
        return true;
    }

    T& Get(unsigned int index)
    {
        assert(index < GetSize());
        return static_cast<T&>(data[index]);
    }

    void Add(T object)
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

    std::vector<T> GetData()
    {
        return data;
    }

private:
    std::vector<T> data;
};

}
