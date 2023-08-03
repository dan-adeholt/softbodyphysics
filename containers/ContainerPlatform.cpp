#include "./ContainerPlatform.h"

void* operator new(size_t, void* place) noexcept {
    return place;
}