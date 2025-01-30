#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        61, 137, 84, 16, 32,
        0, 170, 10, 58, 132,
        242, 247, 226, 193, 181,
        146, 36, 185, 162, 194,
        162, 227, 248, 103, 33,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        247, 247, 226,
        247, 247, 226,
        248, 248, 248,

     };
