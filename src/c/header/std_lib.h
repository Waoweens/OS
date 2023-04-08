#include "std_type.h"

// Operator matematika umum
// Implementasikan
// Tips : Gunakan C integer division
int div(int a, int b);

int mod(int a, int n);

// Operasi standar bahasa C
// Implementasikan

// Mengcopy n bytes yang ditunjuk src ke dest
void memcpy(byte *dest, byte *src, unsigned int n);

// Mengembalikan panjang suatu null terminated string
unsigned int strlen(char *string);

// Mengembalikan true jika string sama
bool strcmp(char *s1, char *s2);

// Melakukan penyalinan null terminated string
void strcpy(char *dst, char *src);

// Mengosongkan byte array yang memiliki panjang n
void clear(byte *ptr, unsigned int n);

// concat string
void strcat(char *dst, char *src);

// clear array
void clrarr(char *arr, int size);

// starts with
bool startsWith(char *str, char *prefix);