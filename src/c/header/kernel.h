// Kernel header

#include "std_type.h"
#include "std_lib.h"
#include "filesystem.h"

// Fungsi bawaan
extern void putInMemory(int segment, int address, char character);
extern int interrupt(int int_number, int AX, int BX, int CX, int DX);
void makeInterrupt21();
void handleInterrupt21(int AX, int BX, int CX, int DX);
void fillMap();


// TODO : Implementasikan

void printString(char *string);
void printChar(char character);
void readString(char *string);
void clearScreen();

void writeSector(byte *buffer, int sector_number);
void readSector(byte *buffer, int sector_number);

void write(struct file_metadata *metadata, enum fs_retcode *return_code);
void read(struct file_metadata *metadata, enum fs_retcode *return_code);

void shell();

void printCWD(char *path_str, byte current_dir);
void ls(char* dir_name, byte current_dir);
void cat(char* file_name, byte current_dir);