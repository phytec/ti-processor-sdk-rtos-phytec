#ifdef WIN32
uint8_t staticRefInCase26[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase26[] =
#endif
     {
        20, 62, 149, 89, 41, 24,
        96, 29, 160, 33, 226, 134,
        118, 243, 153, 6, 230, 70,
        119, 58, 183, 57, 135, 103,
        3, 150, 77, 210, 112, 95,
        247, 69, 80, 12, 64, 41,

     };

#ifdef WIN32
int16_t staticRefFilterCase26[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase26[] =
#endif
     {
        -1, 0, 0, 1, 0,
        -1, 0, 0, 0, 1,
        1, 2, 1, 1, 0,
        0, 0, 0, -1, 0,
        -1, 0, 1, 2, 1,

     };

#ifdef WIN32
int16_t staticRefOutCase26[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase26[] =
#endif
     {
        55, 55,
        14, 50,

     };
