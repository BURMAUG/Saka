#include "stats.h"
#include <dirent.h>

int frequency(map *map)
{
	return ++map->stats.frequency;
}
