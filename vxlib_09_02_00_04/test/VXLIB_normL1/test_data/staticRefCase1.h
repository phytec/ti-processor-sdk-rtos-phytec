#ifdef WIN32
int8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn0Case1[] =
#endif
     {
        16, -38, -97,
        28, 91, 72,
        -101, 57, 2,

     };

#ifdef WIN32
int8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static int8_t staticRefIn1Case1[] =
#endif
     {
        -12, 7, 32,
        -126, -41, 62,
        24, 57, 87,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        28, 45, 129,
        154, 132, 134,
        125, 114, 89,

     };
