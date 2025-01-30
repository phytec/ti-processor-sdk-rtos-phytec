
	

    .global CSL_c7xSetESTP_SS
CSL_c7xSetESTP_SS:
     MVC .S1 A4, ESTP_SS
     MVK64   .L1 ss_vector_table,A0
     RET .B1



    .global CSL_c7xSetESTP_S
CSL_c7xSetESTP_S:
     MVC .S1 A4, ESTP_S
     MVK64   .L1 ss_vector_table,A0
     RET .B1


	.sect ".ss_vectors"
	.global ss_vector_table
	.global soft_reset
	.global int_exp
	.global loop_forever
	
ss_vector_table:

soft_reset:
	MVK64   .L1 0x80000000,A0
	B .B1 soft_reset
	.align 0x0200

int_exp:

	MVC .S1   IERR, A2
	MVC .S1   IESR, A3
	MVC .S1 IEAR, A4
	MVC .S1 IEDR, A5

	MVC .S1   IERR, A2
	MVC .S1   IESR, A3
	MVC .S1 IEAR, A4
	MVC .S1 IEDR, A5

loop_forever1:
	B .B1 loop_forever1
	.align 0x0200

page_fault_exp:

	MVC .S1   IERR, A2
	MVC .S1   IESR, A3
	MVC .S1 IEAR, A4
	MVC .S1 IEDR, A5

	MVC .S1   IERR, A2
	MVC .S1   IESR, A3
	MVC .S1 IEAR, A4
	MVC .S1 IEDR, A5

loop_forever:
	B .B1  loop_forever

dummy_func:
	RET .B1

