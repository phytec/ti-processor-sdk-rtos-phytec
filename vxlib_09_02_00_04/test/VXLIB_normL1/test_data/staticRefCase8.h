#ifdef WIN32
int8_t staticRefIn0Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case8[] =
#endif
     {
        8, 108, 89,
        -125, -120, -18,
        -63, 18, 14,

     };

#ifdef WIN32
int8_t staticRefIn1Case8[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case8[] =
#endif
     {
        -111, 101, -35,
        -5, -61, -5,
        -41, 17, 65,

     };

#ifdef WIN32
uint8_t staticRefOutCase8[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase8[] =
#endif
     {
        119, 209, 124,
        130, 181, 23,
        104, 35, 79,

     };
