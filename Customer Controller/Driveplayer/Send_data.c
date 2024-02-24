#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "driveas5600.h"
#include "CRC.h"
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "NRF24L01.h"

extern float angle;
float angle1;
uint8_t header[39]={0};
uint8_t Serial_RxPacket[6]={0};
uint8_t Serial_RxFlag;
uint8_t data1[4]={0};

uint8_t tx_buffer[100];
uint8_t rx_buffer[100];

float angle3;
float angle4;
uint8_t TxData[32]={0,0,0,0,0,0,0,0};
float debug;
float debug2;
float data_to_c[4];
static void Data_Concatenation(void);


void Send_Task(void const * argument)
{						
  /* USER CODE BEGIN Send_Task */
  /* Infinite loop */
  //uint32_t wait_time = xTaskGetTickCount();
  HAL_UART_Receive_IT(&huart3,rx_buffer,20);
	Init_NRF24L01();
  if(NRF24L01_Check())
  {
    debug=7;
  }
  TX_Mode();
  for(;;)
  { 

    angle=Programe_Run(hi2c1);
    angle1=Programe_Run(hi2c2);
    //Data_Concatenation();
    tx_buffer[0]=0xab;
    for(int i=0;i<4;i++)
    {
      tx_buffer[1+i]=*((char*)(&angle)+i);
    }
    for(int i=0;i<4;i++)
    {
      tx_buffer[5+i]=*((char*)(&angle1)+i);
    }
    HAL_UART_Transmit(&huart2,tx_buffer, 10, 10);



if(rx_buffer[0]==0xab){
		for(int i=0;i<4;i++)
    {
      *((char*)(&angle3)+i)=rx_buffer[i+1];
    }
    for(int i=0;i<4;i++)
    {
      *((char*)(&angle4)+i)=rx_buffer[i+5];
    }
}

data_to_c[0]=angle3-260.0f;
data_to_c[1]=angle4-40.7f;
data_to_c[2]=angle-52.9f;
data_to_c[3]=angle1-113.0f;

if(data_to_c[0]>180)
{
  data_to_c[0]-=360;
}
if(data_to_c[1]>180)
{
  data_to_c[1]-=360;
}
if(data_to_c[2]<-180)
{
  data_to_c[2]+=360;
}
if(data_to_c[3]>180)
{
  data_to_c[3]-=360;
}
TxData[0]=0xab;
for(int i=0;i<4;i++)
{
  for(int j=0;j<4;j++)
  {
    TxData[i*4+1+j]=*((char*)(&(data_to_c[i]))+j);
  }
}
TxData[17]=0xcd;
		if(NRF24L01_TxPacket(TxData)==TX_OK)
    {
      debug2=666;
    }
	
    osDelay(5);
  }

  /* USER CODE END Send_Task */
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART3)
  {
    
    }
    HAL_UART_Receive_IT(&huart3,rx_buffer,20);
     // HAL_UART_Receive_IT(&huart3,(uint8_t *)Serial_RxPacket,6);
 }
static void Data_Concatenation(void)
{
   static uint8_t seq = 0;
   header[0]=0xA5;
   header[1]=0x1E;
   header[3]=seq++;
   Append_CRC8_Check_Sum((uint8_t *)(&header),5);
   header[5]=0x02;
   header[6]=0x03;
   header[7]=(uint16_t)angle>>8;
   header[8]=(uint16_t)angle;
   header[9]=(uint16_t)angle1>>8;
   header[10]=(uint16_t)angle1;
   header[11]=data1[0];
   header[12]=data1[1];
   header[13]=data1[2];
   header[14]=data1[3];
   Append_CRC16_Check_Sum((uint8_t *)(&header),39);
}