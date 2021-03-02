@echo -off
mode 80 25

cls
if exist .\efi\boot\boot.efi then
 .\efi\boot\boot.efi
 goto END
endif

if exist fs0:\efi\boot\boot.efi then
 fs0:
 echo Found bootloader on fs0:
 efi\boot\boot.efi
 goto END
endif

if exist fs1:\efi\boot\boot.efi then
 fs1:
 echo Found bootloader on fs1:
 efi\boot\boot.efi
 goto END
endif

if exist fs2:\efi\boot\boot.efi then
 fs2:
 echo Found bootloader on fs2:
 efi\boot\boot.efi
 goto END
endif

if exist fs3:\efi\boot\boot.efi then
 fs3:
 echo Found bootloader on fs3:
 efi\boot\boot.efi
 goto END
endif

if exist fs4:\efi\boot\boot.efi then
 fs4:
 echo Found bootloader on fs4:
 efi\boot\boot.efi
 goto END
endif

if exist fs5:\efi\boot\boot.efi then
 fs5:
 echo Found bootloader on fs5:
 efi\boot\boot.efi
 goto END
endif

echo "Bootloader not found."
 
:END
