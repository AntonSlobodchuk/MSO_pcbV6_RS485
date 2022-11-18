//�������� ������� - ������������ �������� �� 1 �� 65535 ����������� 
void delay_ms(uint16_t value){

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //������ ������������ �� TIM2
 TIM2->PSC     = 11059-1;             //��������� �������� ��� ������������ �����������
 TIM2->CR1     = TIM_CR1_OPM;        //����� ������ ��������

 TIM2->ARR = value;  //��������� �������� ��������
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //��������� ������
 while((TIM2->SR & TIM_SR_UIF)==0){} //��������� ����� ��������
 TIM2->SR &= ~TIM_SR_UIF;	      //�������� ����

};

void delay_us(uint16_t value){

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //������ ������������ �� TIM2
 TIM2->PSC     = 11-1;             //��������� �������� ��� ������������ �����������
 TIM2->CR1     = TIM_CR1_OPM;        //����� ������ ��������

 TIM2->ARR = value;  //��������� �������� ��������
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //��������� ������
 while((TIM2->SR & TIM_SR_UIF)==0){} //��������� ����� ��������
 TIM2->SR &= ~TIM_SR_UIF;	      //�������� ����

};

void delay_s(uint16_t value){ // ����. 327�.

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //������ ������������ �� TIM2
 TIM2->PSC     = 55295-1;             //��������� �������� ��� ������������ ��������� � 5 ����������
 TIM2->CR1     = TIM_CR1_OPM;        //����� ������ ��������

 TIM2->ARR = value*200;  //��������� �������� ��������
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //��������� ������
 while((TIM2->SR & TIM_SR_UIF)==0){} //��������� ����� ��������
 TIM2->SR &= ~TIM_SR_UIF;	      //�������� ����

};
