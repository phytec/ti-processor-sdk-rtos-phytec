#ifdef WIN32
uint8_t staticRefInCase15[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase15[] =
#endif
     {
        123, 22, 69, 184,
        119, 66, 24, 50,

     };

#ifdef WIN32
uint8_t staticRefOutCase15[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase15[] =
#endif
     {
        132, 233, 186, 71,
        136, 189, 231, 205,

     };
