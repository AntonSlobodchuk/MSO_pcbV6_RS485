
void init_gpio(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // подключаем тактовые ипмульсы к порту A
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // подключаем тактовые ипмульсы к порту B
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // подключаем тактовые ипмульсы к порту C

		//Remap PB3, PB4
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; // отключим JTAG, т.е. вернем PB3 и PB4 функции ввода/вывода

		GPIOA->CRH   |= GPIO_CRH_MODE11_1; // Порт PA11 на выход. //Buzz
		GPIOA->CRH   &=~ GPIO_CRH_MODE11_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF11; //  Двухтактный выход на PA11

		////////////////////74HC595//////////////////////
		GPIOC->CRH   |= GPIO_CRH_MODE13_1; // Порт PC13 на выход. //HC595_SCL
		GPIOC->CRH   &=~ GPIO_CRH_MODE13_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF13; //  Двухтактный выход на PC13

		GPIOC->CRH   |= GPIO_CRH_MODE14_1; // Порт PC14 на выход. //HC595_LCL
		GPIOC->CRH   &=~ GPIO_CRH_MODE14_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF14; //  Двухтактный выход на PC14

		GPIOC->CRH   |= GPIO_CRH_MODE15_1; // Порт PC15 на выход. //HC595_A
		GPIOC->CRH   &=~ GPIO_CRH_MODE15_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF15; //  Двухтактный выход на PC15
		/////////////////////////////////////////////////


		////////////////////I2C/////////////////////////////////
		GPIOA->CRL   |= GPIO_CRL_MODE5_1; //Max. output speed 2 MHz
		GPIOA->CRL   &=~ GPIO_CRL_MODE5_0;
		GPIOA->CRL   &=~ GPIO_CRL_CNF5_1; //Open Drain
		GPIOA->CRL  |=  GPIO_CRL_CNF5_0;

		GPIOA->CRL   |= GPIO_CRL_MODE6_1; //Max. output speed 2 MHz
		GPIOA->CRL   &=~ GPIO_CRL_MODE6_0;
		GPIOA->CRL   &=~ GPIO_CRL_CNF6_1; //Open Drain
		GPIOA->CRL  |=  GPIO_CRL_CNF6_0;
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		GPIOA->CRL   &=~ GPIO_CRL_MODE7; //Порт на вход с подтяжкой //STOP
		GPIOA->CRL   |= GPIO_CRL_CNF7_1; //
		GPIOA->CRL   &=~ GPIO_CRL_CNF7_0;
		GPIOA->ODR   |= GPIO_ODR_ODR7;

		GPIOB->CRL   &=~ GPIO_CRL_MODE0; //Порт на вход с подтяжкой //F1
		GPIOB->CRL   |= GPIO_CRL_CNF0_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF0_0;
		GPIOB->ODR   |= GPIO_ODR_ODR0;

		GPIOB->CRL   &=~ GPIO_CRL_MODE1; //Порт на вход с подтяжкой //F2
		GPIOB->CRL   |= GPIO_CRL_CNF1_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF1_0;
		GPIOB->ODR   |= GPIO_ODR_ODR1;

		GPIOB->CRL   &=~ GPIO_CRL_MODE2; //Порт на вход с подтяжкой //F3
		GPIOB->CRL   |= GPIO_CRL_CNF2_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF2_0;
		GPIOB->ODR   |= GPIO_ODR_ODR2;

		GPIOB->CRH   &=~ GPIO_CRH_MODE14; //Порт на вход с подтяжкой //F4
		GPIOB->CRH   |= GPIO_CRH_CNF14_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF14_0;
		GPIOB->ODR   |= GPIO_ODR_ODR14;

		GPIOB->CRH   &=~ GPIO_CRH_MODE13; //Порт на вход с подтяжкой //F5
		GPIOB->CRH   |= GPIO_CRH_CNF13_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF13_0;
		GPIOB->ODR   |= GPIO_ODR_ODR13;

		GPIOB->CRH   &=~ GPIO_CRH_MODE12; //Порт на вход с подтяжкой //F6
		GPIOB->CRH   |= GPIO_CRH_CNF12_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF12_0;
		GPIOB->ODR   |= GPIO_ODR_ODR12;

		GPIOB->CRH   &=~ GPIO_CRH_MODE15; //Порт на вход с подтяжкой //SW_serv_1
		GPIOB->CRH   |= GPIO_CRH_CNF15_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF15_0;
		GPIOB->ODR   |= GPIO_ODR_ODR15;

		GPIOA->CRH   &=~ GPIO_CRH_MODE8; //Порт на вход с подтяжкой //SW_serv_2
		GPIOA->CRH   |= GPIO_CRH_CNF8_1; //
		GPIOA->CRH   &=~ GPIO_CRH_CNF8_0;
		GPIOA->ODR   |= GPIO_ODR_ODR8;
		////////////////////////////////////////////////////////

		////////////////////////LED_MBI5026/////////////////////
		GPIOA->CRH   |= GPIO_CRH_MODE12_1; // Порт PA12 на выход. //
		GPIOA->CRH   &=~ GPIO_CRH_MODE12_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF12; //  Двухтактный выход на PA12

		GPIOA->CRH   |= GPIO_CRH_MODE15_1; // Порт PA15 на выход. //
		GPIOA->CRH   &=~ GPIO_CRH_MODE15_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF15; //  Двухтактный выход на PA15

		GPIOB->CRL   |= GPIO_CRL_MODE3_1; // Порт PB3 на выход. //
		GPIOB->CRL   &=~ GPIO_CRL_MODE3_0;
		GPIOB->CRL   &=~ GPIO_CRL_CNF3; //  Двухтактный выход на PB3
		////////////////////////////////////////////////////////

		GPIOA->CRL   |= GPIO_CRL_MODE4_1; // Порт PA4 на выход. //RX-TX_MAX485
		GPIOA->CRL   &=~ GPIO_CRL_MODE4_0;
		GPIOA->CRL   &=~ GPIO_CRL_CNF4; //

		GPIOA->BSRR=GPIO_BSRR_BS4;


		//купюроприемник
		GPIOA->CRL &=~ GPIO_CRL_MODE0; // PA0 на ввод без подтяг. резистора
		//монетоприемник
		GPIOA->CRL &=~ GPIO_CRL_MODE1; // PA1 на ввод без подтяг. резистора

		//Настроим прерывания для купюро и монетоприемника
		AFIO->EXTICR [0>>0x02] |= AFIO_EXTICR1_EXTI0_PA; // Прерывание INT0 на PA0
		AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI1_PA; // Прерывание INT1 на PA1
		//Прерывания от нулевой и первой ноги разрешены
		EXTI->IMR|=(EXTI_IMR_MR0 | EXTI_IMR_MR1);
		//Прерывания на обоих ногах по спадающему фронту
		EXTI->FTSR|=(EXTI_FTSR_TR0 | EXTI_FTSR_TR1);


		RCC->APB1ENR|= RCC_APB1ENR_USART2EN; // Включение тактирования USART2.
		GPIOA->CRL |= GPIO_CRL_MODE2; // Вывод TX PA.2 - на выход.
		GPIOA->CRL &=~GPIO_CRL_CNF2; GPIOA->CRL |=GPIO_CRL_CNF2_1; // Альтернативный выход.
		USART2->CR1 |= USART_CR1_TE; // Разрешить вывод TX PA2.
		// Скорость 9600 bps. USARTDIV=FSYS/(16*baud) = 11.0592+e6/(16*9600) = 72,0.
		USART2->BRR=(72<<4); // Целая часть коэффициента деления USART2.
		USART2->BRR|=0; // Дробная часть*16 = 0,0*0 = 0 (окр).
		USART2->CR1 |=USART_CR1_UE; // Включение USART2.


		RCC->APB2ENR|= RCC_APB2ENR_USART1EN; // Включение тактирования USART1.
		GPIOA->CRH |= GPIO_CRH_MODE9; // Вывод TX PA.9 - на выход.
		GPIOA->CRH &=~GPIO_CRH_CNF9; GPIOA->CRH |=GPIO_CRH_CNF9_1; // Альтернативный выход.
		USART1->CR1 |=(USART_CR1_RE | USART_CR1_TE); // Разрешить выводы RX, TX - PA10, PA9.
		// Скорость 9600 bps. USARTDIV=FSYS/(16*baud) = 11.0592+e6/(16*9600) = 72,0.
		USART1->BRR=(72<<4); // Целая часть коэффициента деления USART1.
		USART1->BRR|=0; // Дробная часть*16 = 0,0*16 = 0 (окр).
		USART1->CR1 |=USART_CR1_UE; // Включение USART1.

		// Разрешаем прерывание по приёму информации с RxD
		//USART1->CR1 |= USART_CR1_RXNEIE;
		// Назначаем обработчик для всех прерываний от USART1
		//NVIC_EnableIRQ(USART1_IRQn);
};
