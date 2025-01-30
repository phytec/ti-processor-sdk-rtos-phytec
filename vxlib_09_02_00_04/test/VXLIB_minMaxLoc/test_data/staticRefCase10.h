#ifdef WIN32
uint8_t staticRefInCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase10[] =
#endif
     {
        153,

     };

#ifdef WIN32
uint8_t staticRefMinValCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMinValCase10[] =
#endif
     {
        153
     };

#ifdef WIN32
uint8_t staticRefMaxValCase10[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMaxValCase10[] =
#endif
     {
        153
     };

#ifdef WIN32
uint32_t staticRefMinCountCase10[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinCountCase10[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMaxCountCase10[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxCountCase10[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMinLocCase10[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinLocCase10[] =
#endif
     {
        0, 0,

     };

#ifdef WIN32
uint32_t staticRefMaxLocCase10[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxLocCase10[] =
#endif
     {
        0, 0,

     };
