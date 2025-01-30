#ifdef WIN32
uint8_t staticRefInCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase10[] =
#endif
     {
        193, 78, 46,
        116, 95, 189,
        45, 254, 9,

     };

#ifdef WIN32
uint8_t staticRefOutCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase10[] =
#endif
     {
        113,

     };
