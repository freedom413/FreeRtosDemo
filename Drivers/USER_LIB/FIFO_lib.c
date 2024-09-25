#include "FIFO_lib.h"




static uint8_t isEmpty(Fifo_t *f)
{
    return (f->end == f->head);
}

static uint8_t isFull(Fifo_t *f)
{
    return ((f->end + 1) % f->Max_Size == f->head);
}


/**
 * @brief 环形队列初始化
 * @param Fifo_t *f 待初始化队列结构体指针
 */
void FifoInit(Fifo_t *f,uint8_t *data,uint32_t Max_Size)
{
    f->end = 0;
    f->head = 0;
    f->data = data;
    f->Max_Size = Max_Size;
}

/**
 * @brief 入队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *in 入队数据指针
 * @param uint32_t len 入队数据长度
 * @retval 0 表示队列满了存不下了入队失败 其它数字表示成功入队的数据个数
 */
uint32_t FifoIn(Fifo_t *f, uint8_t *in, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isFull(f))
        {
            return 0;
        }
        f->end = (f->end + 1) % f->Max_Size;
        f->data[f->end] = in[i];
    }

    return i;
}


/**
 * @brief 出队函数
 * @param Fifo_t *f 队列结构体指针
 * @param uint8_t *out 出队数据指针
 * @param uint32_t len 出队数据长度
 * @retval 0 表示队列空了没数据量出队失败 其它数字表示成功出队的数据个数
 */
uint8_t FifoOut(Fifo_t *f, uint8_t *out, uint32_t len)
{
    uint32_t i;
    for (i = 0; i < len; i++)
    {
        if (isEmpty(f))
        {
            return 0;
        }
        f->head = (f->head + 1) % f->Max_Size;
        out[i] = f->data[f->head];
    }
    return i;
}


#if 0

static Fifo_t g_fifo;

int fifo_test(void)
{
    FifoInit(&g_fifo);

    uint8_t in[10] = {1,2,3,4,5,6,7,8,9,10};
    uint8_t out[10] = {0};
    
    //入队 3个数据 1 2 3
    FifoIn(&g_fifo,in,3);

    //出队 2个数据 1 2
    FifoOut(&g_fifo,out,2);

    //入队 4 5 6 7 8 此时队列里右6个数据
    FifoIn(&g_fifo,&in[3],5);

    //出队队2个数据 应该是 3 4 此时队列里有4 个数据
    FifoOut(&g_fifo,out,2);

    //入队6 个数据 1 2 3 4 5 6   此时队列里有 10 个数据 
    //但是最后一个数据会 入队失败一次  因为队列的最大容量为f->Max_Size -1 == 9。 
    FifoIn(&g_fifo,&in[0],6);


    //出队8 个数据应该是  5 6 7 8 1 2 3 4 此时队列里有 1 个数据 
    FifoOut(&g_fifo,out,8);

    //出队2 个数据应该是 5 然后队列空 出队失败一次
    FifoOut(&g_fifo,out,8);

    return 0;
}

#endif

