#ifdef WIN32
uint8_t staticRefInCase17[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase17[] =
#endif
     {
        115, 206, 84, 175, 208, 186, 95,
        154, 129, 215, 209, 90, 140, 49,
        18, 126, 87, 238, 40, 163, 180,
        151, 17, 38, 87, 50, 119, 21,
        233, 118, 153, 210, 252, 138, 124,
        92, 195, 182, 70, 179, 97, 236,
        38, 26, 217, 27, 220, 31, 251,

     };

#ifdef WIN32
int16_t staticRefFilterCase17[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase17[] =
#endif
     {
        0, -1, 0, 2, 0,
        0, 2, 1, 1, 0,
        0, 1, 0, 0, 0,
        2, 0, -1, 1, 1,
        0, 0, 0, -1, 1,

     };

#ifdef WIN32
uint8_t staticRefOutCase17[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase17[] =
#endif
     {
        40, 53, 72,
        78, 82, 96,
        54, 82, 35,

     };
