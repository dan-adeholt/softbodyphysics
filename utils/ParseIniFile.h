#ifndef __PARSE_INI_FILE_H
#define __PARSE_INI_FILE_H

template <typename T>
void parseIniFile(FILE *file, T *instance, void (T::*handleVarFunc)(const char *, const char *, const char *))
{
    const int bufferSize = 255;
    char sectionBuf[bufferSize];
    char nameBuf[bufferSize];
    char valueBuf[bufferSize];
    int nameBufPos = 0;
    int valueBufPos = 0;
    int sectionBufPos = 0;
    bool nameBufActive = true;
    bool sectionBufActive = false;
    sectionBuf[0] = '\0';
    char buffer[2048];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        for (size_t i = 0; i < bytesRead; i++)
        {
            char c = buffer[i];
            // Process each character as in the existing loop
            if (c == '[')
            {
                sectionBufActive = true;
            }
            else if (c == ']')
            {
                sectionBufActive = false;
                sectionBuf[sectionBufPos] = '\0';
                sectionBufPos = 0;
            }
            else if (c == '=')
            {
                nameBufActive = false;
            }
            else if (c == '\n')
            {
                nameBuf[nameBufPos] = '\0';
                valueBuf[valueBufPos] = '\0';
                if (nameBufPos > 0)
                {
                    (instance->*handleVarFunc)(sectionBuf, nameBuf, valueBuf);
                }
                nameBufPos = 0;
                valueBufPos = 0;
                nameBufActive = true;
            }
            else if (sectionBufActive)
            {
                sectionBuf[sectionBufPos++] = c;
            }
            else if (nameBufActive)
            {
                nameBuf[nameBufPos++] = c;
            }
            else
            {
                valueBuf[valueBufPos++] = c;
            }
        }
    }
}

void writeIniSection(FILE *file, const char *section)
{
    fprintf(file, "[%s]\n", section);
}

void writeIniProperty(FILE *file, const char *name, const char *value)
{
    fprintf(file, "%s=%s\n", name, value);
}

#endif
