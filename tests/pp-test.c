#define MY_MACRO hello
#define MY_MACROFN(x) MY_MACRO ## x

#ifdef MY_MACRO
#   error MY_MACRO is defined!
#elif defined(MY_MACROFN)
#   error MY_MACRO isn't defined, but MY_MACROFN is!
#else
#   error nothing is defined :(
#endif

#define CString char*
const CString name = "pepe";

#define STR(x) #x
#define STR2(x) STR(x)

const char* a_string = "pepe, " STR(MY_MACRO) " is " STR2(MY_MACRO);

#include <stdio.h>

int main(int argc, char* argv[])
{
	int typedef a;
	a f(void);
	a v(void);
	
	a (*pp[2])(void) = {
		f, v
	};
}
