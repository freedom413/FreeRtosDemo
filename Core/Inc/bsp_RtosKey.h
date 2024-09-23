#ifndef __bsp_rtoskey_h__
#define __bsp_rtoskey_h__

#define EN_multi_button 1

typedef enum
{
  KEYNONE,
  KEY0,
  KEY1,
  KEY2,
  KEYUP,
} KEYsta;

KEYsta GetKey(void);

#include "multi_button.h"
extern Button MKEY0;
extern Button MKEY1;
extern Button MKEY2;
extern Button MKEYUP;

#endif
void KeyInit(void);
