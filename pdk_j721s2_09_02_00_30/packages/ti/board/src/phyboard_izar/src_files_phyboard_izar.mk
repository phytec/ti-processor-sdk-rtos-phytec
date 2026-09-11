
SRCDIR += src/phyboard_izar
INCDIR += src/phyboard_izar src/phyboard_izar/include

# Common source files across all platforms and cores
SRCS_COMMON += board_init.c board_lld_init.c board_clock.c board_mmr.c board_pll.c
SRCS_COMMON += board_ddr.c board_ethernet_config.c board_utils.c board_power.c
SRCS_COMMON += board_pinmux.c J721S2_pinmux_data.c
PACKAGE_SRCS_COMMON = src/phyboard_izar/src_files_phyboard_izar.mk
