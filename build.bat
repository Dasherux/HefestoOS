@echo off
setlocal

echo [BUILD] Assembling bootloader...

nasm -f bin boot\boot.asm -o boot.bin
if errorlevel 1 goto error

nasm -f bin boot\stage2.asm -o stage2.bin
if errorlevel 1 goto error

echo [BUILD] Compiling Kernel...

nasm -f elf64 kernel\entry.asm -o entry.o
if errorlevel 1 goto error

clang --target=x86_64-unknown-none-elf ^
    -c kernel\kernel.c ^
    -o kernel.o ^
    -ffreestanding ^
    -fno-stack-protector ^
    -fno-pic -fno-pie ^
    -mno-red-zone ^
    -mno-sse -mno-sse2 ^
    -mcmodel=small ^
    -O0

if errorlevel 1 goto error

echo [BUILD] Compiling modules...

clang --target=x86_64-unknown-none-elf ^
    -c system\console.c ^
    -o console.o ^
    -ffreestanding ^
    -fno-stack-protector ^
    -fno-pic -fno-pie ^
    -mno-red-zone ^
    -mno-sse -mno-sse2 ^
    -mcmodel=small ^
    -O0
	
if errorlevel 1 goto error
echo [MODULES] Console compiled!

clang --target=x86_64-unknown-none-elf ^
    -c system\shell.c ^
    -o shell.o ^
    -ffreestanding ^
    -fno-stack-protector ^
    -fno-pic -fno-pie ^
    -mno-red-zone ^
    -mno-sse -mno-sse2 ^
    -mcmodel=small ^
    -O0
	
if errorlevel 1 goto error
echo [MODULES] Shell compiled!

clang --target=x86_64-unknown-none-elf ^
    -c drivers\keyboard.c ^
    -o keyboard.o ^
    -ffreestanding ^
    -fno-stack-protector ^
    -fno-pic -fno-pie ^
    -mno-red-zone ^
    -mno-sse -mno-sse2 ^
    -mcmodel=small ^
    -O0

if errorlevel 1 goto error
echo [MODULES] Keyboard compiled!

echo [BUILD] Compiling libraries...

clang --target=x86_64-unknown-none-elf ^
    -c lib\string.c ^
    -o string.o ^
    -ffreestanding ^
    -fno-stack-protector ^
    -fno-pic -fno-pie ^
    -mno-red-zone ^
    -mno-sse -mno-sse2 ^
    -mcmodel=small ^
    -O0

if errorlevel 1 goto error
echo [LIBS] "String" compiled!

echo [BUILD] Linking ELF64...

ld.lld -m elf_x86_64 ^
-T kernel/linker.ld ^
-nostdlib ^
-o kernel.elf ^
entry.o kernel.o console.o shell.o keyboard.o string.o

if errorlevel 1 goto error

echo [BUILD] Creating flat binary...

llvm-objcopy -O binary kernel.elf kernel.bin
if errorlevel 1 goto error

echo [BUILD] Checking Kernel size...

C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe -NoProfile -Command "$b=[IO.File]::ReadAllBytes('kernel.bin'); if($b.Length -gt 61440){throw 'Kernel exceeds 60 KiB'}; $p=New-Object byte[] 61440; [Array]::Copy($b,$p,$b.Length); [IO.File]::WriteAllBytes('kernel.pad',$p)"

if errorlevel 1 goto error

echo [BUILD] Creating disk image...

copy /b boot.bin+stage2.bin+kernel.pad os.img >nul

echo [BUILD] Done!
goto end

:error
echo [BUILD] Compilation failed!
pause
exit /b 1

:end
echo [BUILD] OS image ready: os.img
os.img
pause
endlocal