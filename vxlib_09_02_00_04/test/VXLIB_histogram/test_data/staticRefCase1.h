#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        139, 212, 41, 164, 189, 254, 165, 104,
        221, 82, 160, 218, 8, 60, 19, 31,

     };

#ifdef WIN32
uint32_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefOutCase1[] =
#endif
     {
        1, 2, 1, 1, 0, 1, 1, 0, 1, 1, 2, 1, 0, 3, 0, 1,
     };
