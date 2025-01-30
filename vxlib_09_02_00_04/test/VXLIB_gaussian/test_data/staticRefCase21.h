#ifdef WIN32
uint8_t staticRefInCase21[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase21[] =
#endif
     {
        204, 239, 183, 237, 82,
        105, 235, 150, 202, 195,
        217, 168, 71, 68, 69,
        86, 168, 142, 224, 51,
        235, 15, 153, 178, 189,

     };

#ifdef WIN32
uint8_t staticRefOutCase21[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase21[] =
#endif
     {
        71,

     };
