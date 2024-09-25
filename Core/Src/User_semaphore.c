#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_RtosKey.h"
#include "semphr.h"
#include "FIFO_lib.h"
#include "usart.h"

/****************************************Semphr_start************************************/

// 环形fifo
static Fifo_t U1;
#define maxNum (100)
//static
	uint8_t resbuff[maxNum];

// 二值信号量
SemaphoreHandle_t xDataReadySemaphore;

void UART1_RxCpltCallback(UART_HandleTypeDef *huart);
void vTaskB(void *pvParameters);

// 初始化接收
void SemphrInit(void)
{

  FifoInit(&U1, (uint8_t *)resbuff, maxNum);
  // 创建一个二值信号量，初值为 0，表示没有新数据
  xDataReadySemaphore = xSemaphoreCreateBinary();
	xSemaphoreTake(xDataReadySemaphore, 0) ;
  // 注册串口1接收回调函数
  HAL_UART_RegisterCallback(&huart1, HAL_UART_RX_COMPLETE_CB_ID, UART1_RxCpltCallback);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); // 开启串口非空中断
  xTaskCreate(vTaskB, "taskb", 128, NULL, 4, NULL);

  vTaskStartScheduler();

}

// 中断A
// 串口接收回调函数 此函数在串口非空中断时被hal库自动调用 在次任务中接收数据
void UART1_RxCpltCallback(UART_HandleTypeDef *huart)
{
  static uint16_t count = 0;
  count += FifoIn(&U1, (uint8_t *)(&huart->Instance->DR), 1); // 数据入队 并累加数量

  if (count >= maxNum / 2)
  {
    count = 0;
    BaseType_t pxHigherPriorityTaskWoken; // 有高优先级任务要调度标志
    xSemaphoreGiveFromISR(xDataReadySemaphore, &pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
  }
}

  uint8_t tmp[maxNum / 2];
// 任务B
void vTaskB(void *pvParameters)
{
  for (;;)
  {
    // 等待数据准备好的信号量
    if (xSemaphoreTake(xDataReadySemaphore, portMAX_DELAY) == pdTRUE)
    {
      // 处理数据
    
      FifoOut(&U1, tmp, maxNum / 2);
      for (uint16_t i = 0; i < maxNum / 2; i++)
      {
//        tmp[i]++;
        printf("%c ", tmp[i]); 

       if (i % 10 == 0)
       {
         printf("\r\n");
        
       }
      }
    }

  }
}

/****************************************Semphr_end************************************/
