#ifndef __fifo_lib__
#define __fifo_lib__
#include "main.h"
//队列可用容量为最大长度减一 

typedef struct
{
    uint32_t head;
    uint32_t end;
    uint32_t Max_Size;
    uint8_t *data;
} Fifo_t;

void FifoInit(Fifo_t *f,uint8_t *data,uint32_t Max_Size);
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len);
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len);

#endif



