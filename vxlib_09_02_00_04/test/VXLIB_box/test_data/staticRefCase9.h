#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        169, 19, 246,
        60, 23, 41,
        231, 95, 243,

     };

#ifdef WIN32
uint8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase9[] =
#endif
     {
        125,

     };
