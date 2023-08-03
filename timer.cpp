#include <assert.h>

#ifdef __APPLE__
#define HAVE_MACH_TIMER
#include <mach/mach_time.h>
#endif

// Returns monotonic time in nanos, measured from the first time the function
// is called in the process.
uint64_t monotonicTimeNanos()
{
    uint64_t now = mach_absolute_time();
    static struct Data
    {
        Data(uint64_t bias_) : bias(bias_)
        {
            kern_return_t mtiStatus = mach_timebase_info(&tb);
            assert(mtiStatus == KERN_SUCCESS);
        }
        uint64_t scale(uint64_t i)
        {
            return scaleHighPrecision(i - bias, tb.numer, tb.denom);
        }
        static uint64_t scaleHighPrecision(uint64_t i, uint32_t numer,
                                           uint32_t denom)
        {
            uint64_t high = (i >> 32) * numer;
            uint64_t low = (i & 0xffffffffull) * numer / denom;
            uint64_t highRem = ((high % denom) << 32) / denom;
            high /= denom;
            return (high << 32) + highRem + low;
        }
        mach_timebase_info_data_t tb;
        uint64_t bias;
    } data(now);

    return data.scale(now);
}