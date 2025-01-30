#ifdef WIN32
uint16_t staticRefInCase21[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefInCase21[] =
#endif
     {
        59737, 61358, 36743, 8822, 58384,
        64229, 54817, 16222, 43955, 50593,
        24787, 29335, 41872, 51641, 43214,
        35807, 63061, 9645, 9753, 12471,
        43410, 36310, 60566, 37790, 40862,

     };

#ifdef WIN32
uint16_t staticRefOutCase21[] =
#else
__attribute__((section (".staticData")))
     static uint16_t staticRefOutCase21[] =
#endif
     {
        64229, 61358, 58384,
        64229, 63061, 51641,
        63061, 63061, 60566,

     };
