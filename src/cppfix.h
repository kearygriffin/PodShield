/*
 * cppfix.h
 *
 *  Created on: Aug 22, 2009
 *      Author: keary
 */

#ifndef CPPFIX_H_
#define CPPFIX_H_

__extension__ typedef int __guard __attribute__((mode (__DI__)));


inline int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
inline void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
inline void __cxa_guard_abort (__guard *) {};

/*
void * operator new(size_t size);
void operator delete(void * ptr);
void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}
*/

#endif /* CPPFIX_H_ */
