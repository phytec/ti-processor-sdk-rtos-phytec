#ifdef WIN32
int8_t staticRefInCase61[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInCase61[] =
#endif
     {
        -87, -21, -107, -43,
        97, -31, 59, 107,

     };

#ifdef WIN32
int8_t staticRefOutCase61[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase61[] =
#endif
     {
        86, 20, 106, 42,
        -98, 30, -60, -108,

     };
