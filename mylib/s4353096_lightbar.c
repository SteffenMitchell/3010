/**
 ******************************************************************************
 * @file    mylib/sxxxxxx_ledbar.c
 * @author  MyName – MyStudent ID
 * @date    03032016
 * @brief   LED Light Bar peripheral driver
 *	     REFERENCE: LEDLightBar_datasheet.pdf
 *
 ******************************************************************************
 *     EXTERNAL FUNCTIONS
 ******************************************************************************
 * sxxxxxx_lightbar_init() – intialise LED Light BAR
 * sxxxxxx_lightbar_write() – set LED Light BAR value
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include "s4353096_lightbar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


void lightbar_seg_set(int segment, unsigned char segment_value) {

	/*
		Turn segment on (segment_value = 1) or off (segment_value = 0)

     */
		 switch (segment) {
		 case 0:
		 case 1:
		 case 2:
		 case 3:
		 case 4:
		 case 5:
		 case 6:
		 case 7:
		 case 8:
		 case 9:
		 default:
		 	 	break;
		 }
}

/**
  * @brief  Initialise LEDBar.
  * @param  None
  * @retval None
  */
extern void s4353096_lightbar_init(void) {

	/* Configure the GPIO_D0 pin

	 	....

		Configure the GPIO_D9 pin
    */
		//Enable D0-D9 Clocks
		__BRD_D0_GPIO_CLK();
		__BRD_D1_GPIO_CLK();
		__BRD_D2_GPIO_CLK();
		__BRD_D3_GPIO_CLK();
		__BRD_D4_GPIO_CLK();
		__BRD_D5_GPIO_CLK();
		__BRD_D6_GPIO_CLK();
		__BRD_D7_GPIO_CLK();
		__BRD_D8_GPIO_CLK();
		__BRD_D9_GPIO_CLK();
		//Set up Pin behaviour
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; //Output Mode
		GPIO_InitStructure.Pull = GPIO_PULLDOWN; //Pull down resistor
		GPIO_InitStructure.Speed = GPIO_SPEED_FAST; //Pun latency
		/*GPIO Pins D0-D9 are configured to the above specifications in the space
		bellow*/
		GPIO_InitStructure.Pin = BRD_D0_PIN;
		HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D1_PIN;
		HAL_GPIO_Init(BRD_D1_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D2_PIN;
		HAL_GPIO_Init(BRD_D2_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D3_PIN;
		HAL_GPIO_Init(BRD_D3_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D4_PIN;
		HAL_GPIO_Init(BRD_D4_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D5_PIN;
		HAL_GPIO_Init(BRD_D5_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D6_PIN;
		HAL_GPIO_Init(BRD_D6_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D7_PIN;
		HAL_GPIO_Init(BRD_D7_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D8_PIN;
		HAL_GPIO_Init(BRD_D8_GPIO_PORT, &GPIO_InitStructure);

		GPIO_InitStructure.Pin = BRD_D9_PIN;
		HAL_GPIO_Init(BRD_D9_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Set the LED Bar GPIO pins high or low, depending on the bit of ‘value’
  *         i.e. value bit 0 is 1 – LED Bar 0 on
  *          value bit 1 is 1 – LED BAR 1 on
  *
  * @param  value
  * @retval None
  */
extern void s4353096_lightbar_write(unsigned short value) {

	/* Use bit shifts (<< or >>) and bit masks (1 << bit_index) to determine if a bit is set

	   e.g. The following pseudo code checks if bit 0 of value is 1.
			if ((value & (1 << 0)) == (1 << 0))	{
				Turn on LED BAR Segment 0.
			}
		*/


}
