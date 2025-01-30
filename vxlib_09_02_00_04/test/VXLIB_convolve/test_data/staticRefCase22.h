#ifdef WIN32
uint8_t staticRefInCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase22[] =
#endif
     {
        32, 93, 117, 139, 4,
        180, 29, 180, 234, 101,
        67, 167, 91, 107, 52,
        223, 252, 109, 199, 204,
        36, 251, 45, 41, 173,
        103, 185, 242, 102, 154,
        63, 175, 192, 195, 73,
        85, 73, 106, 82, 28,
        164, 149, 200, 129, 249,

     };

#ifdef WIN32
int16_t staticRefFilterCase22[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase22[] =
#endif
     {
        -1, 1, 0, 2, 2,
        0, 2, 2, 2, 1,
        0, 0, 0, 0, 0,
        1, 1, 2, 0, 2,
        0, 0, 0, -1, 0,

     };

#ifdef WIN32
uint8_t staticRefOutCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase22[] =
#endif
     {
        171,
        105,
        166,
        105,
        113,

     };
