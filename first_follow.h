/*
 * BATCH 74
 * ========
 * Deven Bansod    2012B3A7316P
 * Nirant Kasliwal 2012C6PS694P
 *
 */

#ifndef FIRST_FOLLOW_INTERFACE
#define FIRST_FOLLOW_INTERFACE

/*
 * Interface to FileBuffer
 *
 */
void createFirstSet(grammarRule* G, grammarRule* firstset, int *fcount, int count);
void createFollowSet(grammarRule* G, grammarRule* followset, int fcount, int count);

#endif