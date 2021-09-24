/*
 * Author: Daniel Blanco R 
 */

#ifndef I2C_STM32F7_H
#define I2C_STM32F7_H

/**		
 *    ________________________________________
 *			          I2C STM32F746ZG
 *		========================================   
 *		I2Cx				SCL		SDA				SCL		SDA
 *		========================================
 *		I2C1				PB6		PB7				PB8		PB9				
 *		I2C2				PB10	PB11			PF1		PF0		
 *		I2C3				PA8		PC9				 -		 -
 *		I2C4				PD12	PD13			PF14	PF15
 *	
 **/

#include <stdio.h>
#include <stdint.h>
#include <string>
#include "stm32f7xx.h"
#include "stm32f746xx.h"

#define _I2C1
#define _I2C2
#define _I2C3
#define _I2C4

/* Standard-mode (Sm) 10 kHz - HSI = 16 MHz */
#define PRESC_10_KHZ			0x03
#define SCLL_10_KHZ				0xC7
#define SCLH_10_KHZ				0xC3
#define SDADEL_10_KHZ			0x02
#define SCLDEL_10_KHZ			0x04

/* Standard-mode (Sm) 100 kHz - HSI = 16 MHz */
#define PRESC_100_KHZ			0x03
#define SCLL_100_KHZ			0x13
#define SCLH_100_KHZ			0x0F
#define SDADEL_100_KHZ		0x02
#define SCLDEL_100_KHZ		0x04

/* Fast-mode (Fm) 400 kHz - HSI = 16 MHz */
#define PRESC_400_KHZ			0x01
#define SCLL_400_KHZ			0x09
#define SCLH_400_KHZ			0x03
#define SDADEL_400_KHZ		0x02
#define SCLDEL_400_KHZ		0x03

/* Fast-mode Plus (Fm+) 1000 kHz - HSI = 16 MHz */
#define PRESC_1000_KHZ		0x00
#define SCLL_1000_KHZ			0x04
#define SCLH_1000_KHZ			0x02
#define SDADEL_1000_KHZ		0x00
#define SCLDEL_1000_KHZ		0x02

#define timeout_ 1000

class I2C_STM32F7 {

		public:

		I2C_STM32F7(I2C_TypeDef* I2Cx);
		~I2C_STM32F7(void);
		
		void I2C_Timingr(I2C_TypeDef* I2Cx,	
			uint8_t SCLL = SCLL_100_KHZ,	
			uint8_t SCLH = SCLH_100_KHZ,	
			uint8_t SDADEL = SDADEL_100_KHZ,	
			uint8_t SCLDEL = SCLDEL_100_KHZ,	
			uint8_t PRESC = PRESC_100_KHZ);
	
		uint8_t Master_Transmission(I2C_TypeDef* I2Cx, uint8_t Address_Slave, uint8_t Address_Data, uint8_t *buffer, uint8_t NBYTES);
		uint8_t Master_Reception(I2C_TypeDef* I2Cx, uint8_t Address_Slave, uint8_t Address_Register, uint8_t *buffer, uint8_t NBYTES);
		
		
		
	private:

		void I2C_GPIO_Config(I2C_TypeDef* I2Cx);
		void I2C_Initial_Settings(I2C_TypeDef* I2Cx);
		void Master_Initialization(I2C_TypeDef* I2Cx);
		
		uint32_t timeout;
		
};
#endif
