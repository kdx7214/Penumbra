CC = gcc
LD = ld
LDS = kernel.ld
QEMU = qemu-system-x86_64

CFLAGS =  -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib
BUILD = ../build


SRCDIR = src
OBJDIR = build

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

kernel: $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


link:
	$(LD) $(LDFLAGS) -o $(BUILD)/kernel.elf $(OBJS)

clean:
	@rm build/*
	@rm ../build/Penumbra.img
	@rm ../build/kernel.elf

buildimg:
	dd if=/dev/zero of=../build/Penumbra.img bs=512 count=93750
	mformat -i ../build/Penumbra.img -f 1440 ::
	mmd -i ../build/Penumbra.img ::/EFI
	mmd -i ../build/Penumbra.img ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/boot.efi ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/startup.nsh ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/kernel.elf ::/EFI/BOOT

run:
	$(QEMU) -drive format=raw,file=../build/Penumbra.img -m 256M -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE_4M.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS_4M.fd,readonly=on -net none

