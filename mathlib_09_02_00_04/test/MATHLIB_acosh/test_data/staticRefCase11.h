#ifdef WIN32
float staticRefInCase11[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInCase11[] =
#endif
     {
        -0.04652155687444515, 0.38787960362805907, 0.49270508391740475, -0.4422063376425328,
           -0.3395778881505729, 0.5355996139035262, 0.40335994119730767, 0.7019225111873515,
           0.15027001619822045, 0.09410064777170102,
     };

#ifdef WIN32
float staticRefOutCase11[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutCase11[] =
#endif
     {
        2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0,
           2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0,
     };
