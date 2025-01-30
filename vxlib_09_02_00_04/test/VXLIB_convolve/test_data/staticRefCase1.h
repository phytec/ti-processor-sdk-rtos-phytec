#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        87, 49, 98, 59, 29, 226,
        66, 77, 214, 9, 206, 24,
        103, 12, 83, 185, 250, 70,
        205, 30, 132, 114, 132, 218,
        32, 41, 234, 166, 88, 15,
        14, 249, 74, 27, 174, 197,

     };

#ifdef WIN32
int16_t staticRefFilterCase1[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase1[] =
#endif
     {
        1, 2, 0,
        1, -1, 1,
        0, -1, 2,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        27, 13, 73, 30,
        26, 27, 57, 18,
        51, 36, 34, 31,
        73, 4, 33, 45,

     };
