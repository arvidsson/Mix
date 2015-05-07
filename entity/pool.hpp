#pragma once

#include <vector>
#include <cassert>

namespace entity
{

    // Base class so we can have a vector of pools containing different object types.
    class BasePool
    {
    public:
        virtual ~BasePool() {};
        virtual void clear() = 0;
    };

    // A pool is just a vector (contiguous data) of objects of type T.
    template <typename T>
    class Pool : public BasePool
    {
    public:
        Pool(int size = 100)
        {
            resize(size);
        }

        virtual ~Pool() {};

        bool is_empty() const
        {
            return data.empty();
        }

        unsigned int get_size() const
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
            assert(index < get_size());
            data[index] = object;
            return true;
        }

        T& get(unsigned int index)
        {
            assert(index < get_size());
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

    private:
        std::vector<T> data;
    };

}
