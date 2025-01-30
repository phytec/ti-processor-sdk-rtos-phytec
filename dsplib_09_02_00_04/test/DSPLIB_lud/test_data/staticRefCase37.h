// A Matrix
#ifdef WIN32
double staticRefInACase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefInACase37[] =
#endif
     {
        1.8170, 7.3224, -5.0900, -9.3480,
        -7.2052, 5.3398, -8.7459, 7.2452,
        -9.1788, 8.3307, 5.5300, -6.3861,
        4.7920, -4.6748, 2.1439, -4.8352,

     };

// B vector
#ifdef WIN32
double staticRefInBCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefInBCase37[] =
#endif
     {
        -6.2954, 7.4953, -6.6576, 1.4078,
     };

// P Matrix
#ifdef WIN32
unsigned short staticRefOutPCase37[] =
#else
__attribute__((section (".staticData")))
     static unsigned short staticRefOutPCase37[] =
#endif
     {
        0, 0, 1, 0,
        0, 1, 0, 0,
        1, 0, 0, 0,
        0, 0, 0, 1,

     };

// L Matrix
#ifdef WIN32
double staticRefOutLCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutLCase37[] =
#endif
     {
        1.0000, 0.0000, 0.0000, 0.0000,
        0.7850, 1.0000, 0.0000, 0.0000,
        -0.1980, -7.4779, 1.0000, 0.0000,
        -0.5221, 0.2714, -0.0843, 1.0000,

     };

// U Matrix
#ifdef WIN32
double staticRefOutUCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutUCase37[] =
#endif
     {
        -9.1788, 8.3307, 5.5300, -6.3861,
        0.0000, -1.1997, -13.0869, 12.2582,
        0.0000, 0.0000, -101.8575, 81.0538,
        0.0000, 0.0000, 0.0000, -4.6663,

     };

// X vector
#ifdef WIN32
double staticRefOutXCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutXCase37[] =
#endif
     {
        -1.3782, -1.8419, -1.1753, -0.3973,
     };

// invA Matrix
#ifdef WIN32
double staticRefOutInvACase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutInvACase37[] =
#endif
     {
        0.0700, -0.1645, -0.0978, -0.2526,
        0.0793, -0.1509, -0.0379, -0.3294,
        -0.0242, -0.1346, 0.0118, -0.1705,
        -0.0181, -0.0769, -0.0551, -0.2143,

     };
