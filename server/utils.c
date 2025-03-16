#include <stdio.h>
#include <string.h>

/**
 * @brief Removes a trailing newline from a string, if present.
 *
 * This function checks if the last character before the null terminator
 * is a newline (`\n`). If so, it replaces it with a null terminator (`\0`),
 * effectively removing the newline from the string.
 *
 * @param str A null-terminated string that may contain a trailing newline.
 */
void remove_trailing_newline(char *str)
{
    if (str == NULL)
        return;

    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == '\n')
    {
        str[len - 1] = '\0';
    }
}
