#ifndef __CONTAINER_PLATFORM_H
#define __CONTAINER_PLATFORM_H

#include <stddef.h>

void* operator new(size_t, void* place) noexcept;

#endif