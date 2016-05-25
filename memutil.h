#pragma once

typedef unsigned char byte;
typedef unsigned long size_t;

//memcpy that will reverse byte order if the machine is little endian
void ntoh_memcpy(void *dst, const void *src, size_t len);

//Read a short from a buffer based on endianess
short ntoh_read_s(const void *src);

//Read an int from a buffer based on endianess
int ntoh_read_d(const void *src);

//Read a short from a buffer based on endianess
unsigned short ntoh_read_us(const void *src);

//Read an int from a buffer based on endianess
unsigned int ntoh_read_ud(const void *src);

//Write a short to a buffer based on endianess
void ntoh_write_s(void *src, short value);

//Write an int to a buffer based on endianess
void ntoh_write_d(void *src, int value);

//Write a short to a buffer based on endianess
void ntoh_write_us(void *src, unsigned short value);

//Write an int to a buffer based on endianess
void ntoh_write_ud(void *src, unsigned int value);


void emit_err(const char* err);
