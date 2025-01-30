#ifdef WIN32
uint8_t staticRefInpCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInpCase1[] =
#endif
     {
        218, 9, 207, 156,
        46, 168, 175, 148,
        83, 26, 162, 62,
        0, 8, 216, 98,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 1, 1, 1,

     };
