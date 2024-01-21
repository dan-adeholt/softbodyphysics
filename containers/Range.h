#ifndef __RANGE_H
#define __RANGE_H

#include "Span.h"

template <typename T>
struct Range
{
    T *data;
    int size;

    T &operator[](int index) { return data[index]; }
    inline const T &operator[](int index) const { return data[index]; }

    T* begin() { return data; }
    T* end() { return data + size; }

    const T* begin() const { return data; }
    const T* end() const { return data + size; }

    Range slice(const Span& span) const
    {
        return Range{&data[span.start], span.end - span.start};
    }

    Range slice(int start, int end) const
    {
        return Range{&data[start], end - start};
    }    
};

#endif