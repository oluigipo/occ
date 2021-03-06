#define MY_MACRO hello
#define MY_MACROFN(x) MY_MACRO ## x

#define STR(x) #x
#define STR2(x) STR(x)

#ifdef MY_MACRO
#   warning MY_MACRO is defined!
#elif defined(MY_MACROFN)
#   error MY_MACRO isn't defined, but MY_MACROFN is!
#else
#   error nothing is defined :(
#endif

#define CString char*
#define OTHER_STR STR
#define char char

const CString name = OTHER_STR(pepe);
void* hello(void)
{
	return "world";
}

int an_int = 0xffffULL;
float a_float = 35.0f;
float wtf = 0x8080.0p+3f;

const char* a_string = "pepe, " STR(MY_MACRO) " is " STR2(MY_MACRO);

#define HE HI
#define LLO _THERE
#define HELLO "HI THERE"
#define CAT(a,b) a##b
#define XCAT(a,b) CAT(a,b)
#define CALL(fn) fn(HE,LLO)
int a = CAT(HE, LLO);
int b = XCAT(HE, LLO);
int c = CALL(CAT);

#include <stdio.h>

#define LOG(...) printf(__VA_ARGS__)

int main(int argc, char* argv[])
{
	int typedef a;
	a f(void);
	a v(void);
	
	a (*pp[2])(void) = {
		f, v
	};
	
	pp[1] = &argc;
	
	LOG("address of 'f' is %p\n", pp[0]);
	LOG("this is line " STR2(__LINE__) "!\n");
}
