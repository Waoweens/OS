// Kode kernel
// PENTING : FUNGSI PERTAMA YANG DIDEFINISIKAN ADALAH main(),
//   cek spesifikasi untuk informasi lebih lanjut

// TODO : Tambahkan implementasi kode C

#include "header/kernel.h"

int main() {
	char buf[128];

	fillMap();
	makeInterrupt21();
	clearScreen();
	printString("Hello, World!\r\n");

	while (true)
		;
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

void printChar(char character) { interrupt(0x10, 0xE00 + character, 0, 0, 0); }

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

void readSector(byte *buffer, int sector_number) {
	int sector_read_count = 0x01;
	int cylinder, sector;
	int head, drive;

	cylinder = div(sector_number, 36) << 8;	 // CH
	sector = mod(sector_number, 18) + 1;	 // CL

	head = mod(div(sector_number, 18), 2) << 8;	 // DH
	drive = 0x00;								 // DL

	interrupt(0x13,						   // interrupt 13
			  0x0201 | sector_read_count,  // AX
			  buffer,					   // BX
			  cylinder | sector,		   // CX
			  head | drive				   // DX
	);
}

void writeSector(byte *buffer, int sector_number) {
	int sector_write_count = 0x01;
	int cylinder, sector;
	int head, drive;

	cylinder = div(sector_number, 36) << 8;	 // CH
	sector = mod(sector_number, 18) + 1;	 // CL

	head = mod(div(sector_number, 18), 2) << 8;	 // DH
	drive = 0x00;								 // DL

	interrupt(0x13,							// interrupt 13
			  0x0301 | sector_write_count,	// AX
			  buffer,						// BX
			  cylinder | sector,			// CX
			  head | drive					// DX
	);
}

void fillMap() {
	struct map_filesystem map_fs_buffer;
	int i;
	// Load filesystem map
	readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
	/*
		Edit filesystem map
	*/

	// 0 - 15 true
	for (i = 0; i < 16; i++) {
		map_fs_buffer.is_filled[i] = true;
	}

	// 256 - 511 true
	for (i = 256; i < 512; i++) {
		map_fs_buffer.is_filled[i] = true;
	}

	// Update filesystem map
	writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);
}

// i don't know what i'm doing
void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
	struct node_filesystem node_fs_buffer;
	struct sector_filesystem sector_fs_buffer;
	struct node_entry node_buffer;
	struct sector_entry sector_buffer;
	int i;

	// Tambahkan tipe data yang dibutuhkan
	// ?

	// Masukkan filesystem dari storage ke memori buffer
	readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

	// 1. Cari node dengan nama dan lokasi yang sama pada filesystem.
	//    Jika ditemukan node yang cocok, lanjutkan ke langkah ke - 2.
	//    Jika tidak ditemukan kecocokan, tuliskan retcode FS_R_NODE_NOT_FOUND
	//    dan keluar.
	
	for (i = 0; i < 64; i++) {
		if (node_fs_buffer.nodes[i].parent_node_index == metadata->parent_index &&
			strcmp(node_fs_buffer.nodes[i].name, metadata->node_name) == 0) {
			break;
		}
	}

	if (i == 64) {
		*return_code = FS_R_NODE_NOT_FOUND;
		return;
	}

	// 2. Cek tipe node yang ditemukan
	//    Jika tipe node adalah file, lakukan proses pembacaan.
	//    Jika tipe node adalah folder, tuliskan retcode FS_R_TYPE_IS_FOLDER
	//    dan keluar.

	if (node_fs_buffer.nodes[i].sector_entry_index == FS_NODE_S_IDX_FOLDER) {
		*return_code = FS_R_TYPE_IS_FOLDER;
		return;
	}

	// Pembacaan
	// 1. memcpy() entry sector sesuai dengan byte S
	memcpy(&node_buffer, &(node_fs_buffer.nodes[i]), sizeof(struct node_entry));
	memcpy(&sector_buffer, &(sector_fs_buffer.sector_list[node_buffer.sector_entry_index]), sizeof(struct sector_entry));

	// 2. Lakukan iterasi proses berikut, i = 0..15
	for (i = 0; i < 16; i++) {
		// 3. Baca byte entry sector untuk mendapatkan sector number partisi
		// file

		// 4. Jika byte bernilai 0, selesaikan iterasi
		if (sector_buffer.sector_numbers[i] == 0) {
			break;
		}

		// 5. Jika byte valid, lakukan readSector()
		//    dan masukkan kedalam buffer yang disediakan pada metadata
		readSector(metadata->buffer + i * 512, sector_buffer.sector_numbers[i]);

		// 6. Lompat ke iterasi selanjutnya hingga iterasi selesai

		
	}

	// 7. Tulis retcode FS_SUCCESS dan ganti filesize
	//    pada akhir proses pembacaan.

	*return_code = FS_SUCCESS;
	metadata->filesize = 512 * i;
}

void write(struct file_metadata *metadata, enum fs_retcode *return_code) {

}