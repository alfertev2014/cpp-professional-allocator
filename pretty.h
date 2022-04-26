
#ifndef __PRETTY_FUNCTION__
#ifdef _MSC_VER 
    #define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
#endif

#ifdef USE_PRETTY
#define pretty_function (__PRETTY_FUNCTION__)
#else
#define pretty_function (__FUNCTION__)
#endif
