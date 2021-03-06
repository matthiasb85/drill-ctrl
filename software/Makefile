##############################################################################
# Build global options
# NOTE: Can be overridden externally.
#

include env_vars
export $(shell sed 's/=.*//' env_vars)

# Compiler options here.
ifeq ($(USE_OPT),)
  USE_OPT = -O0 -ggdb -fomit-frame-pointer -falign-functions=16 
endif

# C specific options here (added to USE_OPT).
ifeq ($(USE_COPT),)
  USE_COPT = 
endif

# C++ specific options here (added to USE_OPT).
ifeq ($(USE_CPPOPT),)
  USE_CPPOPT = -fno-rtti
endif

# Enable this if you want the linker to remove unused code and data.
ifeq ($(USE_LINK_GC),)
  USE_LINK_GC = yes
endif

# Linker extra options here.
ifeq ($(USE_LDOPT),)
  USE_LDOPT = 
endif

# Enable this if you want link time optimizations (LTO).
ifeq ($(USE_LTO),)
  USE_LTO = yes
endif

# Enable this if you want to see the full log while compiling.
ifeq ($(USE_VERBOSE_COMPILE),)
  USE_VERBOSE_COMPILE = no
endif

# If enabled, this option makes the build process faster by not compiling
# modules not used in the current configuration.
ifeq ($(USE_SMART_BUILD),)
  USE_SMART_BUILD = yes
endif

#
# Build global options
##############################################################################

##############################################################################
# Architecture or project specific options
#

# Stack size to be allocated to the Cortex-M process stack. This stack is
# the stack used by the main() thread.
ifeq ($(USE_PROCESS_STACKSIZE),)
  USE_PROCESS_STACKSIZE = 0x400
endif

# Stack size to the allocated to the Cortex-M main/exceptions stack. This
# stack is used for processing interrupts and exceptions.
ifeq ($(USE_EXCEPTIONS_STACKSIZE),)
  USE_EXCEPTIONS_STACKSIZE = 0x400
endif

# Enables the use of FPU (no, softfp, hard).
ifeq ($(USE_FPU),)
  USE_FPU = no
endif

# FPU-related options.
ifeq ($(USE_FPU_OPT),)
  USE_FPU_OPT = -mfloat-abi=$(USE_FPU) -mfpu=fpv4-sp-d16
endif

#
# Architecture or project specific options
##############################################################################

##############################################################################
# Project, target, sources and paths
#

# Define project name here
PROJECT = dctrl

# Target settings.
MCU  = cortex-m3

# Imported source files and paths.
CHIBIOS  := ./.3rdparty/ChibiOS_$(OS_VERSION)
U8G2     := ./.3rdparty/u8g2/csrc
CONFDIR  := ./inc/ch
BUILDDIR := ./build
DEPDIR   := ./.dep

# Licensing files.
include $(CHIBIOS)/os/license/license.mk
# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f1xx.mk
# HAL-OSAL files (optional).
include $(CHIBIOS)/os/hal/hal.mk
include $(CHIBIOS)/os/hal/ports/STM32/STM32F1xx/platform.mk
include $(CHIBIOS)/os/hal/boards/MAPLEMINI_STM32_F103/board.mk
include $(CHIBIOS)/os/hal/osal/rt-nil/osal.mk
# RTOS files (optional).
include $(CHIBIOS)/os/rt/rt.mk
include $(CHIBIOS)/os/common/ports/ARMCMx/compilers/GCC/mk/port_v7m.mk
# Auto-build files in ./source recursively.
include $(CHIBIOS)/tools/mk/autobuild.mk
# Other files (optional).u8g2.hu8g2.h
include $(CHIBIOS)/os/hal/lib/streams/streams.mk
include $(CHIBIOS)/os/various/shell/shell.mk

# Define linker script file here
ifeq ($(USE_MAPLEMINI_BOOTLOADER),1)
  LDSCRIPT = $(STARTUPLD)/STM32F103xB_maplemini_bootloader.ld
else
  LDSCRIPT = $(STARTUPLD)/STM32F103xB.ld
endif

# u8g2 sources

U8G2_SRC = \
       ${U8G2}/u8x8_setup.c \
       ${U8G2}/u8x8_u8toa.c \
       ${U8G2}/u8x8_d_ssd1306_96x16.c \
       ${U8G2}/u8g2_d_memory.c \
       ${U8G2}/u8g2_selection_list.c \
       ${U8G2}/u8g2_hvline.c \
       ${U8G2}/u8x8_d_ssd1306_128x64_noname.c \
       ${U8G2}/u8x8_message.c \
       ${U8G2}/u8x8_d_ssd1306_48x64.c \
       ${U8G2}/u8x8_d_ssd1305.c \
       ${U8G2}/u8g2_kerning.c \
       ${U8G2}/u8x8_input_value.c \
       ${U8G2}/u8x8_d_st7586s_s028hn118a.c \
       ${U8G2}/u8g2_polygon.c \
       ${U8G2}/u8x8_d_uc1604.c \
       ${U8G2}/u8x8_d_ssd1306_128x32.c \
       ${U8G2}/u8x8_d_ssd1322.c \
       ${U8G2}/u8x8_d_sbn1661.c \
       ${U8G2}/u8g2_input_value.c \
       ${U8G2}/u8x8_8x8.c \
       ${U8G2}/u8x8_debounce.c \
       ${U8G2}/u8x8_d_uc1638.c \
       ${U8G2}/u8g2_setup.c \
       ${U8G2}/u8x8_d_ist3020.c \
       ${U8G2}/u8x8_display.c \
       ${U8G2}/u8x8_d_ssd1317.c \
       ${U8G2}/u8x8_d_ks0108.c \
       ${U8G2}/u8g2_box.c \
       ${U8G2}/u8x8_selection_list.c \
       ${U8G2}/u8x8_d_ssd1326.c \
       ${U8G2}/u8x8_d_st75256.c \
       ${U8G2}/u8x8_d_ssd1309.c \
       ${U8G2}/u8x8_byte.c \
       ${U8G2}/u8x8_d_ssd1607_200x200.c \
       ${U8G2}/u8x8_d_pcd8544_84x48.c \
       ${U8G2}/u8log.c \
       ${U8G2}/u8x8_d_sh1107.c \
       ${U8G2}/u8log_u8x8.c \
       ${U8G2}/u8g2_message.c \
       ${U8G2}/u8x8_d_uc1610.c \
       ${U8G2}/u8x8_d_sh1106_72x40.c \
       ${U8G2}/u8g2_line.c \
       ${U8G2}/u8g2_intersection.c \
       ${U8G2}/u8g2_fonts.c \
       ${U8G2}/u8x8_d_sh1106_64x32.c \
       ${U8G2}/u8x8_d_sed1330.c \
       ${U8G2}/u8x8_d_pcf8814_hx1230.c \
       ${U8G2}/u8x8_d_il3820_296x128.c \
       ${U8G2}/u8x8_d_uc1701_dogs102.c \
       ${U8G2}/u8g2_cleardisplay.c \
       ${U8G2}/u8x8_d_ssd1325.c \
       ${U8G2}/u8g2_font.c \
       ${U8G2}/u8x8_d_uc1601.c \
       ${U8G2}/u8x8_d_pcf8812.c \
       ${U8G2}/u8x8_cad.c \
       ${U8G2}/u8x8_d_uc1611.c \
       ${U8G2}/u8x8_string.c \
       ${U8G2}/u8g2_ll_hvline.c \
       ${U8G2}/u8x8_fonts.c \
       ${U8G2}/u8x8_d_ld7032_60x32.c \
       ${U8G2}/u8x8_d_max7219.c \
       ${U8G2}/u8x8_d_st7588.c \
       ${U8G2}/u8x8_d_ssd1606_172x72.c \
       ${U8G2}/u8x8_d_st7586s_erc240160.c \
       ${U8G2}/u8x8_d_stdio.c \
       ${U8G2}/u8x8_d_sh1122.c \
       ${U8G2}/u8log_u8g2.c \
       ${U8G2}/u8g2_circle.c \
       ${U8G2}/u8x8_d_ssd1306_64x48.c \
       ${U8G2}/u8g2_buffer.c \
       ${U8G2}/u8g2_bitmap.c \
       ${U8G2}/u8x8_d_lc7981.c \
       ${U8G2}/u8x8_d_st7920.c \
       ${U8G2}/u8x8_d_t6963.c \
       ${U8G2}/u8x8_d_st7567.c \
       ${U8G2}/u8x8_d_ls013b7dh03.c \
       ${U8G2}/u8x8_d_ssd1327.c \
       ${U8G2}/u8x8_d_sh1108.c \
       ${U8G2}/u8x8_gpio.c \
       ${U8G2}/u8x8_d_uc1608.c \
       ${U8G2}/u8x8_d_uc1617.c \
       ${U8G2}/u8x8_d_ssd1329.c \
       ${U8G2}/u8x8_d_ssd1306_64x32.c \
       ${U8G2}/u8g2_d_setup.c \
       ${U8G2}/u8x8_d_uc1701_mini12864.c \
       ${U8G2}/u8x8_u16toa.c \
       ${U8G2}/u8x8_d_a2printer.c \
       ${U8G2}/u8x8_d_st7565.c \

# C sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CSRC = $(ALLCSRC) \
       ${U8G2_SRC} \
       src/main.c \
       src/app/drill_ctrl.c \
       src/app/usb_shell.c \
       src/app/menu.c \
       src/ch/usb_cfg.c \
       src/output/esc_pwm.c \
       src/sensors/cur_adc.c \
       src/sensors/rev_cnt.c \
       src/ui/glcd.c \
       src/ui/inc_enc.c \
       src/ui/ui.c \
       

# C++ sources that can be compiled in ARM or THUMB mode depending on the global
# setting.
CPPSRC = $(ALLCPPSRC)

# List ASM source files here.
ASMSRC = $(ALLASMSRC)

# List ASM with preprocessor source files here.
ASMXSRC = $(ALLXASMSRC)

# Inclusion directories.
INCDIR = $(CONFDIR) $(ALLINC) $(U8G2) inc 

# Define C warning options here.
CWARN = -Wall -Wextra -Wundef -Wstrict-prototypes

# Define C++ warning options here.
CPPWARN = -Wall -Wextra -Wundef

#
# Project, target, sources and paths
##############################################################################

##############################################################################
# Start of user section
#

# List all user C define here, like -D_DEBUG=1
UDEFS = -DSHELL_CMD_TEST_ENABLED=0

# Define ASM defines here
UADEFS =

# List all user directories here
UINCDIR =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

#
# End of user section
##############################################################################

##############################################################################
# Common rules
#

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
include $(RULESPATH)/arm-none-eabi.mk
include $(RULESPATH)/rules.mk

#
# Common rules
##############################################################################

##############################################################################
# Custom rules
#

#
# Custom rules
##############################################################################
