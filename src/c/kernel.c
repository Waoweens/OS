// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"

int main() {
	char buf[128];
	clearScreen();

	printString("Hello, World!\r\n");

	

	while (true) {
		readString(buf);
		printString(buf);
		printString("\r\n");
	}
}

void handleInterrupt21(int AX, int BX, int CX, int DX) {
	switch (AX) {
		case 0x0:
			printString(BX);
			break;
		case 0x1:
			readString(BX);
			break;
		default:
			printString("Invalid interrupt");
	}
}

void printString(char *string) {
	int i = 0;

	while (string[i] != '\0') {
		interrupt(0x10, 0xE00 + string[i], 0, 0, 0);
		i++;
	}
}

void printChar(char character) {
	interrupt(0x10, 0xE00 + character, 0, 0, 0);
}

void readString(char *string) {
	int i = 0;
	char c;
	while (true) {
		c = interrupt(0x16, 0, 0, 0, 0);
		if (c == 0xD) {
			string[i] = '\0';
			printString("\r\n");
			break;
		} else if (c == 0x8) {
			printString("\b \b");
			i--;
		} else {
			string[i] = c;
			printChar(c);
			i++;
		}
	}
}

void clearScreen() {
	// 80x25
	int i = 0;

	for (i = 0; i < 80 * 25; i++) {
		putInMemory(0xB000, 0x8000 + 2 * i, 0x20);
		putInMemory(0xB000, 0x8001 + 2 * i, 0xF);
	}

	interrupt(0x10, 0x0200, 0, 0, 0x0000);
}
