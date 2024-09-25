#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_RtosKey.h"

/****************************************task_start*********************************** */

//**********************任务创建***********************
TaskHandle_t taskled1;
TaskHandle_t taskled2;

/*静态任务 start*/
StaticTask_t stask1ctrbuff; // 静态任务控制buff
TaskHandle_t stask1ctr;     // 静态任务控制句柄

#define stask1size 128              // 静态任务栈大小128 （word ->32bit == 4byte）
StackType_t stask1buff[stask1size]; // 静态任务buff

/*静态任务 end*/

void taskled1_app(void *p);  // 任务进入函数1
void taskled2_app(void *p);  // 任务进入函数2
void taskState_app(void *p); // 状态切换函数

// 传入参数 的生命周期要一直在任务运行期间保持
uint8_t a = 1;
uint8_t b = 2;

void taskInit(void)
{

#if 0 /*有句柄和无句柄动态创建任务 */
  // 动态创建任务，内存由freertos自动开辟释放
  xTaskCreate(
      taskled1_app, // 任务进入函数
      "led1",       // 任务名字
      128,          // 堆栈大小
      NULL,         // 传入参数，没有就传入NULL
      2,            // 任务优先级
      &taskled1     // 任务控制句柄
  );
  xTaskCreate(taskled2_app, "led2", 128, NULL, 3, NULL); // 无控制句柄也可运行
#endif

#if 0 /*静态创建，与传参 */
  // 静态创建任务，内存由用户手动开辟释放
  stask1ctr = xTaskCreateStatic(
      taskled1_app, // 任务进入函数
      "led2",       // 任务名字
      stask1size,   // 堆栈大小
      &a,           // 传入参数，没有就传入NULL
      3,            // 任务优先级
      stask1buff,   // 静态堆栈buff
      &stask1       // 静态任务控制buff
  );
  xTaskCreate(taskled2_app, "led1", 128, &b, 3, &taskled1);
#endif

#if 1 /*可以使用相同的进入函数 因为每个任务的堆栈是独立的 所以虽然调用的函数一样,但是传入的参数和运行空间都是独立的 */
	
  stask1ctr = xTaskCreateStatic(
      taskled1_app,  // 任务进入函数
      "led2",        // 任务名字
      stask1size,    // 堆栈大小
      &a,            // 传入参数，没有就传入NULL
      3,             // 任务优先级
      stask1buff,    // 静态堆栈buff
      &stask1ctrbuff // 静态任务控制buff
  );
  xTaskCreate(taskled1_app, "led1", 128, &b, 3, &taskled1);
  xTaskCreate(taskled2_app, "led22", 128, &b, 3, &taskled2);
  xTaskCreate(taskState_app, "raskstate", 128, NULL, 3, NULL);
#endif


  KeyInit();
  vTaskStartScheduler(); /*最重要一点 必循开启调度器才可以使任务运行*/
}

// 通过传入的参数来控制不同led翻转
void taskled1_app(void *p)
{
  uint16_t j = 0;
  while (1) // 每个运行函数必须有死循环
  {
    uint8_t *is = (uint8_t *)p; // 参数强制转换
    if (*is == 1)
    {
      HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
      vTaskDelay(1000);
      printf("led2:%d\r\n",j++);
    }
    else if (*is == 2)
    {
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
      vTaskDelay(1500);
      printf("led1:%d\r\n",j++);
    }

  }
}

void taskled2_app(void *p)
{
  (void)p; // 显式告诉编译器p没有使用
  while (1)
  {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    HAL_Delay(500); // 不具有任务阻塞功能，会在这里死等，而不进行任务切换
    // vTaskDelay(pdMS_TO_TICKS(1500) /*毫秒转滴答时间*/);
  }
}

//**********************任务状态&&优先级控制***********************
void taskState_app(void *p)
{
  (void)p;
  while (1)
  {

    if (/*GetKey() == KEY0*/ get_button_event(&MKEY0) == SINGLE_CLICK)
    {
      vTaskSuspend(taskled1); // 唤醒任务
      vTaskResume(stask1ctr); // 暂停任务
    }
    else if (/*GetKey() == KEY1*/  get_button_event(&MKEY1) == SINGLE_CLICK)
    {
      vTaskResume(taskled1);
      vTaskSuspend(stask1ctr);
    }
    else if (/*GetKey() == KEY2*/  get_button_event(&MKEY2) == SINGLE_CLICK)
    {
      vTaskDelete(taskled1);  // 只能删除动态创建任务后内存会被空闲任务释放
      vTaskDelete(stask1ctr); // 删除静态创建任务后内存不会被释放，容易造成内存泄漏
    }
    else if (/*GetKey() == KEYUP*/  get_button_event(&MKEYUP) == SINGLE_CLICK)
    {
      if (uxTaskPriorityGet(stask1ctr) == uxTaskPriorityGet(taskled2))
      {
        vTaskPrioritySet(taskled2,configMAX_PRIORITIES-2); //优先级设置到最高 并且延时非阻塞，所以其他任务无法执行。
      }
      else
      {
        vTaskPrioritySet(taskled2, 3);
      }
    }

     vTaskDelay(50);
  }
}

/****************************************task_end*********************************** */
