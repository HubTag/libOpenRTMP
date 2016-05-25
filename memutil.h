
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
