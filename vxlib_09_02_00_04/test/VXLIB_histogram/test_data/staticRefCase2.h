#ifdef WIN32
uint8_t staticRefInCase2[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase2[] =
#endif
     {
        44, 143, 84, 121, 104, 229, 85, 106, 59, 67, 153, 198, 129, 151, 81, 73,
        7, 221, 160, 203, 191, 122, 58, 39, 183, 41, 193, 96, 223, 133, 112, 202,

     };

#ifdef WIN32
uint32_t staticRefOutCase2[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefOutCase2[] =
#endif
     {
        1, 0, 3, 2, 3, 3, 3, 2, 3, 3, 0, 2, 4, 2, 1, 0,
     };
