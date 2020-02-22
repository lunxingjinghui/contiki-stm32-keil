/******************** (C) COPYRIGHT 2018 GZ GOSAFE******************************************
* 文件名称: main.c
* 程序作者: LJH
* 程序版本: V1.0.0
* 编制日期: 2018-10-02
* 功能描述: Contiki基于keil环境的移植，实现线程test1_process操作led0（PA8）250ms频率闪烁，
	线程test2_process操作led1（PD2）1000ms频率闪烁。
********************************************************************************************/

/********************************************************************************************
*									包含头文件
********************************************************************************************/
#include "main.h"
#include "stm32f1xx_hal.h"
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>

/********************************************************************************************
*									函数声明
********************************************************************************************/
/*-------------系统时钟设置------------*/ 
void SystemClock_Config(void);
/*-------------灯GPIO初始化------------*/ 
static void MX_GPIO_Init(void);


/********************************************************************************************
*									线程定义
********************************************************************************************/
/*---声明任务函数，定义任务控制块------*/ 
PROCESS(test1_process, "test1");
PROCESS(test2_process, "test2");


/********************************************************************************************
*									线程自启动绑定
********************************************************************************************/
AUTOSTART_PROCESSES(&test1_process, &test2_process);


/********************************************************************************************                                     
*函数名称:process_thread_test1_process
*函数功能:线程test1_process实现
*输入参数:ev	
*输入参数:data	
*输出参数:无
*返回值:无
*********************************************************************************************/ 
PROCESS_THREAD(test1_process, ev, data)
{
	PROCESS_BEGIN();										/*protothread机制所有线程以PROCESS_BEGIN()开始			*/

	static struct etimer timer;
	etimer_set(&timer, CLOCK_SECOND/4);						/*设置延时250ms											*/

	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));	/*等到时钟事件超时										*/
		etimer_reset(&timer);
		
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
	}

	PROCESS_END();											/*protothread机制所有线程以PROCESS_END()结束			*/
}

/********************************************************************************************                                     
*函数名称:process_thread_test2_process
*函数功能:线程test2_process实现
*输入参数:ev	
*输入参数:data	
*输出参数:无
*返回值:无
*********************************************************************************************/ 
PROCESS_THREAD(test2_process, ev, data)
{
	PROCESS_BEGIN();										/*protothread机制所有线程以PROCESS_BEGIN()开始			*/

	static struct etimer timer;
	etimer_set(&timer, CLOCK_SECOND);						/*设置延时1000ms										*/

	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);/*等到时钟事件超时，第二种写法							*/
		etimer_reset(&timer);

		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_2);

	}

	PROCESS_END();											/*protothread机制所有线程以PROCESS_END()结束			*/
}

/********************************************************************************************                                     
*函数名称:main
*函数功能:配置向量表 设置系统时钟 初始化系统 创建主任务
*输入参数:无
*输出参数:无
*返回值:无
*********************************************************************************************/ 
int main(void)
{
	HAL_Init();												/*HAL库初始化											*/
	SystemClock_Config();									/*stm32系统时钟初始化									*/
	MX_GPIO_Init();											/*硬件初始化											*/

	clock_init();											/*Contiki时钟初始化										*/
	process_init();											/*Contiki线程初始化										*/
	process_start(&etimer_process, NULL);					/*etimer线程启动										*/
	autostart_start(autostart_processes);					/*用户自启动线程启动									*/

	uint8_t AAAAA = 0;
	
	while(1)
	{
		do 
		{
			/*执行完所有needspoll为1的进程及处理完所有队列*/
		}
		while(process_run() > 0);							/*任务调度器轮询执行线程								*/
	}
}

/********************************************************************************************                                     
*函数名称:SystemClock_Config
*函数功能:系统时钟配置 内部时钟 PLL 24MHz
*输入参数:无
*输出参数:无
*返回值:无
*********************************************************************************************/ 
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
									|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/********************************************************************************************
*函数名称：MX_GPIO_Init
*函数功能：GPIO初始化
*输入参数：无
*输出参数：无
*返回值：  无
********************************************************************************************/
static void MX_GPIO_Init(void)
{
	/*------------初始化结构体----------------------*/
	GPIO_InitTypeDef GPIO_InitStruct;

	/*------------使能时钟--------------------------*/
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*------------引脚初始化--------------------------*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

	/*------------PA8引脚配置--------------------------*/
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*------------PD2引脚配置--------------------------*/
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/********************************************************************************************                                    
*函数名称:_Error_Handler
*函数功能:报告有HAL配置故障的位置
*输入参数:无
*输出参数:无
*返回值:无
*********************************************************************************************/
void _Error_Handler(char *file, int line)
{
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/********************************************************************************************                                    
*函数名称:assert_failed
*函数功能:当断言参数出现错误是，报告对应错误处的文件和函数
*输入参数:file 源文件的位置
*		  line 发生错误的行数
*输出参数:无
*返回值:无
*********************************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
}
#endif /* USE_FULL_ASSERT */

