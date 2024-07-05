#include "postgres.h"

// Looks stupid, but fast.
extern inline int mn_is_str_marked_null(char* str)
{
    if (str[0] == 'N' || str[0] == 'n')
        if (str[1] == 'U' || str[1] == 'u')
            if (str[2] == 'L' || str[2] == 'l')
                if (str[3] == 'L' || str[3] == 'l')
                    if (str[4] == ':')
                        return 1;
    return 0;
}

// String can have spaces at its head or tail.
extern inline char mn_is_str_number(char* str)
{
    char* c = str;
    if (*str == '-')
        c += 1;
    for ( ; *c != '\0'; c++)
    {
        if (!isdigit(*c))
            return 0;
    }
    if (*str == '\0' || strcmp(str, "-") == 0)
        return 0;
    return 1;
}

// String can have spaces at its head or tail.
extern inline uint32 mn_str_to_null_identifier(char* str)
{
	char* c = str;
    uint32 null_id;
	for ( ; *c != '\0'; c++)
    {
        if (!isdigit(*c))
            return 0;
    }
	if (*str == '\0')
		return 0;
	sscanf(str, "%u", &null_id);
	return null_id;
}