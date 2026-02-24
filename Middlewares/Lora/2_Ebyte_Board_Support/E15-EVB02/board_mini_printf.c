#include <stdarg.h>
#include "usart.h"
#include "board_mini_printf.h"

/* !
 * @brief 目标硬件串口通信接口
 * 
 * @param data 写入的数据 1 Byte
 */
//static void send_uart_data(uint8_t data)
//{
////  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
////  USART_SendData8(USART1,data);
//    HAL_UART_Transmit(&huart1, &data, 1, 0xffff);
//}
 
/*
  功能：将int型数据转为2，8，10，16进制字符串
  参数：value --- 输入的int整型数
        str --- 存储转换的字符串
        radix --- 进制类型选择
  注意：8位单片机int字节只占2个字节
*/
static char *sky_itoa(int value, char *str, unsigned int radix)
{
  char list[] = "0123456789ABCDEF";
  unsigned int tmp_value;
  int i = 0, j, k = 0;
//  if (NULL == str) {
  if (0 == str) {
//    return NULL;
    return 0;
  }
  if (2 != radix && 8 != radix && 10 != radix && 16 != radix) {
//    return NULL;
    return 0;
  }
  if (radix == 10 && value < 0) {
    //十进制且为负数
    tmp_value = (unsigned int)(0 - value);
    str[i++] = '-';
    k = 1;
  } else {
    tmp_value = (unsigned int)value;
  }
  //数据转换为字符串，逆序存储
  do {
    str[i ++] = list[tmp_value%radix];
    tmp_value /= radix;
  } while(tmp_value);
  str[i] = '\0';
  //将逆序字符串转换为正序
  char tmp;
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  return str;
}
 
/*
  功能：将double型数据转为字符串
  参数：value --- 输入的double浮点数
        str --- 存储转换的字符串
        eps --- 保留小数位选择，至少保留一个小数位,至多保留4个小数位
  注意：8位单片机int字节只占2个字节
*/
static void sky_ftoa(double value, char *str, unsigned int eps)
{
  unsigned int integer;
  double decimal;
  char list[] = "0123456789";
  int i = 0, j, k = 0;
  //将整数及小数部分提取出来
  if (value < 0) {
    decimal = (double)(((int)value) - value);
    integer = (unsigned int)(0 - value);
    str[i ++] = '-';
    k = 1;
  } else {
    integer = (unsigned int)(value);
    decimal = (double)(value - integer);
  }
  //整数部分数据转换为字符串，逆序存储
  do {
    str[i ++] = list[integer%10];
    integer /= 10;
  } while(integer);
  str[i] = '\0';
  //将逆序字符串转换为正序
  char tmp;
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  //处理小数部分
  if (eps < 1 || eps > 4) {
    eps = 4;
  }
  
  //精度问题，防止输入1.2输出1.19等情况
  double pp = 0.1;
  for (j = 0; j <= eps; j++) {
    pp *= 0.1;
  }
  decimal += pp;
  while (eps) {
    decimal *= 10;
    eps --;
  }
  int tmp_decimal = (int)decimal;
  str[i ++] = '.';
  k = i;
  //整数部分数据转换为字符串，逆序存储
  do {
    str[i ++] = list[tmp_decimal%10];
    tmp_decimal /= 10;
  } while(tmp_decimal);
  str[i] = '\0';
  //将逆序字符串转换为正序
  for (j = k; j < (i+k)/2; j++) {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  str[i] = '\0';
}
 
 
void mprintf(char * Data, ...)
{
//	unsigned char temp=0;
//  while ( * Data != '\0' ) 
//  {
//  	user_param_uart[0].rx_buff[temp ++] = *Data;
//	Data ++;
//  }
//  user_param_uart[0].tx_len = temp;
//  HAL_UART_Transmit_DMA(&huart1, user_param_uart[temp].rx_buff, user_param_uart[temp].tx_len);
}
