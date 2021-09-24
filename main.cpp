#include <stdio.h>
#include "stm32f7xx.h"
#include "I2C_STM32F7.h"
#include "MPU_REGISTERS.h"



uint8_t data[2];

uint8_t Buff[10];

int16_t ACCEL[4];
float Ax,Ay,Az;

I2C_STM32F7 i2c1(I2C1);

int t, tiempo = 100;

int main(void){

	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER |= (1UL<<2*14);
	GPIOB->OSPEEDR |= (2UL<<2*14);
	GPIOB->ODR &=~(1UL<<14);
	
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
	
	i2c1.Master_Reception(I2C1,MPU6050_DEFAULT_ADDRESS,MPU6050_RA_WHO_AM_I,&data[0],1);
	i2c1.Master_Reception(I2C1,MPU6050_DEFAULT_ADDRESS,MPU6050_RA_ACCEL_CONFIG,&data[1],1);
		
	while(1){
	
		
		if(t>tiempo){
			t=0;
			i2c1.Master_Reception(I2C1,MPU6050_DEFAULT_ADDRESS,MPU6050_RA_ACCEL_XOUT_H,Buff,6);
			ACCEL[0] = (Buff[0]<<8) | Buff[1];
			ACCEL[1] = (Buff[2]<<8) | Buff[3];
			ACCEL[2] = (Buff[4]<<8) | Buff[5];
			
			Ax = ACCEL[0] * (9.81 / 16384.00);
			Ay = ACCEL[1] * (9.81 / 16384.00);
			Az = ACCEL[2] * (9.81 / 16384.00);
			
			GPIOB->ODR ^= (1UL<<14);
		}
		

		
	}
}

extern "C"{

	void SysTick_Handler(void){
	
		t++;

	}
}
