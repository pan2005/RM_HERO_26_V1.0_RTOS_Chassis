/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "bsp_usart.h"
#include "CAN_receive.h"
#include "remote_control.h"
#include "chassis_control_task.h"
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
/* USER CODE BEGIN Variables */
extern const RC_ctrl_t * local_rc_ctrl;
/* USER CODE END Variables */
/* Definitions for test */
osThreadId_t testHandle;
const osThreadAttr_t test_attributes = {
  .name = "test",
  .stack_size = 128 ,
  .priority = (osPriority_t) osPriorityNormal,
};


osThreadId_t print_dataHandle;
const osThreadAttr_t print_attributes = {
  .name = "print_data",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

osThreadId_t cybergearHandle;
const osThreadAttr_t cybergear_attributes = {
  .name = "Cybergear",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityHigh,
};
osThreadId_t LKMotorHandle;
const osThreadAttr_t LKmotor_attributes = {
  .name = "Lkmotor",
  .stack_size = 256,
  .priority = (osPriority_t) osPriorityHigh,
};

osThreadId_t DaemonHandle;
const osThreadAttr_t Daemon_attributes = {
  .name = "Daemon",
  .stack_size =  256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

osThreadId_t Sup_capHandle;
 const osThreadAttr_t Sup_cap_attributes = {
   .name = "Daemon",
   .stack_size =  256,
   .priority = (osPriority_t) osPriorityHigh,
 };

osThreadId_t Chassis_Handel;
const osThreadAttr_t Chassis_attributes = {
  .name = "Chassis",
  .stack_size = 256 * 2,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void test_task(void *argument);
void print_task(void *argument);
void cybergear_task(void *argument);
void LK_Motor_testtask(void* argument) ;
void Daemon_Task(void *pvParameters);
void Super_capacitor_Task(void *pvParameter);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */
void chassis_test_task(void * argument);
void chassis_control_task();

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of test */
  testHandle = osThreadNew(test_task, NULL, &test_attributes);
  //print_dataHandle = osThreadNew(print_task,NULL,&print_attributes);
 //cybergearHandle = osThreadNew(cybergear_task,NULL,&cybergear_attributes);
  LKMotorHandle = osThreadNew(LK_Motor_testtask,NULL,&LKmotor_attributes);
  //DaemonHandle = osThreadNew(Daemon_Task,NULL,&Daemon_attributes);
  //Sup_capHandle = osThreadNew(Super_capacitor_Task,NULL,&Sup_cap_attributes);
  //Chassis_Handel = osThreadNew(chassis_control_task,NULL,&Chassis_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_test_task */
/**
  * @brief  Function implementing the test thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_test_task */
__weak void test_task(void *argument)
{
  /* init code for USB_DEVICE */
 //MX_USB_DEVICE_Init();
  /* USER CODE BEGIN test_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END test_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void chassis_test_task(void * argument) {
  int16_t velocity_forword = 0;
  int16_t velocity_turn = 0;
  int16_t velocity_14 = 0;
  int16_t velocity_23 = 0;

  while (1) {
    if (switch_is_down(local_rc_ctrl->rc.s[1])) {
      velocity_forword = local_rc_ctrl->rc.ch[3] * 40;
      velocity_turn = local_rc_ctrl->rc.ch[2] * 5;
      CAN_cmd_chassis(velocity_forword + velocity_turn, -(velocity_forword - velocity_turn), velocity_forword + velocity_turn, -(velocity_forword - velocity_turn));

    }
    else if (switch_is_up(local_rc_ctrl->rc.s[1])) {

      velocity_14 = local_rc_ctrl->rc.ch[2] * 30 + local_rc_ctrl->rc.ch[3] * 30;
      velocity_23 = -local_rc_ctrl->rc.ch[2] * 30 + local_rc_ctrl->rc.ch[3] * 30;
      CAN_cmd_chassis(velocity_14,-velocity_23,velocity_23,-velocity_14);




    }
    osDelay(2);

  }


}
/* USER CODE END Application */

