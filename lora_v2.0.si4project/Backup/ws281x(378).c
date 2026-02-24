#include "ws281x.h"

WS28xx_TypeStruct WS28xx;
dis_schedule_queue	DisCmdQueue;


static float min(float a, float b, float c)
{
  float m;
  
  m = a < b ? a : b;
  return (m < c ? m : c); 
}

static float max(float a, float b, float c)
{
  float m;
  
  m = a > b ? a : b;
  return (m > c ? m : c); 
}
  
void __rgb2hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v)
{
  float red, green ,blue;
  float cmax, cmin, delta;
  
  red = (float)r / 255;
  green = (float)g / 255;
  blue = (float)b / 255;
  
  cmax = max(red, green, blue);
  cmin = min(red, green, blue);
  delta = cmax - cmin;

  /* H */
  if(delta == 0)
  {
    *h = 0;
  }
  else
  {
    if(cmax == red)
    {
      if(green >= blue)
      {
        *h = 60 * ((green - blue) / delta);
      }
      else
      {
        *h = 60 * ((green - blue) / delta) + 360;
      }
    }
    else if(cmax == green)
    {
      *h = 60 * ((blue - red) / delta + 2);
    }
    else if(cmax == blue) 
    {
      *h = 60 * ((red - green) / delta + 4);
    }
  }
  
  /* S */
  if(cmax == 0)
  {
    *s = 0;
  }
  else
  {
    *s = delta / cmax;
  }
  
  /* V */
  *v = cmax;
}
  
void __hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    int hi = ((int)h / 60) % 6;
    float f = h * 1.0 / 60 - hi;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1- (1 - f) * s);
    switch (hi){
        case 0:
            *r = 255 * v;
            *g = 255 * t;
            *b = 255 * p;
            break;
        case 1:
            *r = 255 * q;
            *g = 255 * v;
            *b = 255 * p;
            break;
        case 2:
            *r = 255 * p;
            *g = 255 * v;
            *b = 255 * t;
            break;
        case 3:
            *r = 255 * p;
            *g = 255 * q;
            *b = 255 * v;
            break;
        case 4:
            *r = 255 * t;
            *g = 255 * p;
            *b = 255 * v;
            break;
        case 5:
            *r = 255 * v;
            *g = 255 * p;
            *b = 255 * q;
            break;
    }
}


void __show()
{
	HAL_TIM_PWM_Start_DMA(&WS28xx_PWM_hTIMER, WS28xx_PWM_Chaneel, (uint32_t *)(&WS28xx.WS28xx_Data), sizeof(WS28xx.WS28xx_Data));
}
	
//设置index的颜�?
void __SetPixelColor_RGB(unsigned short int index,unsigned char r,unsigned char g,unsigned char b, unsigned char duty)
{
	unsigned char j;
    float h, s, v;
	if(index > WS28xx.Pixel_size)
		return;

    __rgb2hsv(r, g, b, &h, &s, &v);
    v = v*duty/100;
    __hsv2rgb(h, s, v, &r, &g, &b);
    
	for(j = 0; j < 8; j++)
	{		
		WS28xx.WS28xx_Data.ColorRealData [24 * index + j]        = (g & (0x80 >> j)) ? BIT_1 : BIT_0;  //G 将高位先�?
		WS28xx.WS28xx_Data.ColorRealData [24 * index + j + 8]    = (r & (0x80 >> j)) ? BIT_1 : BIT_0;  //R将高位先�?
		WS28xx.WS28xx_Data.ColorRealData [24 * index + j + 16]   = (b & (0x80 >> j)) ? BIT_1 : BIT_0;  //B将高位先�?
	}
}
//获取某个位置的RGB
void __GetPixelColor_RGB(unsigned short int index,unsigned char *r,unsigned char *g,unsigned char *b)
{
	unsigned char j;
	*r=0;
	*g=0;
	*b=0;
	if(index > WS28xx.Pixel_size)
		return;
	for(j = 0; j <8; j++)
	{
		(*g)|=((WS28xx.WS28xx_Data.ColorRealData [24 * index + j]     >=BIT_1)? 0x80>>j:0); 	//G
		(*r)|=((WS28xx.WS28xx_Data.ColorRealData [24 * index + j + 8] >=BIT_1)? 0x80>>j:0);  	//R
		(*b)|=((WS28xx.WS28xx_Data.ColorRealData [24 * index + j + 16]>=BIT_1)? 0x80>>j:0);   //B
	}
}

void __SetPixelColor_From_RGB_Buffer( unsigned short int pixelIndex,unsigned char pRGB_Buffer[][3],unsigned short int DataCount)
{
	unsigned short int Index,j=0;
	for(Index=pixelIndex;Index < WS28xx.Pixel_size; Index++)
	{
			WS28xx.SetPixelColor_RGB(Index, pRGB_Buffer[j][0], pRGB_Buffer[j][1], pRGB_Buffer[j][2], 100);
			j++;
			if(j>DataCount)
				return;
	}
}

//设置所有颜�?
void __SetALLColor_RGB(unsigned char r,unsigned char g,unsigned char b, unsigned char duty)
{
	unsigned short int Index;

	for(Index=0;Index < WS28xx.Pixel_size; Index++)
	{
		WS28xx.SetPixelColor_RGB(Index,r,g,b,duty);
	}
}






void Dis_Cmd_InitQueue(dis_schedule_queue *cmd_queue)
{
	uint8_t temp=0;

	for(temp=0; temp<QUEUE_SIZE; temp++)
	{
		cmd_queue->cmd[temp] = DIS_CMD_IDLE;
	}
	cmd_queue->rear = 0;
	cmd_queue->front = 0;
	cmd_queue->lock = 0;
}


uint8_t Dis_Cmd_InQueue(dis_schedule_queue *cmd_queue,	uint16_t cmd)
{
//	uint8_t temp=0;
	if(((cmd_queue->rear+1)%QUEUE_SIZE == cmd_queue->front) || (cmd_queue->lock == 1))
	{
		return 0;
	}
	cmd_queue->lock = 1;
//	for(temp=(cmd_queue->rear+1)%QUEUE_SIZE; temp!=cmd_queue->front; (temp ++)%QUEUE_SIZE)
//	{
//		if(cmd == cmd_queue->cmd[temp])
//		{
//			cmd_queue->lock = clear;
//			return 1;
//		}
//	}	
	cmd_queue->rear = (cmd_queue->rear+1)%QUEUE_SIZE;
	cmd_queue->cmd[cmd_queue->rear] = cmd;
	cmd_queue->lock = 0;
	return 1;
}


uint16_t Dis_Cmd_OutQueue(dis_schedule_queue *cmd_queue)
{
	uint16_t ret_cmd=DIS_CMD_IDLE;
	if((cmd_queue->front == cmd_queue->rear)||(cmd_queue->lock == 1))	
	{
		return ret_cmd;
	}
	cmd_queue->lock = 1;
	cmd_queue->front = (cmd_queue->front+1)%QUEUE_SIZE;
	ret_cmd = cmd_queue->cmd[cmd_queue->front];
	cmd_queue->lock = 0;
	return ret_cmd;
}


void	WS28xx_TypeStructInit()
{
	WS28xx.Pixel_size=PIXEL_SIZE;
	WS28xx.GetPixelColor_RGB=__GetPixelColor_RGB;
	WS28xx.SetPixelColor_From_RGB_Buffer=__SetPixelColor_From_RGB_Buffer;
	WS28xx.SetPixelColor_RGB=__SetPixelColor_RGB;
	WS28xx.SetALLColor_RGB=__SetALLColor_RGB;
	WS28xx.show=__show;
	Dis_Cmd_InitQueue(&DisCmdQueue);
}


void WS28xx_DisplayTask(void)
{
	static uint32_t tickstart = 0;
	uint16_t dis_cmd=0;

	if ((HAL_GetTick() - tickstart) > 100)
	{
		tickstart = HAL_GetTick();

		dis_cmd = Dis_Cmd_OutQueue(&DisCmdQueue);
		WS28xx.SetALLColor_RGB(0, 0, 0, 0); //close all
		switch(dis_cmd)
		{
			case DIS_CMD_SD_DET_IN:
				WS28xx.SetALLColor_RGB(0, 0xff, 0, LIGHT_DUTY); 
			break;
			case DIS_CMD_SD_DET_OUT:
				WS28xx.SetALLColor_RGB(0xff, 0, 0, LIGHT_DUTY); 
			break;

			case DIS_CMD_SELECT_UART1:
				WS28xx.SetPixelColor_RGB(0, (uint8_t)(DISPLAY_COLOR>>16), (uint8_t)(DISPLAY_COLOR>>8), (uint8_t)DISPLAY_COLOR, LIGHT_DUTY); 
			break;
			case DIS_CMD_SELECT_UART2:
				WS28xx.SetPixelColor_RGB(2, (uint8_t)(DISPLAY_COLOR>>16), (uint8_t)(DISPLAY_COLOR>>8), (uint8_t)DISPLAY_COLOR, LIGHT_DUTY); 
			break;
			case DIS_CMD_SELECT_UART3:
				WS28xx.SetPixelColor_RGB(4, (uint8_t)(DISPLAY_COLOR>>16), (uint8_t)(DISPLAY_COLOR>>8), (uint8_t)DISPLAY_COLOR, LIGHT_DUTY); 
			break;
			
//			case DIS_CMD_IDLE:
			default:
//				WS28xx.SetALLColor_RGB(0, 0, 0xcc, LIGHT_DUTY); 
			break;
		}
		WS28xx.show(); 
	}
}



