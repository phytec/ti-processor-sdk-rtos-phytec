#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        204, 6, 226, 229, 213, 141,
        30, 86, 109, 249, 147, 246,
        82, 107, 119, 228, 128, 110,
        227, 21, 47, 9, 52, 46,
        238, 211, 3, 195, 132, 215,
        124, 219, 203, 10, 50, 89,

     };

#ifdef WIN32
uint32_t staticRefPixelsProcessedInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefPixelsProcessedInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
uint32_t staticRefCurrentSumInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSumInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
uint32_t staticRefCurrentSqSumInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSqSumInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
float staticRefOut0Case1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOut0Case1[] =
#endif
     {
        131.97222900390625,
     };

#ifdef WIN32
float staticRefOut1Case1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOut1Case1[] =
#endif
     {
        81.22653198242188,
     };

#ifdef WIN32
uint32_t staticRefPixelsProcessedOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefPixelsProcessedOutCase1[] =
#endif
     {
        36,
     };

#ifdef WIN32
uint32_t staticRefCurrentSumOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSumOutCase1[] =
#endif
     {
        4751,
     };

#ifdef WIN32
uint32_t staticRefCurrentSqSumOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSqSumOutCase1[] =
#endif
     {
        864519,
     };
