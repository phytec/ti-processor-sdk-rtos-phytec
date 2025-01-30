#ifdef WIN32
uint8_t staticRefInCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase22[] =
#endif
     {
        29, 235, 22, 235, 129,
        83, 19, 27, 18, 145,
        65, 24, 65, 148, 180,
        2, 123, 36, 60, 55,
        75, 108, 168, 71, 137,

     };

#ifdef WIN32
uint8_t staticRefOutCase22[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase22[] =
#endif
     {
        71,

     };
