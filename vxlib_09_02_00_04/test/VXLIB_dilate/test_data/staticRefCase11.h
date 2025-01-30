#ifdef WIN32
uint8_t staticRefInCase11[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase11[] =
#endif
     {
        90, 171, 46, 167, 65,
        190, 73, 15, 164, 17,
        220, 142, 110, 120, 91,
        223, 163, 94, 218, 102,
        23, 60, 188, 237, 77,

     };

#ifdef WIN32
uint8_t staticRefOutCase11[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase11[] =
#endif
     {
        190, 171, 167,
        220, 164, 164,
        223, 218, 218,
        223, 237, 237,

     };
