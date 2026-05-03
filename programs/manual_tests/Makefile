# Makefile pour les programmes de test manuels RISC-V
#
# Convertit chaque .S en .bin via :
#   .S --[as]--> .o --[objcopy -O binary]--> .bin

RISCV_AS      = riscv64-unknown-elf-as
RISCV_OBJCOPY = riscv64-unknown-elf-objcopy
ASFLAGS       = -march=rv32i_zicsr -mabi=ilp32

SOURCES = $(wildcard *.S)
OBJECTS = $(SOURCES:.S=.o)
BINS    = $(SOURCES:.S=.bin)

.PHONY: all clean
.PRECIOUS: %.o

all: $(BINS)

%.o: %.S
	$(RISCV_AS) $(ASFLAGS) $< -o $@

%.bin: %.o
	$(RISCV_OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.bin
