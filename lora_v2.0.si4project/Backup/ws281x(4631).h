#ifndef MYLIB_WS28XX
#define MYLIB_WS28XX

#include "tim.h"			
/****************************************
*Config
****************************************/
#define BIT_1               61            //1码比较值为61-->850us
#define BIT_0               28            //0码比较值为28-->400us

#define PIXEL_SIZE			    5             //灯的数量
#define WS28xx_PWM_hTIMER	  htim2         //定时�?
#define WS28xx_PWM_Chaneel  TIM_CHANNEL_1	//通道1	


#define  LIGHT_DUTY         10	//1~100
#define  DISPLAY_COLOR      0xE46084

//整个WS28xx_DataTypeStruct结构体将被以PWM方式发�?
typedef struct
{						
	unsigned char ColorStartData[3];           //3�?等待PWM稳定			
	unsigned char ColorRealData[24*PIXEL_SIZE];//实际GRB数据(已经转换为PWM对应的�?
	unsigned char ColorEndData;             	 //结束位为0
}WS28xx_DataTypeStruct;

/*--------------------------------------------------------------*/
#define QUEUE_SIZE		10
enum
{
	DIS_CMD_SD_DET_IN=0,
	DIS_CMD_SD_DET_OUT,
	DIS_CMD_SELECT_UART1,
	DIS_CMD_SELECT_UART2,
	DIS_CMD_SELECT_UART3,
	DIS_CMD_IDLE
};
typedef struct
{
	uint16_t cmd[QUEUE_SIZE];
	uint8_t front;
	uint8_t rear;
	uint8_t lock;
}dis_schedule_queue;
extern dis_schedule_queue DisCmdQueue;

/****************************************
*对象化编�?
****************************************/
typedef struct 
{
	//实际发送的数据
	WS28xx_DataTypeStruct WS28xx_Data;
	//灯数�?
	unsigned short int Pixel_size;
	
    //单独设置index的RGB颜色
    void (*SetPixelColor_RGB)(unsigned short int index,unsigned char r,unsigned char g,unsigned char b, unsigned char duty);
    //从RGB数据读出：设置index的RGB颜色
    void (*SetPixelColor_From_RGB_Buffer)( unsigned short int pixelIndex,unsigned char pRGB_Buffer[][3],unsigned short int DataCount);
    //设置所有为RGB颜色
    void (*SetALLColor_RGB)(unsigned char r,unsigned char g,unsigned char b, unsigned char duty);
    //获取某个位置的RGB
    void (*GetPixelColor_RGB)(unsigned short int index,unsigned char *r,unsigned char *g,unsigned char *b);
    //显示（发出数据）
    void (*show)(void);
}WS28xx_TypeStruct;

extern WS28xx_TypeStruct WS28xx;

void   WS28xx_TypeStructInit(void);



uint8_t Dis_Cmd_InQueue(dis_schedule_queue *cmd_queue,	uint16_t cmd);
uint16_t Dis_Cmd_OutQueue(dis_schedule_queue *cmd_queue);
void WS28xx_DisplayTask(void);

#endif


