/**
 ******************************************************************************
 * @file mylib/s4353096_pantilt.c
 * @author Steffen Mitchell - 43530960
 * @date 16032015
 * @brief Servo Pan and Tilt peripheral driver
 * REFERENCE:
 ******************************************************************************
 * EXTERNAL FUNCTIONS
 ******************************************************************************
 * s4353096_pantilt_init() - Initialise servo (GPIO, PWM, Timer, etc)
 * s4353096_pantilt_angle(type, angle) - Write the pan or tilt servo to an angle
 ******************************************************************************
*/
/* Includes */
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4353096_pantilt.h"
/* Private typedef */
GPIO_InitTypeDef  GPIO_InitStructure;
TIM_OC_InitTypeDef PWMConfig;
TIM_HandleTypeDef TIM_Init;
uint16_t PrescalerValue = 0;
/*Initialise pantilt GPIO, Timer, PWM */
extern void s4353096_pantilt_init(void) {
  /* Enable the PWM Pin Clocks */
  __PWM_PAN_GPIO_CLK();
  __PWM_TILT_GPIO_CLK();
  /* Configure the PWM Pan pin with Timer output */
  GPIO_InitStructure.Pin = PWM_PAN_PIN;				//Pin
  GPIO_InitStructure.Mode =GPIO_MODE_AF_PP; 		//Set mode to be output alternate
  GPIO_InitStructure.Pull = GPIO_NOPULL;			//Enable Pull up, down or no pull resister
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;			//Pin latency
  GPIO_InitStructure.Alternate = PWM_PAN_GPIO_AF_TIM;	//Set alternate function to be timer pan
  HAL_GPIO_Init(PWM_PAN_GPIO_PORT, &GPIO_InitStructure);	//Initialise Pin
  /* Configure the PWM Tilt pin with Timer output */
  GPIO_InitStructure.Pin = PWM_TILT_PIN;				//Pin
  GPIO_InitStructure.Alternate = PWM_TILT_GPIO_AF_TIM;	//Set alternate function to be timer tilt
  HAL_GPIO_Init(PWM_TILT_GPIO_PORT, &GPIO_InitStructure);
  /* Enable clock for pan and tilt timer's  */
  __PWM_PAN_TIMER_CLK();
  //__PWM_TILT_TIMER_CLK();
  /* Compute the prescaler value. SystemCoreClock = 168000000 - set for 50Khz clock */
  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 500000) - 1;

  /* Configure Timer settings */
  TIM_Init.Instance = PWM_PAN_TIM;					//Enable Timer 2
  TIM_Init.Init.Period = (500000/1000)*20;			//Set for 200ms (5Hz) period
  TIM_Init.Init.Prescaler = PrescalerValue;	//Set presale value
  TIM_Init.Init.ClockDivision = 0;			//Set clock division
  TIM_Init.Init.RepetitionCounter = 0; 		// Set Reload Value
  TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.

  /* PWM Mode configuration for Channel 2 - set pulse width*/
  PWMConfig.OCMode			 = TIM_OCMODE_PWM1;	//Set PWM MODE (1 or 2 - NOT CHANNEL)
  PWMConfig.Pulse				= (((2*500000)/10000) * 7.25); //Sets initial servo position to 0 degrees
  PWMConfig.OCPolarity	 = TIM_OCPOLARITY_HIGH;
  PWMConfig.OCNPolarity	= TIM_OCNPOLARITY_HIGH;
  PWMConfig.OCFastMode	 = TIM_OCFAST_DISABLE;
  PWMConfig.OCIdleState	= TIM_OCIDLESTATE_RESET;
  PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  /* Enable PWM for PWM Pan Timer */
  HAL_TIM_PWM_Init(&TIM_Init);
  HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, PWM_PAN_TIM_CHANNEL);

  /* Start PWM for Pan*/
  HAL_TIM_PWM_Start(&TIM_Init, PWM_PAN_TIM_CHANNEL);

  /* Configure Timer setting for PWM Tilt */
  TIM_Init.Instance = PWM_TILT_TIM;
  /* Enable PWM for PWM Tilt Timer */
  HAL_TIM_PWM_Init(&TIM_Init);
  HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, PWM_TILT_TIM_CHANNEL);

  /* Start PWM for Tilt*/
  HAL_TIM_PWM_Start(&TIM_Init, PWM_TILT_TIM_CHANNEL);
  /*Set up Interrupt timer*/
  __PANTILT_IR_TIMER_CLK();
	/* Compute the prescaler value for 50Khz */
  PrescalerValue = (uint16_t) ((SystemCoreClock /2)/50000) - 1;
	/* Time base configuration */
	TIM_Init.Instance = PANTILT_IR_TIM;				//Enable Timer 2
	//Set period count to be 1ms, so timer interrupt occurs every (1ms)*0.2.
  TIM_Init.Init.Period = (50000/1000)*20;//*0.18;
  TIM_Init.Init.Prescaler = PrescalerValue;	//Set presale value
  TIM_Init.Init.ClockDivision = 0;			//Set clock division
	TIM_Init.Init.RepetitionCounter = 0;	// Set Reload Value
  TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;	//Set timer to count up.
	/* Initialise Timer */
	HAL_TIM_Base_Init(&TIM_Init);

	/* Set priority of Timer 2 update Interrupt [0 (HIGH priority) to 15(LOW priority)] */
	/* 	DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
	HAL_NVIC_SetPriority(PANTILT_TIM_IRQn, 10, 0);		//Set Main priority ot 10 and sub-priority ot 0.

	/* Enable timer update interrupt and interrupt vector for Timer  */
	NVIC_SetVector(PANTILT_TIM_IRQn, (uint32_t)&s4353096_pantilt_irqhandler);
	NVIC_EnableIRQ(PANTILT_TIM_IRQn);
	/*Start the timer*/
	HAL_TIM_Base_Start_IT(&TIM_Init);
}

extern void s4353096_pantilt_angle_write(int type, int angle) {
  float pwm_pulse_period_percentage;
  float pwm_multiplier = 4.723 * (angle/85.000);
  /*If negative*/
  if ((angle < 0) && (angle > -77)) {
    pwm_pulse_period_percentage = (7.25 - (-1*pwm_multiplier));
    PWMConfig.Pulse				= (((2*500000)/10000) * pwm_pulse_period_percentage);
                            //(((2*500000)/10000) * 7.25)
  } else if ((angle >= 0) && (angle < 77)) {
    pwm_pulse_period_percentage = ((7.25 + pwm_multiplier));
    PWMConfig.Pulse				= (((2*500000)/10000) * pwm_pulse_period_percentage);
  } else {

  }
  /*Type 1 == Pan */
  if (type == 1) {
    TIM_Init.Instance = PWM_PAN_TIM;
    /* Enable PWM for PWM Pan Timer */
    //HAL_TIM_PWM_Init(&TIM_Init);
    HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, PWM_PAN_TIM_CHANNEL);
    HAL_TIM_PWM_Start(&TIM_Init, PWM_PAN_TIM_CHANNEL);
  } else if (type == 0) { /*If Type 0 == Tilt */
    TIM_Init.Instance = PWM_TILT_TIM;
    //HAL_TIM_PWM_Init(&TIM_Init);
    HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, PWM_TILT_TIM_CHANNEL);
    HAL_TIM_PWM_Start(&TIM_Init, PWM_TILT_TIM_CHANNEL);
  } else {

  }
}
