#ifdef WIN32
uint8_t staticRefInCase5[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase5[] =
#endif
     {
        217, 167, 143, 204, 229,
        250, 188, 217, 19, 232,
        202, 187, 65, 241, 129,
        199, 95, 76, 205, 96,
        88, 121, 7, 246, 173,
        66, 99, 63, 108, 232,
        131, 200, 4, 190, 94,
        231, 227, 163, 136, 86,
        219, 122, 173, 1, 135,

     };

#ifdef WIN32
int16_t staticRefFilterCase5[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase5[] =
#endif
     {
        -1, 0, -1,
        0, -1, 2,
        2, -1, 0,

     };

#ifdef WIN32
uint8_t staticRefOutCase5[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase5[] =
#endif
     {
        10, 0, 29,
        11, 0, 10,
        9, 10, 0,
        0, 22, 0,
        0, 14, 1,
        0, 11, 0,
        0, 34, 0,

     };
