#ifdef WIN32
uint8_t staticRefInCase13[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase13[] =
#endif
     {
        93, 170, 252, 29, 183,
        239, 120, 243, 220, 232,
        140, 104, 30, 181, 222,
        64, 213, 200, 198, 118,
        7, 216, 251, 176, 230,
        120, 136, 98, 2, 227,
        213, 71, 229, 240, 243,
        92, 174, 84, 54, 123,
        160, 174, 84, 213, 231,

     };

#ifdef WIN32
int16_t staticRefFilterCase13[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase13[] =
#endif
     {
        0, 1, 1,
        2, 1, -1,
        2, 1, 0,

     };

#ifdef WIN32
int16_t staticRefOutCase13[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase13[] =
#endif
     {
        80, 62, 65,
        56, 86, 104,
        58, 77, 80,
        98, 76, 57,
        75, 54, 91,
        57, 62, 68,
        69, 61, 77,

     };
