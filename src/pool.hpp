#ifndef INCLUDED_POOL_HPP
#define INCLUDED_POOL_HPP

#include <vector>

namespace entity {

    // Base class so we can have a vector of pools containing different elements.
    class BasePool
    {
    public:
        virtual ~BasePool() {};
        virtual void clear() = 0;
    };

    // A pool is just a vector (contiguous data).
    template <typename T>
    class Pool : public BasePool {
    public:
        Pool(int initial_capacity = 100) { reserve(initial_capacity); }
        virtual ~Pool() {};

        int get_capacity() const { return data.capacity(); }
        int get_size() const { return data.size(); }
        bool is_empty() const { return data.empty(); }

        bool set(int index, T object) {
            assert(index < get_capacity());
            data[index] = object;
            return true;
        }

        T& get(int index) {
            assert(index < get_capacity());
            return (T &)data[index];
        }

        void add(T object) { data.push_back(object); }
        void reserve(int n) { data.reserve(n); }
        void clear() { data.clear(); }
        std::vector<T> get_data() const { return data; }

    private:
        std::vector<T> data;
    };

}

#endif
