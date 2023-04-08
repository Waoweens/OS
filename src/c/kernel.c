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

	shell();

	while (true)
		;
}

void shell() {
	char input_buf[64];
	char path_str[128];
	byte current_dir = 0x0;

	while(true) {
		printString("user@host:");
		printCWD(path_str, current_dir);
		printString(" $ ");
		readString(input_buf);

		if (startsWith(input_buf, "pwd")) {
			printCWD(path_str, current_dir);
			printString("\r\n");
		} else if (startsWith(input_buf, "ls")) {
			ls("", current_dir);

		} else if (startsWith(input_buf, "cat")) {
			cat(input_buf + 4, current_dir);
		} else if (input_buf[0] == 0) {
			// do nothing
		} else {
			printString("Command not found\r\n");
		}
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
		case 0x2:
			readSector(BX, CX);
			break;
		case 0x3:
			writeSector(BX, CX);
			break;
		case 0x4:
			read(BX, CX);
			break;
		case 0x5:
			write(BX, CX);
			break;
		default:
			printString("Invalid interrupt");
	}
}

// printCWD(path_str, current_dir)
void printCWD(char* path_str, byte current_dir) {
	struct node_filesystem node_fs_buffer;
	byte array[64];
	int i = 0;
	int j = 0;

	clrarr(path_str, 128);
	
	readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

	if (current_dir == FS_NODE_P_IDX_ROOT) {
		printString("/");
		strcpy(path_str, "/");
		return;
	} 

	while (current_dir != FS_NODE_P_IDX_ROOT) {
		array[i] = current_dir;
		current_dir = node_fs_buffer.nodes[current_dir].parent_node_index;
		i++;
	}

	for (j = i-1; j >= 0; j--) {
		printString("/");
		strcat(path_str, "/");
		printString(node_fs_buffer.nodes[array[j]].name);
		strcat(path_str, node_fs_buffer.nodes[array[j]].name);
	}

}

void ls(char* dir_name, byte current_dir) {
	struct node_filesystem node_fs_buffer;
	int i = 0;
	
	readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

	for (i = 0; i < 64; i++) {
		if (node_fs_buffer.nodes[i].parent_node_index == current_dir) {
			if (node_fs_buffer.nodes[i].name[0] == 0) {
				continue;
			}

			printString(node_fs_buffer.nodes[i].name);
			printString("\r\n");
		}
	}
}

void cat(char* file_name, byte current_dir) {
	struct file_metadata metadata;
	byte buffer[8192];
	enum fs_retcode retcode;

	metadata.buffer = buffer;
	metadata.node_name = file_name;
	metadata.parent_index = current_dir;
	metadata.filesize = 0;

	read(&metadata, &retcode);
	switch (retcode) {
		case FS_SUCCESS:
			printString(metadata.buffer);
			printString("\r\n");
			break;
		case FS_R_NODE_NOT_FOUND:
			printString("File not found\r\n");
			break;
		case FS_R_TYPE_IS_FOLDER:
			printString("File is a directory\r\n");
			break;
		default:
			printString("Unknown error\r\n");
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
			if (i == 0)
				continue;
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

void read(struct file_metadata *metadata, enum fs_retcode *return_code) {
	struct node_filesystem node_fs_buffer;
	struct sector_filesystem sector_fs_buffer;

	// Tambahkan tipe data yang dibutuhkan
	struct node_entry node_buffer;
	struct sector_entry sector_buffer;
	int i;

	// Masukkan filesystem dari storage ke memori buffer
	readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);

	// 1. Cari node dengan nama dan lokasi yang sama pada filesystem.
	//    Jika ditemukan node yang cocok, lanjutkan ke langkah ke - 2.
	//    Jika tidak ditemukan kecocokan, tuliskan retcode FS_R_NODE_NOT_FOUND
	//    dan keluar.

	for (i = 0; i < 64; i++) {
		if (node_fs_buffer.nodes[i].parent_node_index ==
				metadata->parent_index &&
			strcmp(node_fs_buffer.nodes[i].name, metadata->node_name)) {
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
	memcpy(&sector_buffer,
		   &(sector_fs_buffer.sector_list[node_buffer.sector_entry_index]),
		   sizeof(struct sector_entry));

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
	struct node_filesystem node_fs_buffer;
	struct sector_filesystem sector_fs_buffer;
	struct map_filesystem map_fs_buffer;

	// Tambahkan tipe data yang dibutuhkan
	struct node_entry node_buffer;
	struct sector_entry sector_buffer;
	int i, j, sector_index, node_index;

	// Masukkan filesystem dari storage ke memori
	readSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	readSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
	readSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

	// 1. Cari node dengan nama dan lokasi parent yang sama pada node.
	//    Jika tidak ditemukan kecocokan, lakukan proses ke-2.
	//    Jika ditemukan node yang cocok, tuliskan retcode
	//    FS_W_FILE_ALREADY_EXIST dan keluar.
	for (i = 0; i < 64; i++) {
		if (node_fs_buffer.nodes[i].parent_node_index ==
				metadata->parent_index &&
			strcmp(node_fs_buffer.nodes[i].name, metadata->node_name)) {
			*return_code = FS_W_FILE_ALREADY_EXIST;
			return;
		}
	}

	// 2. Cari entri kosong pada filesystem node dan simpan indeks.
	//    Jika ada entry kosong, simpan indeks untuk penulisan.
	//    Jika tidak ada entry kosong, tuliskan FS_W_MAXIMUM_NODE_ENTRY
	//    dan keluar.
	for (i = 0; i < 64; i++) {
		if (node_fs_buffer.nodes[i].name[0] == 0) {
			memcpy(&node_buffer, &(node_fs_buffer.nodes[i]),
				   sizeof(struct node_entry));
			node_index = i;
			break;
		}
	}

	if (i == 64) {
		*return_code = FS_W_MAXIMUM_NODE_ENTRY;
		return;
	}

	// 3. Cek dan pastikan entry node pada indeks P adalah folder.
	//    root terdefinisi sebagai suatu folder.
	//    Jika pada indeks tersebut adalah file atau entri kosong,
	//    Tuliskan retcode FS_W_INVALID_FOLDER dan keluar.

	if (metadata->parent_index != FS_NODE_P_IDX_ROOT) {
		if (node_fs_buffer.nodes[metadata->parent_index].sector_entry_index !=
			FS_NODE_S_IDX_FOLDER) {
			*return_code = FS_W_INVALID_FOLDER;
			return;
		}
	}

	// 4. Dengan informasi metadata filesize, hitung sektor-sektor
	//    yang masih kosong pada filesystem map. Setiap byte map mewakili
	//    satu sektor sehingga setiap byte mewakili 512 bytes pada storage.
	//    Jika empty space tidak memenuhi, tuliskan retcode
	//    FS_W_NOT_ENOUGH_STORAGE dan keluar.
	//    Jika ukuran filesize melebihi 8192 bytes, tuliskan retcode
	//    FS_W_NOT_ENOUGH_STORAGE dan keluar.
	//    Jika tersedia empty space, lanjutkan langkah ke-5.
	for (i = 0; i < 64; i++) {
		if (map_fs_buffer.is_filled[i] == 0) {
			j++;
		}
	}

	if (metadata->filesize > j * 512) {
		*return_code = FS_W_NOT_ENOUGH_STORAGE;
		return;
	}

	if (metadata->filesize > 8 * 1024) {
		*return_code = FS_W_NOT_ENOUGH_STORAGE;
		return;
	}

	// 5. Cek pada filesystem sector apakah terdapat entry yang masih kosong.
	//    Jika ada entry kosong dan akan menulis file, simpan indeks untuk
	//    penulisan.
	//    Jika tidak ada entry kosong dan akan menulis file, tuliskan
	//    FS_W_MAXIMUM_SECTOR_ENTRY dan keluar.
	//    Selain kondisi diatas, lanjutkan ke proses penulisan.
	for (i = 0; i < 32; i++) {
		for (j = 0; j < 16; j++) {
			if (sector_fs_buffer.sector_list[i].sector_numbers[j] != 0) {
				break;
			}
		}

		if (j == 16) {
			sector_index = i;
			memcpy(&sector_buffer, &(sector_fs_buffer.sector_list[i]),
				   sizeof(struct sector_entry));
			break;
		}
	}

	if (i == 32) {
		*return_code = FS_W_MAXIMUM_SECTOR_ENTRY;
		return;
	}

	// Penulisan
	// 1. Tuliskan metadata nama dan byte P ke node pada memori buffer
	memcpy(node_buffer.name, metadata->node_name, 14);

	// 2. Jika menulis folder, tuliskan byte S dengan nilai
	//    FS_NODE_S_IDX_FOLDER dan lompat ke langkah ke-8
	if (metadata->filesize == 0) {
		node_buffer.sector_entry_index = FS_NODE_S_IDX_FOLDER;
		//
	}
	// 3. Jika menulis file, tuliskan juga byte S sesuai indeks sector
	else {
		node_buffer.sector_entry_index = sector_index;

		// 4. Persiapkan variabel j = 0 untuk iterator entry sector yang kosong
		j = 0;

		// 5. Persiapkan variabel buffer untuk entry sector kosong

		// 6. Lakukan iterasi berikut dengan kondisi perulangan
		//    (penulisan belum selesai && i = 0..255
		for (i = 0; i < 256; i++) {
			// 1. Cek apakah map[i] telah terisi atau tidak
			if (map_fs_buffer.is_filled[i] == 1) {
				// 2. Jika terisi, lanjutkan ke iterasi selanjutnya / continue
				continue;
			}

			// 3. Tandai map[i] terisi
			map_fs_buffer.is_filled[i] = 1;

			// 4. Ubah byte ke-j buffer entri sector dengan i
			sector_buffer.sector_numbers[j] = i;

			// 5. Tambah nilai j dengan 1
			j++;

			// 6. Lakukan writeSector() dengan file pointer buffer pada
			//    metadata dan sektor tujuan i
			writeSector(&(metadata->buffer[512*j]), i);

			// 7. Jika ukuran file yang telah tertulis lebih besar atau sama
			//    dengan filesize pada metadata, penulisan selesai
			if (512*j >= metadata->filesize) {
				break;
			}
		}

		// 7. Lakukan update dengan memcpy() buffer entri sector dengan
		//    buffer filesystem sector
		memcpy(&(sector_fs_buffer.sector_list[sector_index]), &sector_buffer,
			   sizeof(struct sector_entry));
		
		memcpy(&(node_fs_buffer.nodes[node_index]), &node_buffer,
			   sizeof(struct node_entry));
	}

	// 8. Lakukan penulisan seluruh filesystem (map, node, sector) ke storage
	//    menggunakan writeSector() pada sektor yang sesuai
	writeSector(&sector_fs_buffer, FS_SECTOR_SECTOR_NUMBER);
	writeSector(&(node_fs_buffer.nodes[0]), FS_NODE_SECTOR_NUMBER);
	writeSector(&(node_fs_buffer.nodes[32]), FS_NODE_SECTOR_NUMBER + 1);
	writeSector(&map_fs_buffer, FS_MAP_SECTOR_NUMBER);

	// 9. Kembalikan retcode FS_SUCCESS
	*return_code = FS_SUCCESS;
}