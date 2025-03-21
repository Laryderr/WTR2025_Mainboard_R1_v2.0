/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DJI.h"
#include "wtr_can.h"
#include "Caculate.h"
#include "Expansion.h"
#include "Unitree_user.h"
#include "io_retargetToUart.h"
#include "encoder.h"
#include <stdio.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
  UnitreeMotor motor3;
  UnitreeMotor motor4;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART6_UART_Init();
  MX_UART8_Init();
  /* USER CODE BEGIN 2 */
  CANFilterInit(&hcan1);
  CAN2FilterInit(&hcan2);
  Encoder_Init(&hcan2);
  for (uint8_t i = 0; i < 8; ++i) hDJI[i].motorType = M3508;
  DJI_Init();
  Expansion_Init();
  
  /*
  UnitreeMotor shootMotor;
  if (Unitree_init(&shootMotor, &huart6, 3) == HAL_OK) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
  else HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2, GPIO_PIN_RESET);
  Unitree_UART_tranANDrev(&shootMotor, 3, 1, 0, 0, 0, 0, 0);
  float startpos = shootMotor.data.Pos;
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4, GPIO_PIN_SET);
  Expansion_Up.expand();
  //Expansion_Down.expand();
  */
 /*
 UnitreeMotor motor0;
 UnitreeMotor motor1;
 UnitreeMotor motor2;
 */

  float motor3POffset;
  float motor4POffset;
  /*
  if (Unitree_init(&motor0, &huart6, 0) == HAL_OK)
  {
    printf("1OK\n");
    HAL_Delay(10);
  }
  else
  {
    printf("1BAD\n");
    HAL_Delay(10);
  }
  if (Unitree_init(&motor1, &huart6, 1) == HAL_OK)
  {
    printf("1OK\n");
    HAL_Delay(10);
  }
  else
  {
    printf("1BAD\n");
    HAL_Delay(10);
  }
  if (Unitree_init(&motor2, &huart6, 2) == HAL_OK)
  {
    printf("1OK\n");
    HAL_Delay(10);
  }
  else
  {
    printf("1BAD\n");
    HAL_Delay(10);
  }
  */
  if (Unitree_init(&motor3, &huart6, 3) == HAL_OK)
  {
    printf("1OK\n");
    HAL_Delay(10);
  }
  else
  {
    printf("1BAD\n");
    HAL_Delay(10);
  }
  if (Unitree_init(&motor4, &huart6, 4) == HAL_OK) 
  {
    printf("0OK\n");
    HAL_Delay(10);
    }
  else
  {
    printf("0BAD\n");
    HAL_Delay(10);
  }
  for (uint8_t i = 0; i < 10; ++i)
  {
    Unitree_UART_tranANDrev(&motor3, 3, 1, 0, 0, 0, 0, 0);
    Unitree_UART_tranANDrev(&motor4, 4, 1, 0, 0, 0, 0, 0);    
    motor3POffset += motor3.data.Pos / UNITREE_REDUCTION_RATE;
    motor4POffset += motor4.data.Pos / UNITREE_REDUCTION_RATE;
  }
  motor3POffset /= 10;
  motor4POffset /= 10;
  HAL_Delay(200);
    
    while (caldata.angle > 30500)
    {
        Unitree_UART_tranANDrev(&motor3, 3, 1, -20, 0, 0, 0, 0);
        Unitree_UART_tranANDrev(&motor4, 4, 1, 20, 0, 0, 0, 0);
        HAL_Delay(2);
    }
    while (caldata.angle < 34000)
    {
        Unitree_UART_tranANDrev(&motor3, 3, 1, 0, 0, 0, 0, 0);
        Unitree_UART_tranANDrev(&motor4, 4, 1, -3, 0.2, motor4POffset, 0.8, 0.1);
        HAL_Delay(2);
    }
    
   HAL_Delay(500);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Unitree_UART_tranANDrev(&motor4, 4, 1, 0, 0, 0, 0, 0);
    HAL_Delay(2);
    //printf("123\n");
    
    /*
    //Expansion_Up.m_Angle = 10 * 360;
    //Expansion_Down.m_Angle = 360;
    //Expansion_Up.m_Angle = -360;
    
    Expansion_Executor();
    HAL_Delay(2);
    */
    /*
    Expansion_Up.m_Angle = 2 * 360;
    float DJI_Output_WithFC_Down[2];
    float DJI_Output_WithFC_Up[2];
    for (uint8_t i = 0; i < 2; ++i)
    {
        positionServo(Expansion_Down.m_Angle,   Expansion_Down.m_DJI + i);
        if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output + (Expansion_Down.m_DJI + i)->f_current;
        else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output - (Expansion_Down.m_DJI + i)->f_current;
        else
            DJI_Output_WithFC_Down[i] = (Expansion_Down.m_DJI + i)->speedPID.output;
        
        positionServo(Expansion_Up.m_Angle,     Expansion_Up.m_DJI + i);        
        if((Expansion_Down.m_DJI + i)->speedPID.output > 100)
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output + (Expansion_Up.m_DJI + i)->f_current;
        else if((Expansion_Down.m_DJI + i)->speedPID.output < -100)
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output - (Expansion_Up.m_DJI + i)->f_current;
        else
            DJI_Output_WithFC_Up[i] = (Expansion_Up.m_DJI + i)->speedPID.output;
    }
    CanTransmit_DJI_5678(&hcan1, DJI_Output_WithFC_Down[0], DJI_Output_WithFC_Down[1], DJI_Output_WithFC_Up[0], DJI_Output_WithFC_Up[1]);
    HAL_Delay(2);
    */
    /*
    float dji_output_withfc[8];
    for(int i=0;i<8;i++)
    {
        positionServo(10, &hDJI[4]);
        positionServo(10, &hDJI[5]);
        if(hDJI[i].speedPID.output>100)
            dji_output_withfc[i]=hDJI[i].speedPID.output+hDJI[i].f_current;
        else if(hDJI[i].speedPID.output<-100)
            dji_output_withfc[i]=hDJI[i].speedPID.output-hDJI[i].f_current;
        else
            dji_output_withfc[i]=hDJI[i].speedPID.output;
    }
    CanTransmit_DJI_1234(&hcan1,dji_output_withfc[0],dji_output_withfc[1],dji_output_withfc[2],dji_output_withfc[3]);
    CanTransmit_DJI_5678(&hcan1,dji_output_withfc[4],dji_output_withfc[5],dji_output_withfc[6],dji_output_withfc[7]);
    HAL_Delay(2);
    */
    /*
    float tran = -1.2 + shootMotor.cmd.K_P * (shootMotor.cmd.Pos - shootMotor.data.Pos) + shootMotor.cmd.K_W * (shootMotor.cmd.W - shootMotor.data.W);
    //Unitree_UART_tranANDrev(&shootMotor, 3, 1, tran, -0.5, -0.75, 0.3, 0.05); //前馈力矩-1
    Unitree_UART_tranANDrev(&shootMotor, 3, 1, tran, -0.5, startpos-2, 0.6, 0.045);
    //Unitree_UART_tranANDrev(&shootMotor, 3, 1, 0, -0.5, startpos-0.5, 0.1, 0.02);
    //Unitree_UART_tranANDrev(&shootMotor, 3, 1, 0, 0, 0, 0, 0);
    HAL_Delay(5);
    */
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
