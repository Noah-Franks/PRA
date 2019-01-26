#ifndef COMPILER_GAURD_TYPES
#define COMPILER_GAURD_TYPES

// Integers
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

typedef signed char  schar;
typedef signed short sshort;
typedef signed int   sint;
typedef signed long  slong;

// Lambda Calculus
typedef void   (* lambda)(void *);
typedef void   (*    eta)(void  );
typedef void * (*     mu)(void *);

#endif
