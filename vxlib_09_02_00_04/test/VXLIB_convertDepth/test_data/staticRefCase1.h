#ifdef WIN32
uint8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn0Case1[] =
#endif
     {
        37, 241, 125,
        154, 84, 158,
        251, 231, 124,

     };

#ifdef WIN32
int16_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase1[] =
#endif
     {
        148, 964, 500,
        616, 336, 632,
        1004, 924, 496,

     };
