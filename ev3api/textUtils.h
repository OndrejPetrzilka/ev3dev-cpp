#pragma once
#include <stdint.h>
#include <stdio.h>
#include <math.h>

static void strreverse(char *begin, char *end)
{
    char aux;
    while (end > begin)
        aux = *end, *end-- = *begin, *begin++ = aux;
}

char *modp_itoa10(int32_t value, char *str)
{
    char *wstr = str;
    // Take care of sign
    unsigned int uvalue = (value < 0) ? -value : value;
    // Conversion. Number is reversed.
    do
        *wstr++ = (char)(48 + (uvalue % 10));
    while (uvalue /= 10);
    if (value < 0)
        *wstr++ = '-';
    *wstr = '\0';
    // Reverse string
    strreverse(str, wstr - 1);
    return wstr;
}

// Finds newline or null character, returns number of characters on current line (return bufSize when there's not break)
int findBreak(const char* buf, int bufSize)
{
    for (size_t i = 0; i < bufSize; i++)
    {
        if (buf[i] == '\n' || buf[i] == '\0')
        {
            // Found newline or null char
            return i;
        }
    }
    return bufSize;
}