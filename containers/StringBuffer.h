#ifndef __STRINGBUFFER_H
#define __STRINGBUFFER_H

#include <stdarg.h>
#include "../stb_sprintf/stb_sprintf.h"

template <int N>
struct StringBuffer
{
    char data[N] = {};
    static constexpr int capacity = N;

    StringBuffer() = default;

    StringBuffer(const char *format)
    {
        append(format);
    }

    // Track the current number of characters in the buffer (not counting the null terminator)
    int size = 0;

    inline char operator[](int i) const
    {
        return data[i];
    }
    const char *begin() const { return data; }
    const char *end() const { return data + size; } // Buf is zero-terminated, so end() will point on the zero-terminator
    bool empty() const { return size == 0; }
    void clear()
    {
        size = 0;
        data[0] = '\0';
    }

    const char *c_str() const { return data; }

    int appendRange(const char *start, const char *end)
    {
        int count = 0;
        while (size < (N - 1) && start != end)
        {
            data[size++] = *start++;
            count++;
        }

        if (size < N)
        {
            data[size] = '\0';
        }

        return count;
    }

    // Returns the number of characters appended (not counting the null terminator).
    int append(const char *format, ...)
    {
        // If there's no space left, bail immediately
        if (size >= N || format == nullptr)
            return 0;

        // Calculate how many characters remain (including space for a null terminator)
        const int spaceLeft = N - size;

        // Prepare to handle variadic arguments
        va_list args;
        va_start(args, format);

        // stb_sprintf’s "vsnprintf" variant:
        //  int stbsp_vsnprintf(char *buf, int count, const char *fmt, va_list va);
        // It returns the number of characters that would have been written
        // (not counting the terminating null), or a negative value on an error.
        const int written = stbsp_vsnprintf(data + size, spaceLeft, format, args);

        va_end(args);

        // Check for error
        if (written < 0)
        {
            // Error happened, do not advance `size`
            return 0;
        }

        // If `written >= spaceLeft`, it means the text was truncated to fit.
        // stbsp_vsnprintf will still null-terminate the string as long as `spaceLeft > 0`.
        // But we clamp `written` so we can correctly update `size`.
        const int actualWritten = (written >= spaceLeft) ? (spaceLeft - 1) : written;

        // Advance the used size
        size += actualWritten;

        // Ensure null termination just to be safe (stbsp_vsnprintf should handle it, but this is extra safety).
        data[size] = '\0';

        return actualWritten;
    }
};

#endif // __STRINGBUFFER_H