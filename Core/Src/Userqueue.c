#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_RtosKey.h"
#include "queue.h"

/****************************************queue_start*********************************** */

// 静态队列创建
#define QUEUE_LENGTH 5
#define ITEM_SIZE sizeof(uint32_t)

// xQueueBuffer用来保存队列结构体
StaticQueue_t xQueueBuffer;

// ucQueueStorage 用来保存队列的数据
uint8_t ucQueueStorage[QUEUE_LENGTH * ITEM_SIZE]; // 大小为：队列长度 * 数据大小

// xQueue1 用来控制队列
QueueHandle_t xQueue1;

// 动态队列创建
QueueHandle_t xQueue2;

QueueSetHandle_t XqueueSet1;

void queueSendfun(void *p);
void queueResfun(void *p);

void queueInit(void)
{

  // 创建队列: 可以容纳QUEUE_LENGTH个数据，每个数据大小是ITEM_SIZE
  xQueue1 = xQueueCreateStatic(QUEUE_LENGTH,
                               ITEM_SIZE,
                               ucQueueStorage,
                               &xQueueBuffer);

  xQueue2 = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);

  xTaskCreate(queueSendfun, "q1", 128, NULL, 3, NULL);
  xTaskCreate(queueResfun, "q2", 128, NULL, 3, NULL);

  // 创建队列集
  XqueueSet1 = xQueueCreateSet(QUEUE_LENGTH * 2);
  // 队列加入队列集
  xQueueAddToSet(xQueue1, XqueueSet1);
  xQueueAddToSet(xQueue2, XqueueSet1);

  KeyInit();

  vTaskStartScheduler();
}

void queueSendfun(void *p)
{
  uint16_t i = 0;
  uint16_t j = 0;
  static PressEvent btn1_event_val = NONE_PRESS;
  static PressEvent btn2_event_val = NONE_PRESS;
  BaseType_t re = pdFALSE;
  while (1)
  {
    //判断按键按下 
    if (btn1_event_val != get_button_event(&MKEY1))
    {
      btn1_event_val = get_button_event(&MKEY1);
      if (btn1_event_val == PRESS_DOWN)
      {
        re = xQueueSend(xQueue1, &i, 0); //发送数据到队列
        printf("q1send:%d,pd:%d\r\n", i, re);
        i++;
      }
    }

    if (btn2_event_val != get_button_event(&MKEY2))
    {
      btn2_event_val = get_button_event(&MKEY2);
      if (btn2_event_val == PRESS_DOWN)
      {
        re = xQueueSend(xQueue2, &j, 0);
        printf("q2send:%d,pd:%d\r\n", j, re);
        j++;
      }
    }
    //    vTaskDelay(1);
  }
}

void queueResfun(void *p)
{
  uint32_t res;
  BaseType_t re = pdFALSE;
  QueueSetMemberHandle_t quet = NULL;
  while (1)
  {
    quet = xQueueSelectFromSet(XqueueSet1, portMAX_DELAY); //从队列集里面读取数据 没有数据就阻塞

    if (quet == xQueue1)
    {
      /* code */
      re = xQueueReceive(xQueue1, &res, 0);//从队列里面读取数据 一定有数据不用等待
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
      printf("q1res:%d,pd:%d,avlb:%d\r\n", res, re ,/*获取队列剩余可用空间*/uxQueueSpacesAvailable(xQueue1));
      vTaskDelay(1000);
    }
    else if (quet == xQueue2)
    {
      /* code */
      re = xQueueReceive(xQueue2, &res, 0);
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
      printf("q2res:%d,pd:%d,avlb:%d\r\n", res, re,uxQueueSpacesAvailable(xQueue1));
      vTaskDelay(1000);
    }
    else
    {
      printf("fialed\r\n");
    }
  }
}

/****************************************queue_end*********************************** */
