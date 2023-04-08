#include "header/std_lib.h"

// Tambahkan implementasi kode C

int div(int a, int b) {
	int i = 0;
	while (a - b >= 0) {
		a -= b;
		i++;
	}
	return i;
}

int mod(int a, int n) {
	while (a - n >= 0) {
		a -= n;
	}
	return a;
}

// Mengcopy n bytes yang ditunjuk src ke dest
void memcpy(byte *dest, byte *src, unsigned int n) {
	int i;
	for (i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

// Mengembalikan panjang suatu null terminated string
unsigned int strlen(char *string) {
	int i = 0;
	while (string[i] != '\0') {
		i++;
	}
	return i;
}

// Mengembalikan true jika string sama
bool strcmp(char *s1, char *s2) {
	int i = 0;
	while (s1[i] != '\0' && s2[i] != '\0') {
		if (s1[i] != s2[i]) {
			return false;
		}
		i++;
	}
	return true;
}

// Melakukan penyalinan null terminated string
void strcpy(char *dst, char *src) {
	int i = 0;
	while (src[i] != '\0') {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

// Mengosongkan byte array yang memiliki panjang n
void clear(byte *ptr, unsigned int n) {
	int i;
	for (i = 0; i < n; i++) {
		ptr[i] = 0;
	}
}

// concat string
void strcat(char *dst, char *src) {
	int i = 0;
	int j = 0;
	while (dst[i] != '\0') {
		i++;
	}
	while (src[j] != '\0') {
		dst[i] = src[j];
		i++;
		j++;
	}
	dst[i] = '\0';
}

// clear array
void clrarr(char *arr, int size) {
	int i;
	for (i = 0; i < size; i++) {
		arr[i] = '\0';
	}
}

// starts with
bool startsWith(char *str, char *prefix) {
	int i = 0;
	while (prefix[i] != '\0') {
		if (str[i] != prefix[i]) {
			return false;
		}
		i++;
	}
	return true;
}