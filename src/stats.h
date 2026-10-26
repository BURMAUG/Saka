#ifndef __TINGONE_STATS_H__
#define __TINGONE_STATS_H__

#include <dirent.h>
#include <sys/dir.h>
#include <dirent.h> // I want to use this for I directory walks or something. Come back to this thought


/* 
 * I want my data to be store in a fast cache
 *  L1 - L3  if need be
 */


// a single tracked directory entry.
// this will be like a look up table
typedef struct map {
	const char *path;

	union stats {
		unsigned int is_active;
		unsigned long duration; // this should be in time unit how much time I dwell 
		unsigned long frequency; // count of how many times I go to work in a given dir
					 //
       	} stats;
} map; 

// increment the visit count for dir and return the new value.
int frequency(map *dir);



// calculate cache miss  as well here
#endif
