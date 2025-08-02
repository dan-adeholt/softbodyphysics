#ifndef __RANGE_H
#define __RANGE_H

#include "Span.h"
#include <stddef.h>

template <typename T>
struct Range
{
    Range() : data(nullptr), size(0) {}
    Range(T *data, int size) : data(data), size(size) {}
    template <size_t N>
    Range(T (&array)[N]) : data(array), size(static_cast<int>(N)) {}

    T *data;
    int size;

    T &operator[](int index) { return data[index]; }
    inline const T &operator[](int index) const { return data[index]; }

    T *begin() { return data; }
    T *end() { return data + size; }

    const T *begin() const { return data; }
    const T *end() const { return data + size; }

    bool isValid() const
    {
        return data != nullptr && size > 0;
    }

    Range slice(const Span &span) const
    {
        return Range{&data[span.start], span.end - span.start};
    }

    Range slice(int start, int end) const
    {
        return Range{&data[start], end - start};
    }

    bool contains(const T &element) const
    {
        for (int i = 0; i < size; i++)
        {
            if (data[i] == element)
            {
                return true;
            }
        }
        return false;
    }
};

#endif