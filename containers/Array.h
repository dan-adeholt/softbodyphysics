#ifndef __ARRAY_H
#define __ARRAY_H

#include "./ContainerPlatform.h"
#include "../utils/MinMax.h"
#include "./Range.h"

template <typename T>
class Array
{
public:
    Array() : m_size(0), m_capacity(0), m_data(nullptr) {}
    Array(int size) : m_size(0), m_capacity(size), m_data(reinterpret_cast<T *>(new char[sizeof(T) * size])) {}

    ~Array()
    {
        clearAndFree();
    }

    T &operator[](int index) { return m_data[index]; }
    inline const T &operator[](int index) const { return m_data[index]; }

    inline int size() const { return m_size; }
    inline int capacity() const { return m_capacity; }

    inline Range<T> range(int start, int end) const
    {
        return Range<T>{&m_data[start], end - start};
    }

    inline Range<T> range(int start) const
    {
        return range(start, m_size);
    }

    inline Range<T> range() const
    {
        return range(0, m_size);
    }

    void push(const T &element)
    {
        if (m_size == m_capacity)
        {
            reserve(m_capacity * 2);
        }

        new (&m_data[m_size++]) T(element);
    }

    void clear()
    {
        // We cannot just call delete[] on the underlying storage
        // because we have excess capacity in the array. Not all of
        // these elements have been constructed, so we cannot call
        // their destructors. Therefore, invoke the destructors manually.
        for (int i = 0; i < m_size; i++)
        {
            m_data[i].~T();
        }

        m_size = 0;
    }

    void clearAndFree()
    {
        clear();
        // Reinterpret as char* in order to avoid calling destructors.
        delete[] reinterpret_cast<char *>(m_data);
        m_capacity = 0;
    }

    void reserve(int newCapacity)
    {
        if (m_capacity > 0 && newCapacity <= m_capacity)
        {
            return;
        }

        int actualCapacity = max(newCapacity, 2);

        T *newData = reinterpret_cast<T *>(new char[sizeof(T) * actualCapacity]);
        for (int i = 0; i < m_size; i++)
        {
            new (&newData[i]) T(m_data[i]);
        }

        int oldSize = m_size;
        clearAndFree();

        m_data = newData;
        m_capacity = actualCapacity;
        m_size = oldSize;
    }

private:
    int m_size;
    int m_capacity;
    T *m_data;
};

#endif