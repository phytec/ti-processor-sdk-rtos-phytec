#ifdef WIN32
int8_t staticRefInpCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInpCase9[] =
#endif
     {
        -110, -66, -48, -17,
        30, -104, 25, -3,
        125, 109, -119, -39,
        -26, 57, -36, 107,

     };

#ifdef WIN32
int8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase9[] =
#endif
     {
        0, 0, 0, 0,
        1, 0, 1, 0,
        1, 1, 0, 0,
        0, 1, 0, 1,

     };
