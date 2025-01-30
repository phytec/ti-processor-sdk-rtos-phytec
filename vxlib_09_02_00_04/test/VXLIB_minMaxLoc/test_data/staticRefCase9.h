#ifdef WIN32
uint8_t staticRefInCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase9[] =
#endif
     {
        253,

     };

#ifdef WIN32
uint8_t staticRefMinValCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMinValCase9[] =
#endif
     {
        253
     };

#ifdef WIN32
uint8_t staticRefMaxValCase9[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefMaxValCase9[] =
#endif
     {
        253
     };

#ifdef WIN32
uint32_t staticRefMinCountCase9[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinCountCase9[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMaxCountCase9[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxCountCase9[] =
#endif
     {
        1
     };

#ifdef WIN32
uint32_t staticRefMinLocCase9[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMinLocCase9[] =
#endif
     {
        0, 0,

     };

#ifdef WIN32
uint32_t staticRefMaxLocCase9[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefMaxLocCase9[] =
#endif
     {
        0, 0,

     };
