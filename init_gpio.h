
void init_gpio(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // ���������� �������� �������� � ����� A
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // ���������� �������� �������� � ����� B
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // ���������� �������� �������� � ����� C

		//Remap PB3, PB4
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
		AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; // �������� JTAG, �.�. ������ PB3 � PB4 ������� �����/������

		GPIOA->CRH   |= GPIO_CRH_MODE11_1; // ���� PA11 �� �����. //Buzz
		GPIOA->CRH   &=~ GPIO_CRH_MODE11_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF11; //  ����������� ����� �� PA11

		////////////////////74HC595//////////////////////
		GPIOC->CRH   |= GPIO_CRH_MODE13_1; // ���� PC13 �� �����. //HC595_SCL
		GPIOC->CRH   &=~ GPIO_CRH_MODE13_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF13; //  ����������� ����� �� PC13

		GPIOC->CRH   |= GPIO_CRH_MODE14_1; // ���� PC14 �� �����. //HC595_LCL
		GPIOC->CRH   &=~ GPIO_CRH_MODE14_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF14; //  ����������� ����� �� PC14

		GPIOC->CRH   |= GPIO_CRH_MODE15_1; // ���� PC15 �� �����. //HC595_A
		GPIOC->CRH   &=~ GPIO_CRH_MODE15_0;
		GPIOC->CRH   &=~ GPIO_CRH_CNF15; //  ����������� ����� �� PC15
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
		GPIOA->CRL   &=~ GPIO_CRL_MODE7; //���� �� ���� � ��������� //STOP
		GPIOA->CRL   |= GPIO_CRL_CNF7_1; //
		GPIOA->CRL   &=~ GPIO_CRL_CNF7_0;
		GPIOA->ODR   |= GPIO_ODR_ODR7;

		GPIOB->CRL   &=~ GPIO_CRL_MODE0; //���� �� ���� � ��������� //F1
		GPIOB->CRL   |= GPIO_CRL_CNF0_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF0_0;
		GPIOB->ODR   |= GPIO_ODR_ODR0;

		GPIOB->CRL   &=~ GPIO_CRL_MODE1; //���� �� ���� � ��������� //F2
		GPIOB->CRL   |= GPIO_CRL_CNF1_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF1_0;
		GPIOB->ODR   |= GPIO_ODR_ODR1;

		GPIOB->CRL   &=~ GPIO_CRL_MODE2; //���� �� ���� � ��������� //F3
		GPIOB->CRL   |= GPIO_CRL_CNF2_1; //
		GPIOB->CRL   &=~ GPIO_CRL_CNF2_0;
		GPIOB->ODR   |= GPIO_ODR_ODR2;

		GPIOB->CRH   &=~ GPIO_CRH_MODE14; //���� �� ���� � ��������� //F4
		GPIOB->CRH   |= GPIO_CRH_CNF14_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF14_0;
		GPIOB->ODR   |= GPIO_ODR_ODR14;

		GPIOB->CRH   &=~ GPIO_CRH_MODE13; //���� �� ���� � ��������� //F5
		GPIOB->CRH   |= GPIO_CRH_CNF13_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF13_0;
		GPIOB->ODR   |= GPIO_ODR_ODR13;

		GPIOB->CRH   &=~ GPIO_CRH_MODE12; //���� �� ���� � ��������� //F6
		GPIOB->CRH   |= GPIO_CRH_CNF12_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF12_0;
		GPIOB->ODR   |= GPIO_ODR_ODR12;

		GPIOB->CRH   &=~ GPIO_CRH_MODE15; //���� �� ���� � ��������� //SW_serv_1
		GPIOB->CRH   |= GPIO_CRH_CNF15_1; //
		GPIOB->CRH   &=~ GPIO_CRH_CNF15_0;
		GPIOB->ODR   |= GPIO_ODR_ODR15;

		GPIOA->CRH   &=~ GPIO_CRH_MODE8; //���� �� ���� � ��������� //SW_serv_2
		GPIOA->CRH   |= GPIO_CRH_CNF8_1; //
		GPIOA->CRH   &=~ GPIO_CRH_CNF8_0;
		GPIOA->ODR   |= GPIO_ODR_ODR8;
		////////////////////////////////////////////////////////

		////////////////////////LED_MBI5026/////////////////////
		GPIOA->CRH   |= GPIO_CRH_MODE12_1; // ���� PA12 �� �����. //
		GPIOA->CRH   &=~ GPIO_CRH_MODE12_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF12; //  ����������� ����� �� PA12

		GPIOA->CRH   |= GPIO_CRH_MODE15_1; // ���� PA15 �� �����. //
		GPIOA->CRH   &=~ GPIO_CRH_MODE15_0;
		GPIOA->CRH   &=~ GPIO_CRH_CNF15; //  ����������� ����� �� PA15

		GPIOB->CRL   |= GPIO_CRL_MODE3_1; // ���� PB3 �� �����. //
		GPIOB->CRL   &=~ GPIO_CRL_MODE3_0;
		GPIOB->CRL   &=~ GPIO_CRL_CNF3; //  ����������� ����� �� PB3
		////////////////////////////////////////////////////////

		GPIOA->CRL   |= GPIO_CRL_MODE4_1; // ���� PA4 �� �����. //RX-TX_MAX485
		GPIOA->CRL   &=~ GPIO_CRL_MODE4_0;
		GPIOA->CRL   &=~ GPIO_CRL_CNF4; //

		GPIOA->BSRR=GPIO_BSRR_BS4;


		//��������������
		GPIOA->CRL &=~ GPIO_CRL_MODE0; // PA0 �� ���� ��� ������. ���������
		//��������������
		GPIOA->CRL &=~ GPIO_CRL_MODE1; // PA1 �� ���� ��� ������. ���������

		//�������� ���������� ��� ������ � ���������������
		AFIO->EXTICR [0>>0x02] |= AFIO_EXTICR1_EXTI0_PA; // ���������� INT0 �� PA0
		AFIO->EXTICR[0]|=AFIO_EXTICR1_EXTI1_PA; // ���������� INT1 �� PA1
		//���������� �� ������� � ������ ���� ���������
		EXTI->IMR|=(EXTI_IMR_MR0 | EXTI_IMR_MR1);
		//���������� �� ����� ����� �� ���������� ������
		EXTI->FTSR|=(EXTI_FTSR_TR0 | EXTI_FTSR_TR1);


		RCC->APB1ENR|= RCC_APB1ENR_USART2EN; // ��������� ������������ USART2.
		GPIOA->CRL |= GPIO_CRL_MODE2; // ����� TX PA.2 - �� �����.
		GPIOA->CRL &=~GPIO_CRL_CNF2; GPIOA->CRL |=GPIO_CRL_CNF2_1; // �������������� �����.
		USART2->CR1 |= USART_CR1_TE; // ��������� ����� TX PA2.
		// �������� 9600 bps. USARTDIV=FSYS/(16*baud) = 11.0592+e6/(16*9600) = 72,0.
		USART2->BRR=(72<<4); // ����� ����� ������������ ������� USART2.
		USART2->BRR|=0; // ������� �����*16 = 0,0*0 = 0 (���).
		USART2->CR1 |=USART_CR1_UE; // ��������� USART2.


		RCC->APB2ENR|= RCC_APB2ENR_USART1EN; // ��������� ������������ USART1.
		GPIOA->CRH |= GPIO_CRH_MODE9; // ����� TX PA.9 - �� �����.
		GPIOA->CRH &=~GPIO_CRH_CNF9; GPIOA->CRH |=GPIO_CRH_CNF9_1; // �������������� �����.
		USART1->CR1 |=(USART_CR1_RE | USART_CR1_TE); // ��������� ������ RX, TX - PA10, PA9.
		// �������� 9600 bps. USARTDIV=FSYS/(16*baud) = 11.0592+e6/(16*9600) = 72,0.
		USART1->BRR=(72<<4); // ����� ����� ������������ ������� USART1.
		USART1->BRR|=0; // ������� �����*16 = 0,0*16 = 0 (���).
		USART1->CR1 |=USART_CR1_UE; // ��������� USART1.

		// ��������� ���������� �� ����� ���������� � RxD
		//USART1->CR1 |= USART_CR1_RXNEIE;
		// ��������� ���������� ��� ���� ���������� �� USART1
		//NVIC_EnableIRQ(USART1_IRQn);
};
