CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-x86_64

CFLAGS =  -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -mno-avx -fno-strict-aliasing -ffreestanding -fno-stack-check -fno-merge-all-constants
CFLAGS += -maccumulate-outgoing-args --std=c11 -D__KERNEL__ -DCONFIG_X86_64 -DGNU_EFI_USE_MS_ABI -DEFI_FUNCTION_WRAPPER 
CFLAGS += -I ../gnu-efi/inc -I ../gnu-efi/inc/x86_64 

LDFLAGS = -nostdlib -znocombreloc -shared -Bsymbolic -T ../gnu-efi/gnuefi/elf_x86_64_efi.lds -L ../gnu-efi/x86_64/lib -L ../gnu-efi/x86_64/gnuefi


SRCS = main.c
OBJS = main.o


../build/boot.efi:  main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64 main.so ../build/boot.efi
	

main.so:  main.o
	$(LD) main.o ../gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o $(LDFLAGS) -l:libefi.a -l:libgnuefi.a -o main.so

main.o:  main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm main.o main.so ../build/boot.efi ../build/Penumbra.img


buildimg:
	dd if=/dev/zero of=../build/Penumbra.img bs=512 count=93750
	mformat -i ../build/Penumbra.img -f 1440 ::
	mmd -i ../build/Penumbra.img ::/EFI
	mmd -i ../build/Penumbra.img ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/boot.efi ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/startup.nsh ::/EFI/BOOT
	mcopy -i ../build/Penumbra.img ../build/bootstrap.elf ::/EFI/BOOT


run:
	$(QEMU) -drive format=raw,file=../build/Penumbra.img -m 256M -cpu qemu64 \
		-drive if=pflash,format=raw,unit=0,file=/usr/share/OVMF/OVMF_CODE_4M.fd,readonly=on \
		-drive if=pflash,format=raw,unit=1,file=/usr/share/OVMF/OVMF_VARS_4M.fd,readonly=on -net none

