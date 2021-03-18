CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

CFLAGS =  -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -mno-avx -fno-strict-aliasing -ffreestanding -fno-stack-check -fno-merge-all-constants
CFLAGS += -maccumulate-outgoing-args --std=c11 -D__KERNEL__ -DCONFIG_X86_64 -DGNU_EFI_USE_MS_ABI -DEFI_FUNCTION_WRAPPER 
CFLAGS += -I ../gnu-efi/inc -I ../gnu-efi/inc/x86_64  -I ../include

LDFLAGS = -nostdlib -znocombreloc -shared -Bsymbolic -T ../gnu-efi/gnuefi/elf_x86_64_efi.lds -L ../gnu-efi/x86_64/lib -L ../gnu-efi/x86_64/gnuefi


SRCS = main.c efi.c elf.c
OBJS = main.o efi.o elf.o


../build/boot.efi:  main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64 main.so ../build/boot.efi
	

main.so:  $(OBJS)
	$(LD) $(OBJS) ../gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o $(LDFLAGS) -l:libefi.a -l:libgnuefi.a -o main.so

%.o:  %c.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJS) main.so ../build/boot.efi ../build/Penumbra.img


buildimg:
	dd if=/dev/zero of=../build/Penumbra.img bs=512 count=93750
	mformat -i ../build/Penumbra.img -f 1440 ::
	mmd -i ../build/Penumbra.img ::/EFI
	mmd -i ../build/Penumbra.img ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/boot.efi ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/startup.nsh ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/kernel.elf ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/gopfbdrv.elf ::/EFI/BOOT


run: ../build/boot.efi buildimg
	$(QEMU) -drive format=raw,file=../build/Penumbra.img -m 256M -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE_4M.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS_4M.fd,readonly=on -net none

