#ifdef WIN32
uint8_t staticRefInCase42[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase42[] =
#endif
     {
        250, 172, 235, 243, 199, 100, 37, 56, 176,
        134, 185, 173, 183, 111, 153, 38, 105, 204,
        188, 92, 93, 29, 81, 243, 232, 49, 156,
        188, 30, 13, 165, 209, 68, 65, 9, 226,
        192, 112, 217, 0, 110, 89, 7, 12, 109,
        11, 25, 155, 155, 106, 73, 41, 69, 134,
        19, 191, 205, 77, 1, 91, 119, 76, 24,
        199, 68, 171, 131, 237, 196, 145, 21, 77,
        148, 118, 41, 140, 70, 21, 253, 114, 110,

     };

#ifdef WIN32
int16_t staticRefFilterCase42[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase42[] =
#endif
     {
        -1, -1, 0, 0, 0, 0, 0,
        -1, 1, 1, 2, 2, 0, 0,
        1, 1, 0, 2, 2, 1, 0,
        1, -1, 2, -1, 1, 2, 2,
        2, 0, 2, 0, 1, 0, 0,
        -1, 1, -1, 0, 1, 0, 1,
        0, 0, 1, -1, 0, 2, 0,

     };

#ifdef WIN32
int16_t staticRefOutCase42[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase42[] =
#endif
     {
        100, 65, 94,
        93, 75, 75,
        78, 80, 69,

     };
