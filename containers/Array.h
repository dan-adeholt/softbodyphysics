#ifndef __ARRAY_H
#define __ARRAY_H

#include "./ContainerPlatform.h"
#include "./Range.h"
#include "./Span.h"
#include <initializer_list>

template <typename T>
class Array
{
public:
    Array() : m_size(0), m_capacity(0), m_data(nullptr) {}
    Array(const Array<T> &list) : m_size(list.size()), m_capacity(list.capacity()), m_data(reinterpret_cast<T *>(new char[sizeof(T) * list.size()]))
    {
        for (int i = 0; i < list.size(); i++)
        {
            auto &ref = list[i];
            new (&m_data[i++]) T(ref);
        }
    }

    Array(const std::initializer_list<T> &list) : m_size(list.size()), m_capacity(list.size()), m_data(reinterpret_cast<T *>(new char[sizeof(T) * list.size()]))
    {
        int i = 0;
        for (const T &element : list)
        {
            new (&m_data[i++]) T(element);
        }
    }

    Array(int size) : m_size(0), m_capacity(size), m_data(reinterpret_cast<T *>(new char[sizeof(T) * size])) {}

    ~Array()
    {
        clearAndFree();
    }

    inline T &operator[](int index) { return m_data[index]; }
    inline const T &operator[](int index) const { return m_data[index]; }

    inline int size() const { return m_size; }
    inline int capacity() const { return m_capacity; }

    inline Range<T> range(Span span) const
    {
        return Range<T>{&m_data[span.start], span.end - span.start};
    }

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
        return this->range(0, m_size);
    }

    void append(const Range<T> &other)
    {
        reserve(m_size + other.size);

        for (int i = 0; i < other.size; i++)
        {
            new (&m_data[m_size++]) T(other[i]);
        }
    }

    void append(const Array<T> &other)
    {
        reserve(m_size + other.size());

        for (int i = 0; i < other.size(); i++)
        {
            new (&m_data[m_size++]) T(other[i]);
        }
    }

    void append(const std::initializer_list<T> &other)
    {
        reserve(m_size + other.size());

        for (const T &element : other)
        {
            new (&m_data[m_size++]) T(element);
        }
    }

    void push(const T &element)
    {
        if (m_size == m_capacity)
        {
            reserve(m_capacity * 2);
        }

        new (&m_data[m_size++]) T(element);
    }

    void pop()
    {
        if (m_size > 0)
        {
            m_data[--m_size].~T();
        }
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

        int actualCapacity = newCapacity < 2 ? 2 : newCapacity;

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

    void fill(const T &element, int size)
    {
        reserve(size);
        m_size = size;
        for (int i = 0; i < m_size; i++)
        {
            m_data[i] = element;
        }
    }

private:
    int m_size;
    int m_capacity;
    T *m_data;
};

#endif