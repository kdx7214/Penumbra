CC = gcc
LD = ld
LDS = gopfbdrv.ld
QEMU = qemu-system-x86_64

CFLAGS =  -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -mno-avx -fno-strict-aliasing -ffreestanding -fno-stack-check -fno-merge-all-constants
CFLAGS += -maccumulate-outgoing-args -D__KERNEL__ -DCONFIG_X86_64 -DGNU_EFI_USE_MS_ABI -DEFI_FUNCTION_WRAPPER -std=c++11
CFLAGS += -I ../gnu-efi/inc -I ../gnu-efi/inc/x86_64  -I ../include

LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib
BUILD = ../build


SRCDIR = src
OBJDIR = build

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))

gopfbdrv: $(OBJS) font link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

font:
	objcopy -O elf64-x86-64 -B i386 -I binary $(SRCDIR)/zap-light24.psf $(OBJDIR)/font.o

link:
	$(LD) $(LDFLAGS) -o $(BUILD)/gopfbdrv.elf $(OBJS) $(OBJDIR)/font.o

clean:
	@rm build/*
	@rm ../build/Penumbra.img
	@rm ../build/gopfbdrv.elf

buildimg:
	dd if=/dev/zero of=../build/Penumbra.img bs=512 count=93750
	mformat -i ../build/Penumbra.img -f 1440 ::
	mmd -i ../build/Penumbra.img ::/EFI
	mmd -i ../build/Penumbra.img ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/boot.efi ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/startup.nsh ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/kernel.elf ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/gopfbdrv.elf ::/EFI/BOOT

run: buildimg
	$(QEMU) -vga std -drive format=raw,file=../build/Penumbra.img -m 256M -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE_4M.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS_4M.fd,readonly=on -net none

