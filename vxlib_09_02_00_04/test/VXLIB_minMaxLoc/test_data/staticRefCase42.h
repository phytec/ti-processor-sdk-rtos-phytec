#ifdef WIN32
uint8_t staticRefInCase42[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase42[] =
#endif
     {
        237,

     };

#ifdef WIN32
uint8_t staticRefMinValCase42[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMinValCase42[] =
#endif
     {
        237
     };

#ifdef WIN32
uint8_t staticRefMaxValCase42[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMaxValCase42[] =
#endif
     {
        237
     };

#ifdef WIN32
uint32_t staticRefMinCountCase42[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinCountCase42[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMaxCountCase42[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxCountCase42[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMinLocCase42[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinLocCase42[] =
#endif
     {
        0, 0,

     };

#ifdef WIN32
uint32_t staticRefMaxLocCase42[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxLocCase42[] =
#endif
     {
        0, 0,

     };
