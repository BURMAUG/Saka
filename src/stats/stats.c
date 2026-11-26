#include "stats.h"

#include <stddef.h>


int
frequency(entry *entry)
{
	if (entry == NULL)
		return -1;
	return (int)(++entry->frequency);
}
