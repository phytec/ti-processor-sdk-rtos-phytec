#ifdef WIN32
uint8_t staticRefInCase30[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase30[] =
#endif
     {
        163, 88, 76, 59, 107,
        239, 222, 218, 200, 134,
        143, 247, 207, 40, 113,
        97, 74, 187, 90, 149,
        102, 94, 122, 198, 182,
        15, 177, 233, 20, 58,
        205, 229, 4, 36, 33,
        88, 41, 214, 45, 70,
        203, 109, 115, 0, 198,

     };

#ifdef WIN32
int16_t staticRefFilterCase30[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase30[] =
#endif
     {
        1, 0, 0, -1, 2,
        1, 0, 0, -1, 2,
        0, 0, 2, -1, -1,
        0, 0, 2, 2, -1,
        0, 0, 0, 2, 2,

     };

#ifdef WIN32
int16_t staticRefOutCase30[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase30[] =
#endif
     {
        108,
        130,
        86,
        85,
        91,

     };
