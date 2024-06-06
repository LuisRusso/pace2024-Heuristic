#ifndef LRUHASH_H
#define LRUHASH_H 
void
hashInit(uint n);
void
hashFree(void);
uint
search(uint l, uint r);
void
insert(uint l, uint r, uint c);
#endif
