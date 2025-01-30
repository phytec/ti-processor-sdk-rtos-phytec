#ifdef WIN32
uint16_t staticRefInCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefInCase31[] =
#endif
     {
        9111, 53195, 7264, 40865, 13402,
        32961, 63615, 5918, 33802, 15454,
        25819, 39043, 41175, 16264, 60991,
        1814, 13031, 7422, 57444, 11127,
        40140, 22005, 41767, 27469, 51015,

     };

#ifdef WIN32
uint16_t staticRefOutCase31[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefOutCase31[] =
#endif
     {
        63615, 63615, 40865,
        63615, 63615, 60991,
        41175, 57444, 60991,
        41767, 57444, 57444,

     };
