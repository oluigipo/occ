/*
* OUR IR
*
*  [ How it works - Ir_Function ]
*
*  Every program/objectfile is made by a bunch of functions and a global space.
*  Functions have 4 interesting things:
*      - Parameters;
*      - Return value;
*      - Locals;
*      - Code;
*  The last 2 (locals and code) are only needed if it's a function definition. Otherwise, only the
*  return value and parameters are important because they incluence how a function is called in the IR.
*
*  With the exception of code, all those things are just "variables", mutable pieces of data. Their type
*  is defined with the 'struct Ir_Locals'.
*  
*  [ How it works - Ir_Locals ]
*
*  Just a simple 'count' field specifies the length of the 'values' array. Every value is either an array
*  of a primitive type, or a byte array (piece of memory). Values of primitive types are going to be regalloc'ed
*  and are not expected to be in any particular order (inside the array).
*  
*  For example, the declarations 'int A, B; float C; struct Test B;'
*  We could represent it as (assuming 'sizeof(struct Test) == 16')
*  {
*      .count = 3,
*      .values = {
*          { Ir_LocalKind_Int32, 2, }
*          { Ir_LocalKind_Float32, 1, }
*          { Ir_LocalKind_Bytes, 16, }
*      },
*  }
*  Specifically the order of A and B in the Int32 array doesn't matter, but the order of the 'values' array
*  itself does matter.
*
*  [ How it works - Operations Stack ]
*
*  
*/

#include "ir_definitions.h"
