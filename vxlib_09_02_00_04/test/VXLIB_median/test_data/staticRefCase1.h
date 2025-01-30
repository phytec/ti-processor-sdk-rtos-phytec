#ifdef WIN32
uint8_t staticRefIn0Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn0Case1[] =
#endif
     {
        72, 218, 1,
        104, 237, 50,
        9, 115, 101,

     };

#ifdef WIN32
uint8_t staticRefIn1Case1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefIn1Case1[] =
#endif
     {
        255, 0, 255,
        255, 0, 255,
        0, 0, 255,

     };

#ifdef WIN32
uint8_t staticRefOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase1[] =
#endif
     {
        101,

     };
