#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        144, 213, 241, 65, 194, 133,
        137, 231, 56, 175, 32, 116,
        106, 226, 90, 68, 223, 8,
        241, 18, 21, 49, 141, 193,
        216, 91, 184, 239, 221, 239,
        89, 167, 226, 145, 176, 196,

     };

#ifdef WIN32
int16_t staticRefFilterCase9[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase9[] =
#endif
     {
        0, 1, 1,
        0, -1, 0,
        -1, 1, 0,

     };

#ifdef WIN32
int16_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase9[] =
#endif
     {
        4, 27, -10, 20,
        13, -11, 9, -8,
        26, 17, 13, 33,
        10, 11, 2, 3,

     };
