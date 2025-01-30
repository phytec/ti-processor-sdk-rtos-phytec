#ifdef WIN32
uint8_t staticRefInCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase23[] =
#endif
     {
        205, 145, 222, 97, 132,
        211, 81, 2, 128, 195,
        162, 19, 24, 203, 233,
        228, 95, 158, 241, 20,
        197, 197, 102, 138, 1,

     };

#ifdef WIN32
uint8_t staticRefOutCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase23[] =
#endif
     {
        111,

     };
