#ifdef WIN32
int8_t staticRefInpCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefInpCase9[] =
#endif
     {
        1, -44, -86, 5,
        -113, 34, -72, -74,
        -96, 101, 78, 30,
        55, 51, -35, 103,

     };

#ifdef WIN32
int8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefOutCase9[] =
#endif
     {
        24, 24, 24, 24,
        24, 24, 24, 24,
        24, 24, 40, 24,
        40, 40, 24, 24,

     };
