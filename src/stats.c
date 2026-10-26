#include "stats.h"

#include <stddef.h>

int
frequency(map *entry)
{
    if (entry == NULL)
        return -1;
    return (int)(++entry->stats.frequency);
}