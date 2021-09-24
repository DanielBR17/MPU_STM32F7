#include "I2C_STM32F7.h"

I2C_STM32F7::I2C_STM32F7(I2C_TypeDef* I2Cx){

	I2C_STM32F7::I2C_GPIO_Config(I2Cx);
}

I2C_STM32F7::~I2C_STM32F7(void){

}

void I2C_STM32F7::I2C_Timingr(I2C_TypeDef* I2Cx,	uint8_t SCLL,	uint8_t SCLH,	uint8_t SDADEL,	uint8_t SCLDEL,	uint8_t PRESC){
	
	I2Cx->TIMINGR |= (PRESC  << I2C_TIMINGR_PRESC_Pos);
	I2Cx->TIMINGR |= (SCLL 	 << I2C_TIMINGR_SCLL_Pos);
	I2Cx->TIMINGR |= (SCLH 	 << I2C_TIMINGR_SCLH_Pos);
	I2Cx->TIMINGR |= (SDADEL << I2C_TIMINGR_SDADEL_Pos);
	I2Cx->TIMINGR |= (SCLDEL << I2C_TIMINGR_SCLDEL_Pos);
	
}

void I2C_STM32F7::I2C_Initial_Settings(I2C_TypeDef* I2Cx){
	
	I2Cx->CR1 &=~ I2C_CR1_PE;
	I2Cx->CR1 &=~ I2C_CR1_ANFOFF;
	
	I2C_STM32F7::I2C_Timingr(I2Cx);
		
	I2Cx->CR1 |= I2C_CR1_NOSTRETCH;
	
	I2Cx->CR1 |= I2C_CR1_PE;
}

void I2C_STM32F7::Master_Initialization(I2C_TypeDef* I2Cx){
	
	I2C_STM32F7::I2C_Initial_Settings(I2Cx);	
	I2Cx->CR2 = 0;
}

uint8_t I2C_STM32F7::Master_Transmission(I2C_TypeDef* I2Cx, uint8_t Address_Slave, uint8_t Address_Data, uint8_t* buffer, uint8_t NBYTES){

	if(!(I2Cx->ISR & I2C_ISR_BUSY)){
	
		uint8_t n;
		
		I2C_STM32F7::Master_Initialization(I2Cx);
		
		I2Cx->CR2 &=~ I2C_CR2_SADD_Msk;
		I2Cx->CR2 |= (Address_Slave << 1U);
		
		I2Cx->CR2 &=~ I2C_CR2_RD_WRN;
		
		I2Cx->CR2 &=~ I2C_CR2_NBYTES;
		I2Cx->CR2 |= ( (NBYTES + 1) << 16U );
		
		I2Cx->CR2 |= I2C_CR2_AUTOEND;
		I2Cx->ISR &=~ I2C_ISR_STOPF;
		
		I2Cx->CR2 |= I2C_CR2_START;
		while(I2Cx->CR2 & I2C_CR2_START){};
		
		timeout = timeout_;
		while (((I2Cx->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS){if(timeout-- == 0 || (I2Cx->ISR & I2C_ISR_NACKF)){ I2Cx->CR2 |= I2C_CR2_STOP; I2Cx->ICR |= I2C_ICR_NACKCF;I2Cx->CR1 &=~ I2C_CR1_PE; return 1;}};

		I2Cx->TXDR = Address_Data;
		
		n = NBYTES;
//		while(!(I2Cx->ISR & I2C_ISR_TXE)){};

		timeout = timeout_;
		while(n > 0){
		
			while (((I2Cx->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS){if(timeout-- == 0 || (I2Cx->ISR & I2C_ISR_NACKF)){ I2Cx->CR2 |= I2C_CR2_STOP; I2Cx->ICR |= I2C_ICR_NACKCF;I2Cx->CR1 |= I2C_CR1_PE; return 1;}};
			I2Cx->TXDR = *buffer;
			buffer++;
			n--;
		}		
		while(!(I2Cx->ISR & I2C_ISR_STOPF)){};
		I2Cx->ISR &=~ I2C_ISR_STOPF;
		
	}
	return 0;
}


uint8_t I2C_STM32F7::Master_Reception(I2C_TypeDef* I2Cx,	uint8_t Address_Slave, uint8_t Address_Register, uint8_t* buffer, uint8_t NBYTES){
	
	if(!(I2Cx->ISR & I2C_ISR_BUSY)){
		
	uint8_t n;
	*buffer = 0;
				
	I2C_STM32F7::Master_Initialization(I2Cx);

	I2Cx->CR2 &=~ I2C_CR2_SADD_Msk;
	I2Cx->CR2 |= (Address_Slave << 1U);

	I2Cx->CR2 &=~ I2C_CR2_RD_WRN;

	I2Cx->CR2 &=~ I2C_CR2_NBYTES;
	I2Cx->CR2 |= ( 1 << 16U );	

	I2Cx->CR2 |= I2C_CR2_START;
	//while(I2Cx->CR2 & I2C_CR2_START){};
  timeout = timeout_;
	while (((I2Cx->ISR) & I2C_ISR_TXIS) != I2C_ISR_TXIS){if(timeout-- == 0 || (I2Cx->ISR & I2C_ISR_NACKF)){ I2Cx->CR2 |= I2C_CR2_STOP; I2Cx->ICR |= I2C_ICR_NACKCF; return 1;}};

	I2Cx->TXDR = Address_Register;

	timeout = timeout_;
	while (((I2Cx->ISR) & I2C_ISR_TC) != I2C_ISR_TC){if(timeout-- == 0 || (I2Cx->ISR & I2C_ISR_NACKF)){ I2Cx->CR2 |= I2C_CR2_STOP; I2Cx->ICR |= I2C_ICR_NACKCF; return 1;}};
		
	I2Cx->CR2 |= I2C_CR2_RD_WRN;
	
	I2Cx->CR2 &=~ I2C_CR2_NBYTES;
	I2Cx->CR2 |= (NBYTES << 16U);

	
	I2Cx->CR2 |= I2C_CR2_START;
	//while(I2Cx->CR2 & I2C_CR2_START){};
		
	n = NBYTES;
	
	timeout = timeout_;
	while (n--){
	
		while (!(I2Cx->ISR & I2C_ISR_RXNE)){if(timeout-- == 0 || (I2Cx->ISR & I2C_ISR_NACKF)){ I2Cx->CR2 |= I2C_CR2_STOP; I2Cx->ICR |= I2C_ICR_NACKCF; return 1;}};
		
		*buffer++ = I2Cx->RXDR;
	}
	
	I2Cx->CR2 |= I2C_CR2_STOP;
	while (I2Cx->CR2 & I2C_CR2_STOP){};
	
	}
	
	return 0;

}

void I2C_STM32F7::I2C_GPIO_Config(I2C_TypeDef* I2Cx){
	
#ifdef _I2C1	
	if (I2Cx == I2C1) {
		/* Enable GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* Enable I2C1 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		
	GPIOB->MODER |= (0x2 << GPIO_MODER_MODER6_Pos);
	GPIOB->MODER |= (0x2 << GPIO_MODER_MODER7_Pos);
	
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);
	
	GPIOB->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR6_Pos);
	GPIOB->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR7_Pos);
	
//	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPDR6_0;
//	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPDR7_0;
	GPIOB->PUPDR |= (1UL<<2*6);
	GPIOB->PUPDR |= (1UL<<2*7);
	
	GPIOB->AFR[0] |= (0x04 << GPIO_AFRL_AFRL6_Pos);
	GPIOB->AFR[0] |= (0x04 << GPIO_AFRL_AFRL7_Pos);
		
//	NVIC_EnableIRQ(I2C1_ER_IRQn);
	
	}
#endif

#ifdef _I2C2	
	if (I2Cx == I2C2) {
		/* Enable GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	/* Enable I2C2 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	GPIOB->MODER |= (0x2 << GPIO_MODER_MODER10_Pos);
	GPIOB->MODER |= (0x2 << GPIO_MODER_MODER11_Pos);
	
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_10 | GPIO_OTYPER_OT_11);
	
	GPIOB->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR10_Pos);
	GPIOB->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR11_Pos);
	
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPDR10_0;
	GPIOB->PUPDR &=~ GPIO_PUPDR_PUPDR11_0;
	
	GPIOB->AFR[1] |= (0x04 << GPIO_AFRH_AFRH2_Pos);
	GPIOB->AFR[1] |= (0x04 << GPIO_AFRH_AFRH3_Pos);
		
//	NVIC_EnableIRQ(I2C2_ER_IRQn);
		
	}
#endif
	
#ifdef _I2C3	
	if (I2Cx == I2C3) {
		/* Enable GPIOA GPIOC clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		
	/* Enable I2C3 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	
	GPIOA->MODER |= (0x2 << GPIO_MODER_MODER8_Pos);
	GPIOC->MODER |= (0x2 << GPIO_MODER_MODER9_Pos);
	
	GPIOA->OTYPER |= GPIO_OTYPER_OT_8;
	GPIOC->OTYPER |= GPIO_OTYPER_OT_9;
	
	GPIOA->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR8_Pos);
	GPIOC->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR9_Pos);
	
	GPIOA->PUPDR &=~ GPIO_PUPDR_PUPDR8_0;
	GPIOC->PUPDR &=~ GPIO_PUPDR_PUPDR9_0;
	
	GPIOA->AFR[1] |= (0x04 << GPIO_AFRH_AFRH0_Pos);
	GPIOC->AFR[1] |= (0x04 << GPIO_AFRH_AFRH1_Pos);
	
//	NVIC_EnableIRQ(I2C3_ER_IRQn);
	
	}
#endif
#ifdef _I2C4	
	if (I2Cx == I2C4) {
		/* Enable GPIOD clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
		
	/* Enable I2C4 clock */
	RCC->APB1ENR |= RCC_APB1ENR_I2C4EN;
	
	GPIOD->MODER |= (0x2 << GPIO_MODER_MODER12_Pos);
	GPIOD->MODER |= (0x2 << GPIO_MODER_MODER13_Pos);
	
	GPIOD->OTYPER |= (GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13);
	
	GPIOD->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR12_Pos);
	GPIOD->OSPEEDR |= (0x02 << GPIO_OSPEEDER_OSPEEDR13_Pos);
	
	GPIOD->PUPDR &=~ GPIO_PUPDR_PUPDR12_0;
	GPIOD->PUPDR &=~ GPIO_PUPDR_PUPDR13_0;
	
	GPIOD->AFR[1] |= (0x04 << GPIO_AFRH_AFRH4_Pos);
	GPIOD->AFR[1] |= (0x04 << GPIO_AFRH_AFRH5_Pos);
		
//	NVIC_EnableIRQ(I2C1_ER_IRQn);
		
	}
#endif

}
