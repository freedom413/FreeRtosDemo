#include "bsp_RtosKey.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "multi_button.h"



#if (!EN_multi_button)/******key bsp start**********/

static KEYsta g_NowKey = KEYNONE;

void taskKEY_app(void *p)
{
  (void)p;
  while (1)
  {

    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
      vTaskDelay(pdMS_TO_TICKS(20));
      if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
        g_NowKey = KEY0;
    }
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
      vTaskDelay(pdMS_TO_TICKS(20));
      if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
        g_NowKey = KEY1;
    }
    if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
    {
      vTaskDelay(pdMS_TO_TICKS(20));
      if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
        g_NowKey = KEY2;
    }
    if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_SET)
    {
      vTaskDelay(pdMS_TO_TICKS(20));
      if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_SET)
        g_NowKey = KEYUP;
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

/*静态任务*/
StaticTask_t staskkey;             // 静态任务句柄
#define staskkeysize 256           // 静态任务栈大小 word ->32bit == 8byte
StackType_t keybuff[staskkeysize]; // 静态任务buff

void KeyInit(void)
{
  xTaskCreateStatic(
      taskKEY_app,  // 任务进入函数
      "key",        // 任务名字
      staskkeysize, // 堆栈大小
      NULL,         // 传入参数，没有就传入NULL
      6,            // 任务优先级
      keybuff,      // 静态堆栈buff
      &staskkey     // 静态任务控制句柄
  );
}

KEYsta GetKey(void)
{
  KEYsta temp = g_NowKey;
  g_NowKey = KEYNONE;
  return temp;
}

#else  /******key bsp end**********/

/************************multi_button bsp start**********************************/

uint8_t KeyLevlefun(uint8_t button_id_)
{
  GPIO_PinState sta = GPIO_PIN_SET;
  switch (button_id_)
  {
  case KEY0:
    sta = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
    break;
  case KEY1:
    sta = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
    break;
  case KEY2:
    sta = HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
    break;
  case KEYUP:
    sta = (GPIO_PinState)!HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
    break;

  default:
    break;
  }
  return (uint8_t)sta;
}

// PRESS_DOWN = 0,
// PRESS_UP,
// PRESS_REPEAT,
// SINGLE_CLICK,
// DOUBLE_CLICK,
// LONG_PRESS_START,
// LONG_PRESS_HOLD,
// number_of_event,
// NONE_PRESS

void KeyCb(void *p)
{
  struct Button *ev = p;

  switch (ev->button_id)
  {
  case 0:
    if (ev->event == SINGLE_CLICK)
    {
    }
    break;
  case 1:
    if (ev->event == SINGLE_CLICK)
    {
    }
    break;
  case 2:
    if (ev->event == SINGLE_CLICK)
    {
    }
    break;
  case 3:
    if (ev->event == SINGLE_CLICK)
    {
    }
    break;
  default:
    break;
  }
}

Button MKEY0;
Button MKEY1;
Button MKEY2;
Button MKEYUP;

static void multi_button_task(void *p)
{
  TickType_t t = xTaskGetTickCount();
  while (1)
  {
    button_ticks();
    vTaskDelayUntil(&t, pdMS_TO_TICKS(5));
  }
}

void KeyInit(void)
{
  button_init(&MKEY0, KeyLevlefun, 0, KEY0);
  button_init(&MKEY1, KeyLevlefun, 0, KEY1);
  button_init(&MKEY2, KeyLevlefun, 0, KEY2);
  button_init(&MKEYUP, KeyLevlefun, 0, KEYUP);

  button_attach(&MKEY0, SINGLE_CLICK, KeyCb);
  button_attach(&MKEY1, SINGLE_CLICK, KeyCb);
  button_attach(&MKEY2, SINGLE_CLICK, KeyCb);
  button_attach(&MKEYUP, SINGLE_CLICK, KeyCb);

  button_start(&MKEY0);
  button_start(&MKEY1);
  button_start(&MKEY2);
  button_start(&MKEYUP);

  xTaskCreate(multi_button_task, "multi_button", 128, NULL, configMAX_PRIORITIES - 3, NULL);
}

#endif /************************multi_button bsp end**********************************/
