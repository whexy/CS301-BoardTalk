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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "screen.h"
#include "msg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define __DEBUG
//#define __RX_DEBUG
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define HEARTBEAT_TIMEOUT 1000
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
uint8_t conn_status = CONN_OFF, is_stable = 1;
char rx_buf[RX_PLOAD_WIDTH + 1];
/* USER CODE END 0 */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */
    int rx_status;
    uint32_t prev_tick, cur_tick;
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_TIM3_Init();
    /* USER CODE BEGIN 2 */
    screen_init();
    conn_init();
    screen_clear();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
#ifdef __DEBUG
#ifdef __RX_DEBUG
    int k = 0;
    while (1) {
        if (SEND_MSG("Hello world") == PKG_TX_OK) {
            screen_write_ralign("SENT", (k % 3 == 0) ? BLACK : ((k % 3 == 1) ? RED : BLUE));
        } else {
            screen_write_ralign("SEND FAILED", (k % 3 == 0) ? BLACK : ((k % 3 == 1) ? RED : BLUE));
        }
        screen_update();
        ++k;
        HAL_Delay(200);
    }
#else
    int k = 0;
    while (1) {
        if (pkg_recv(rx_buf) != EMPTY) {
            screen_write_ralign(rx_buf, (k % 3 == 0) ? BLACK : ((k % 3 == 1) ? RED : BLUE));
            screen_update();
        }
        ++k;
        HAL_Delay(200);
    }
#endif // __RX_DEBUG
#else
    screen_write_lalign("NRF24L01 OK", BLUE);
    screen_write_ralign("KEY0:Connect  KEY1:Disconnect", GREEN);
    screen_update();
    HAL_UART_Receive_IT(&huart1, rxBuffer, 1);
    prev_tick = HAL_GetTick();
    while (1) {
        rx_status = pkg_recv(rx_buf);
        if (conn_status == CONN_ON) {
            switch (rx_status) {
                case MSG_TYPE:
                    screen_write_lalign(rx_buf, BLACK);
                    screen_update();
                    is_stable = 1;
                    break;
                case FIN_TYPE:
                    if (SEND_FIN() == PKG_TX_OK) {
                        conn_status = CONN_OFF;
                        screen_write_ralign("Disconnected", RED);
                        screen_update();
                    }
                    break;
                case ERR_TYPE:
                    is_stable = 0;
                    break;
                default:
                    break;
            }
        } else if (conn_status == CONN_OFF && rx_status == SYN_TYPE) {
            while (SEND_SYN() != PKG_TX_OK) {
                screen_write_lalign("I can't SYN back!!!", RED);
                screen_update();
                HAL_Delay(100);
            }
            conn_status = CONN_ON;
            screen_write_ralign("connected", RED);
            screen_update();
        }
        cur_tick = HAL_GetTick();
        if (cur_tick - prev_tick >= HEARTBEAT_TIMEOUT) {
            if (conn_status == CONN_ON) {
                screen_write_lalign("SEND BEAT", BLUE);
                screen_update();
                if (SEND_BEAT() == PKG_TX_OK) {
                    screen_write_lalign("BEAT OK", MAGENTA);
                    screen_update();
                    is_stable = 1;
                } else {
                    screen_write_lalign("BEAT ERR", RED);
                    screen_update();
                    is_stable = 0;
                }
            }
            prev_tick = cur_tick;
        }
        HAL_Delay(100);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
#endif // __DEBUG
    /* USER CODE END 3 */
}
#pragma clang diagnostic pop

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        static unsigned char uRx_Data[1024] = {0};
        static unsigned char uLength = 0;
        uRx_Data[uLength++] = rxBuffer[0];
        HAL_UART_Transmit(&huart1, rxBuffer, 1, 0xffff);
        if (rxBuffer[0] == '\n') {
            if (uRx_Data[uLength - 1] == '\r') {
                uRx_Data[uLength - 1] = '\0';
            } else {
                uRx_Data[uLength] = '\0';
            }
            if (SEND_MSG((char *) uRx_Data) == PKG_TX_OK) {
                screen_write_ralign((const char *) uRx_Data, BLUE);
            } else {
                screen_write_ralign("Send failed", RED);
            }
            screen_update();
            uLength = 0;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    int i;
//    HAL_Delay(400);
    switch (GPIO_Pin) {
        case GPIO_PIN_5:
            if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET) {
                screen_write_lalign("KEY0", BLUE);
                screen_update();
                if (conn_status == CONN_OFF) {
                    screen_write_lalign("CONN: OFF->PENDING", BLUE);
                    screen_update();
                    conn_status = CONN_PENDING;
                    for (i = 10; i > 0; i--) {
                        if (conn_create(NULL, NULL) == CONN_ON) {
                            break;
                        }
                    }
                    if (i == 0) {
                        conn_status = CONN_OFF;
                        screen_write_lalign("CONN: PENDING->OFF", BLUE);
                        screen_update();
                    } else {
                        conn_status = CONN_ON;
                        screen_write_ralign("connected", RED);
                        screen_update();
                    }
                }
            }
            break;
        case GPIO_PIN_15:
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) {
                screen_write_lalign("KEY1", BLUE);
                screen_update();
                if (conn_status == CONN_ON) {
                    screen_write_lalign("CONN: OFF->PENDING", BLUE);
                    screen_update();
                    conn_status = CONN_AWAIT;
                    for (i = 10; i > 0; --i) {
                        if (conn_close() == CONN_OFF) {
                            break;
                        }
                    }
                    if (i == 0) {
                        screen_write_lalign("CONN: FORCE OFF", RED);
                        screen_update();
                        // Report ERROR?
                    }
                    // Force close?
                    conn_status = CONN_OFF;
                    screen_write_ralign("Disconnected", RED);
                    screen_update();
                }
            }
            break;
        default:
            break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    screen_write_lalign("TIM", BLUE);
    screen_update();
    if (htim->Instance == TIM3) {
        if (conn_status == CONN_ON) {
            screen_write_lalign("SEND BEAT", BLUE);
            screen_update();
            if (SEND_BEAT() == PKG_TX_OK) {
                screen_write_lalign("BEAT OK", MAGENTA);
                screen_update();
                is_stable = 1;
            } else {
                screen_write_lalign("BEAT ERR", RED);
                screen_update();
                is_stable = 0;
            }
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
