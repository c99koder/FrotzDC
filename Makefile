#
# FrotzDC 1.0 Makefile
#
CC = kos-cc

KOS_ARCH_DIR = $(KOS_BASE)/kernel/arch/$(KOS_ARCH)
KOS_START = $(KOS_ARCH_DIR)/kernel/startup.o

CFLAGS = $(KOS_CFLAGS) -I../libc99/include -I$(KOS_INCS) \
	-I$(KOS_BASE)/libc/include -I$(KOS_BASE)/addons/include\
	-I$(KOS_BASE)/kernel/arch/$(KOS_ARCH)/include -D_arch_$(KOS_ARCH)

EXTENSION = .elf


#####################################################
# Nothing under this line should need to be changed.
#####################################################

SRCDIR = src

VERSION = 2.43
 
NAME = frotz
BINNAME = $(NAME)

COMMON_DIR = src/common
COMMON_OBJECT = $(COMMON_DIR)/buffer.o \
		$(COMMON_DIR)/err.o \
		$(COMMON_DIR)/fastmem.o \
		$(COMMON_DIR)/files.o \
		$(COMMON_DIR)/hotkey.o \
		$(COMMON_DIR)/input.o \
		$(COMMON_DIR)/main.o \
		$(COMMON_DIR)/math.o \
		$(COMMON_DIR)/object.o \
		$(COMMON_DIR)/process.o \
		$(COMMON_DIR)/quetzal.o \
		$(COMMON_DIR)/random.o \
		$(COMMON_DIR)/redirect.o \
		$(COMMON_DIR)/screen.o \
		$(COMMON_DIR)/sound.o \
		$(COMMON_DIR)/stream.o \
		$(COMMON_DIR)/table.o \
		$(COMMON_DIR)/text.o \
		$(COMMON_DIR)/variable.o

DC_DIR = src/dc
DC_OBJECT =	$(DC_DIR)/dc_init.o \
		$(DC_DIR)/dc_input.o \
		$(DC_DIR)/dc_output.o \
		$(DC_DIR)/dc_pic.o \
		$(DC_DIR)/dc_sample.o \
		$(DC_DIR)/dc_file.o
#		$(DC_DIR)/romdisk.o

frotz.elf:		$(COMMON_OBJECT) $(DC_OBJECT)
#	$(CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -g -o frotz.elf $(COMMON_OBJECT) $(DC_OBJECT)\
#		-L$(KOS_BASE)/lib -L. -L$(KOS_BASE)/addons/lib -L../libc99/dc -lc99 -lmp3 -lgl -lk++ -lm -lkallisti -lgcc ${KOS_LIBS}
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -g -o frotz.elf $(KOS_START) \
		$(COMMON_OBJECT) $(DC_OBJECT) -L$(KOS_BASE)/lib -L../libc99/dc -lc99 \
		-lkosutils -lgl -lmp3 -lk++ -lz -lm ${KOS_LIBS} -T shlelf.x
romdisk.img:
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

$(DC_DIR)/romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk $(DC_DIR)/romdisk.o

.SUFFIXES:
.SUFFIXES: .c .o .h

.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -o $@ -c $<

clean:
	rm -f $(DC_OBJECT)
	rm -f $(COMMON_OBJECT)
	rm -f frotz.elf

