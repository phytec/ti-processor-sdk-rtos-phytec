
.PHONY : all clean

all:
	$(MKDIR) -p ../../../../docs/apiguide
	$(DOXYGEN) $(SOC)/doxyfile 1>output.txt

clean:
	$(RM) -R $(PDK_INSTALL_PATH)/../docs/apiguide/$(SOC)/html
	$(RM) -R output.txt

# Nothing beyond this point
