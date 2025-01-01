#include <dlfcn.h> // For dladdr, Dl_info
#include <string.h>
#include "DynamicLibrary.h"
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

// A simple function in this library — we use its address to find the library path.
static void myFunctionInThisLibrary() {}

namespace DynamicLibrary
{

    // Returns true on success, false on failure.
    int getLibraryPath(char *buffer, unsigned int bufferSize)
    {
        Dl_info info;
        if (dladdr((void *)&myFunctionInThisLibrary, &info) != 0 && info.dli_fname != NULL)
        {
            // Safely copy the path into the user buffer
            strncpy(buffer, info.dli_fname, bufferSize);
            // Ensure null-termination in case the path is longer than bufferSize
            buffer[bufferSize - 1] = '\0';
            return 1; // success
        }
        return 0; // failure
    }
}

time_t lastModTime = 0;

bool DynamicLibrary::hasLibraryChanged()
{

    char buffer[1024];
    if (getLibraryPath(buffer, sizeof(buffer)))
    {
        struct stat fileInfo;
        if (stat(buffer, &fileInfo) == 0)
        {
            if (lastModTime == 0)
            {
                lastModTime = fileInfo.st_mtime;
                return false;
            }

            bool hasChanged = lastModTime != fileInfo.st_mtime;
            lastModTime = fileInfo.st_mtime;
            return hasChanged;
        }
    }

    return false;
}
