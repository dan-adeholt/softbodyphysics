#ifndef __RANGE_H
#define __RANGE_H

template <typename T>
struct Range
{
    T *data;
    int size;

    T &operator[](int index) { return data[index]; }
    inline const T &operator[](int index) const { return data[index]; }
};

#endif