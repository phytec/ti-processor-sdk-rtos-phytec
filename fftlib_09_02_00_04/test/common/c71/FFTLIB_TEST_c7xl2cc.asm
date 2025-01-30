

    .global FFTLIB_TEST_c7xSetL2CC
FFTLIB_TEST_c7xSetL2CC:
    MVC .S1 A4,     ECR385      ;; L2 Cache Control Register
    RET .B1

    .global FFTLIB_TEST_c7xGetL2CC
FFTLIB_TEST_c7xGetL2CC:
    MVC .S1 ECR385, A4  ;; L2 Cache Control Register
    RET .B1
