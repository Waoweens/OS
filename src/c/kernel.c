// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"

int main() {
	char* string = "Hello, World!";
	int i = 0;

	for (i = 0; i < 13; i++) {
		byte warna = 0xD;

		putInMemory(0xB000, 0x8000 + 2*i, string[i]);
		putInMemory (0xB000, 0x8001 + 2*i, warna);
	}

	while(true);
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
	
}