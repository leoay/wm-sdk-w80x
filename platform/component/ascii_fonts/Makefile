TOP_DIR = ../../..
sinclude $(TOP_DIR)/tools/W800/conf.mk

ifndef PDIR
GEN_LIBS = libasciifonts$(LIB_EXT)
endif

#DEFINES +=

sinclude $(TOP_DIR)/tools/W800/rules.mk
INCLUDES := $(INCLUDES) -I $(PDIR)include
PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
