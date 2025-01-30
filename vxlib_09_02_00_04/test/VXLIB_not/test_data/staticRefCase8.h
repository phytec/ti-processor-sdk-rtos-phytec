#ifdef WIN32
uint8_t staticRefInCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase8[] =
#endif
     {
        157, 178, 54,
        34, 119, 207,
        254, 78, 180,

     };

#ifdef WIN32
uint8_t staticRefOutCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase8[] =
#endif
     {
        98, 77, 201,
        221, 136, 48,
        1, 177, 75,

     };
