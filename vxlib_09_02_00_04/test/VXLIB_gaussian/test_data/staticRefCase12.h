#ifdef WIN32
uint8_t staticRefInCase12[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase12[] =
#endif
     {
        174, 81, 31,
        58, 150, 183,
        239, 0, 12,
        149, 24, 6,
        177, 201, 139,
        162, 68, 109,
        239, 1, 186,
        153, 168, 94,
        112, 79, 79,
        159, 110, 78,
        174, 182, 115,
        53, 247, 45,
        146, 66, 66,
        226, 210, 172,
        75, 124, 154,
        200, 73, 137,

     };

#ifdef WIN32
uint8_t staticRefOutCase12[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase12[] =
#endif
     {
        106,
        77,
        85,
        127,
        122,
        115,
        121,
        108,
        119,
        147,
        136,
        131,
        153,
        140,

     };
