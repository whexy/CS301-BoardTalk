/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "24l01.h"
#include "screen.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rxBuffer[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    u8 key, mode;
    u16 t = 0;
    u8 tmp_buf[33];

    HAL_Init();                            //初始化HAL库
    Stm32_Clock_Init(RCC_PLL_MUL9);    //设置时钟,72M
    delay_init(72);                    //初始化延时函数
    MX_USART1_UART_Init();
    //	uart_init(115200);					//初始化串口
    LED_Init();                            //初始化LED
    KEY_Init();                            //初始化按键
//    LCD_Init();                            //初始化LCD
    screen_init();
    NRF24L01_Init();                    //初始化NRF24L01

    screen_write_lalign("NRF24L01 Test", RED);
    screen_update();
    while (NRF24L01_Check()) {
        screen_write_ralign("Checking NRF24L01...", RED);
        screen_update();
        delay_ms(400);
    }
    screen_write_ralign("NRF24L01 OK", GREEN);
    screen_write_ralign("KEY0:RX_Mode  KEY1:TX_Mode", GREEN);
    screen_update();
    while (1) {
        key = KEY_Scan(0);
        if (key == KEY0_PRES) {
            mode = 0;
            break;
        } else if (key == KEY1_PRES) {
            mode = 1;
            break;
        }
        t++;
//        if (t == 100)LCD_ShowString(10, 150, 230, 16, 16, "KEY0:RX_Mode  KEY1:TX_Mode"); //闪烁显示提示信息
//        if (t == 200) {
//            LCD_Fill(10, 150, 230, 150 + 16, WHITE);
//            t = 0;
//        }
        delay_ms(5);
    }
//    LCD_Fill(10, 150, 240, 166, WHITE);//清空上面的显示
//    POINT_COLOR = BLUE;//设置字体为蓝色
    if (mode == 0)//RX模式
    {
//        LCD_ShowString(30, 150, 200, 16, 16, "NRF24L01 RX_Mode");
//        LCD_ShowString(30, 170, 200, 16, 16, "Received DATA:");
        screen_write_ralign("NRF24L01 RX_Mode", BLUE);
        screen_update();
        NRF24L01_RX_Mode();
        while (1) {
            if (NRF24L01_RxPacket(tmp_buf) == 0)//一旦接收到信息,则显示出来.
            {
                tmp_buf[32] = 0;//加入字符串结束符
                screen_write_ralign(tmp_buf, BLUE);
                screen_update();
//                LCD_ShowString(0, 190, lcddev.width - 1, 32, 16, tmp_buf);
            } else delay_us(100);
            t++;
            if (t == 10000)//大约1s钟改变一次状态
            {
                t = 0;
                LED0 = !LED0;
            }
        }
    } else//TX模式
    {
//        LCD_ShowString(30, 150, 200, 16, 16, "NRF24L01 TX_Mode");
        screen_write_ralign("NRF24L01 TX_Mode", BLUE);
        screen_update();
        NRF24L01_TX_Mode();
        HAL_UART_Receive_IT(&huart1, (uint8_t *) rxBuffer, 1);
        while (1) {
            LED0 = !LED0;
            delay_ms(1500);
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        static unsigned char uRx_Data[1024] = {0};
        static unsigned char uLength = 0;
        uRx_Data[uLength++] = rxBuffer[0];
        if (rxBuffer[0] == '\n') {
            HAL_UART_Transmit(&huart1, uRx_Data, uLength, 0xffff);
            uRx_Data[uLength] = '\0';
            if (NRF24L01_TxPacket(uRx_Data) == TX_OK) {
                screen_write_ralign((const char *) uRx_Data, BLUE);
//                LCD_ShowString(30, 170, 239, 32, 16, "Sent DATA:");
//                LCD_ShowString(0, 190, lcddev.width - 1, 32, 16, uRx_Data);
            } else {
                screen_write_ralign("Send failed", RED);
//                LCD_Fill(0, 170, lcddev.width, 170 + 16 * 3, WHITE);//清空显示
//                LCD_ShowString(30, 170, lcddev.width - 1, 32, 16, "Send Failed ");
            }
            screen_update();
            uLength = 0;
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
