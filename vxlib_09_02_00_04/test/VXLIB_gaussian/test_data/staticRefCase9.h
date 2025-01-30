#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        26, 127, 201,
        105, 222, 127,
        102, 209, 51,

     };

#ifdef WIN32
uint8_t staticRefOutCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase9[] =
#endif
     {
        150,

     };
