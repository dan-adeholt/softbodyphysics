#ifndef __SPAN_H
#define __SPAN_H

struct Span
{
    Span(int start, int end) : start(start), end(end) {}
    Span() : start(0), end(0) {}

    bool isValid() const
    {
        return start < end;
    }

    int start;
    int end;
};

#endif