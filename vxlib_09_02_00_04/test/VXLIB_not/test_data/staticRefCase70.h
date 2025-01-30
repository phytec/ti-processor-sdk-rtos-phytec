#ifdef WIN32
int16_t staticRefInCase70[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefInCase70[] =
#endif
     {
        5112, -5866, -5381,
        5514, 31349, -2267,
        30088, 3915, 23983,

     };

#ifdef WIN32
int16_t staticRefOutCase70[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefOutCase70[] =
#endif
     {
        -5113, 5865, 5380,
        -5515, -31350, 2266,
        -30089, -3916, -23984,

     };
