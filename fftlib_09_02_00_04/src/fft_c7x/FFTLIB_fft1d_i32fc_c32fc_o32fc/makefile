TARGET_PLATFORM ?= C71

OUT= ./out
ASM= ./asm
TEST_CASE=3

INC= -I ./
INC+= -I ./c71/
INC+= -I ../common/
INC+= -I ../common/c71/
INC+= -I $(CGT7X_ROOT)/include

FLAGS= -o3 -mw --keep_asm --abi=eabi --silicon_version=7100 -d __C7100__ -u _DISPLAY_PARAMS -d _USE_GTSC -u _DEBUG -d _EXIT_FAILURE -d TEST_CASE=$(TEST_CASE)

CC= $(CGT7X_ROOT)/bin/cl7x

OBJS= FFTLIB_fft1d_i32fc_c32fc_o32fc_cn.obj FFTLIB_fft1d_i32fc_c32fc_o32fc_d.obj FFTLIB_fft1d_i32fc_c32fc_o32fc_idat.obj FFTLIB_fft1d_i32fc_c32fc_o32fc.obj TI_memory.obj TI_profile.obj TI_test.obj TI_cache.obj
#paramv.obj 

OBJLIST=$(addprefix $(OUT)/, $(OBJS))

all: out_dir FFTLIB_fft1d_i32fc_c32fc_o32fc.out

out_dir:
	mkdir -p $(OUT)
	mkdir -p $(ASM)

move_obj:
	mv *.obj $(OUT)
	mv *.asm $(ASM)

OBJOUT=

$(OUT)/FFTLIB_fft1d_i32fc_c32fc_o32fc_cn.obj: ./FFTLIB_fft1d_i32fc_c32fc_o32fc_cn.c ./FFTLIB_fft1d_i32fc_c32fc_o32fc_cn.h
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/FFTLIB_fft1d_i32fc_c32fc_o32fc.obj: ./c71/FFTLIB_fft1d_i32fc_c32fc_o32fc.c ./c71/FFTLIB_fft1d_i32fc_c32fc_o32fc.h
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/FFTLIB_fft1d_i32fc_c32fc_o32fc_d.obj: ./FFTLIB_fft1d_i32fc_c32fc_o32fc_d.c ./FFTLIB_fft1d_i32fc_c32fc_o32fc_cn.h ./FFTLIB_fft1d_i32fc_c32fc_o32fc_idat.h ./FFTLIB_fft1d_i32fc_c32fc_o32fc.h ../common/TI_test.h ../common/TI_memory.h ../common/TI_profile.h 
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/FFTLIB_fft1d_i32fc_c32fc_o32fc_idat.obj: ./FFTLIB_fft1d_i32fc_c32fc_o32fc_idat.c ./FFTLIB_fft1d_i32fc_c32fc_o32fc_idat.h ../common/TI_memory.h
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

#$(OUT)/paramv.obj: ../common/paramv.c ../common/paramv.h
#	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/TI_memory.obj: ../common/TI_memory.c ../common/TI_memory.h ../common/TI_test.h ../common/TI_profile.h
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/TI_profile.obj: ../common/TI_profile.c ../common/TI_profile.h ../common/TI_memory.h 
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/TI_test.obj: ../common/TI_test.c ../common/TI_test.h ../common/TI_profile.h 
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)

$(OUT)/TI_cache.obj: ../common/c6x/TI_cache.c ../common/TI_memory.h
	$(CC) $(FLAGS) $(INC) $< $(OBJOUT)


FFTLIB_fft1d_i32fc_c32fc_o32fc.out: $(OBJLIST) move_obj
	$(CC) -o3 --abi=eabi --silicon_version=7100 -z --warn_sections --reread_libs --ram_model -l "$(CGT7X_ROOT)/lib/rts7100_le.lib" -l "../lnk.cmd" -o $@ $(OBJLIST) --map_file $(OUT)/$@.map
	mv $@ $(OUT)

clean:
	rm -r ./out ./asm *.out *.asm *.txt *.obj
