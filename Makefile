PROJECT_NAME = 6502_emu

SRCDIR := src
OBJDIR := bin
BUILDDIR := out

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

C_SRC   := $(call rwildcard,$(SRCDIR),*.c)
ASM_SRC := $(call rwildcard,$(SRCDIR),*.S) $(call rwildcard,$(SRCDIR),*.s)

C_OBJS  := $(patsubst $(SRCDIR)/%.c,   $(OBJDIR)/%.o, $(C_SRC))
ASM_OBJS:= $(patsubst $(SRCDIR)/%.S,   $(OBJDIR)/%.o, $(ASM_SRC))
OBJS    := $(C_OBJS) $(ASM_OBJS)

CROSS_COMPILE ?= 

AS := $(CROSS_COMPILE)gcc
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)gcc
OC := $(CROSS_COMPILE)objcopy

CFLAGS  = -O2 -Wall -std=gnu99
ASFLAGS = 
LDFLAGS = 

all: build
	@echo "$(BUILDDIR)/$(PROJECT_NAME).img is ready"

build: clean $(OBJS) link

# Compile C -> object file only (-c flag)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "[CC]\t$@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Assemble .S -> object file only (-c flag)
$(OBJDIR)/%.o: $(SRCDIR)/%.S
	@echo "[AS]\t$@"
	@$(AS) $(ASFLAGS) -c $< -o $@

# Assemble .s -> object file only (-c flag)
$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@echo "[AS]\t$@"
	@$(AS) $(ASFLAGS) -c $< -o $@

link:
	@echo "[LD]\t$(BUILDDIR)/$(PROJECT_NAME)"
	@$(LD) -ffreestanding -O2 $(OBJS) \
	    -o $(BUILDDIR)/$(PROJECT_NAME) $(LDFLAGS)

clean:
	@echo "Cleaning project $(PROJECT_NAME)..."
	@rm -rf $(OBJDIR)/* $(BUILDDIR)/*
	@touch $(OBJDIR)/git_keep_this_thing
	@touch $(BUILDDIR)/git_keep_this_thing
	@echo "Clean complete!"

run: build
	@echo "[RUN] (BUILDDIR)/$(PROJECT_NAME)"
	@./$(BUILDDIR)/$(PROJECT_NAME)
