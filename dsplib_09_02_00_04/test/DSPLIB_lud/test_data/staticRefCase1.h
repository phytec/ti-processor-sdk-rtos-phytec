// A Matrix
#ifdef WIN32
float staticRefInACase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInACase1[] =
#endif
     {
        -4.4200, 9.3900, 7.3500, 6.6000,
        -3.4800, 7.7800, -1.8300, 5.8000,
        2.1300, 0.2200, 0.0000, 2.2900,
        7.0900, -6.4600, -9.6600, 3.3500,

     };

// B vector
#ifdef WIN32
float staticRefInBCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInBCase1[] =
#endif
     {
        4.2200, 0.1000, -4.3200, -1.7900,
     };

// P Matrix
#ifdef WIN32
unsigned short staticRefOutPCase1[] =
#else
__attribute__((section (".staticData")))
     static unsigned short staticRefOutPCase1[] =
#endif
     {
        1, 0, 0, 0,
        0, 0, 0, 1,
        0, 1, 0, 0,
        0, 0, 1, 0,

     };

// L Matrix
#ifdef WIN32
float staticRefOutLCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutLCase1[] =
#endif
     {
        1.0000, 0.0000, 0.0000, 0.0000,
        -1.6041, 1.0000, 0.0000, 0.0000,
        0.7873, 0.0450, 1.0000, 0.0000,
        -0.4819, 0.5516, -0.3069, 1.0000,

     };

// U Matrix
#ifdef WIN32
float staticRefOutUCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutUCase1[] =
#endif
     {
        -4.4200, 9.3900, 7.3500, 6.6000,
        0.0000, 8.6022, 2.1299, 13.9369,
        0.0000, 0.0000, -7.7127, -0.0233,
        0.0000, 0.0000, 0.0000, -2.2243,

     };

// X vector
#ifdef WIN32
float staticRefOutXCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutXCase1[] =
#endif
     {
        -4.5625, -3.9662, 0.4386, 2.7383,
     };

// invA Matrix
#ifdef WIN32
float staticRefOutInvACase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutInvACase1[] =
#endif
     {
        -0.2885, 0.1220, 0.8776, -0.2426,
        -0.3265, 0.2555, 0.7281, -0.2968,
        0.1105, -0.1292, 0.0014, 0.0051,
        0.2997, -0.1380, -0.4496, 0.2542,

     };
