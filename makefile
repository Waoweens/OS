# Makefile
all: diskimage bootloader stdlib kernel
	@echo -e "\033[1;32mSelesai!\033[0m"

# Recipes
diskimage:
	@echo -e "\033[1;33mMembuat disk image...\033[0m"
	dd if=/dev/zero of=out/system.img bs=512 count=2880

bootloader:
	@echo -e "\033[1;33mMembuat bootloader...\033[0m"
	nasm src/asm/bootloader.asm -o out/bootloader
	dd if=out/bootloader of=out/system.img bs=512 count=1 conv=notrunc

kernel:
	@echo -e "\033[1;33mMembuat kernel...\033[0m"
	bcc -ansi -c -o out/kernel.o src/c/kernel.c
	nasm -f as86 src/asm/kernel.asm -o out/kernel_asm.o
	ld86 -o out/kernel -d out/kernel.o out/kernel_asm.o out/stdlib.o
	dd if=out/kernel of=out/system.img bs=512 conv=notrunc seek=1

stdlib:
	@echo -e "\033[1;33mMembuat stdlib...\033[0m"
	bcc -ansi -c -o out/stdlib.o src/c/std_lib.c

run:
	@echo -e "\032[1;33mMenjalankan...\033[0m"
	-echo "c" | bochs -f src/config/if2230.config -q
	cd out && ./tc_gen A
	-echo "c" | bochs -f src/config/if2230.config -q

build-run: all run
