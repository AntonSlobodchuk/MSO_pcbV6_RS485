//аргумент функции - длительность задержки от 1 до 65535 миллисекунд 
void delay_ms(uint16_t value){

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //подать тактирование на TIM2
 TIM2->PSC     = 11059-1;             //настроить делитель для формирования миллисекунд
 TIM2->CR1     = TIM_CR1_OPM;        //режим одного импульса

 TIM2->ARR = value;  //загрузить значение задержки
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //запустить таймер
 while((TIM2->SR & TIM_SR_UIF)==0){} //дождаться конца задержки
 TIM2->SR &= ~TIM_SR_UIF;	      //сбросить флаг

};

void delay_us(uint16_t value){

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //подать тактирование на TIM2
 TIM2->PSC     = 11-1;             //настроить делитель для формирования микросекунд
 TIM2->CR1     = TIM_CR1_OPM;        //режим одного импульса

 TIM2->ARR = value;  //загрузить значение задержки
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //запустить таймер
 while((TIM2->SR & TIM_SR_UIF)==0){} //дождаться конца задержки
 TIM2->SR &= ~TIM_SR_UIF;	      //сбросить флаг

};

void delay_s(uint16_t value){ // макс. 327с.

 RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   //подать тактирование на TIM2
 TIM2->PSC     = 55295-1;             //настроить делитель для формирования интервала в 5 милисекунд
 TIM2->CR1     = TIM_CR1_OPM;        //режим одного импульса

 TIM2->ARR = value*200;  //загрузить значение задержки
 TIM2->CNT = 0;

 TIM2->EGR |= TIM_EGR_UG;
 TIM2->SR &= ~(TIM_SR_UIF);

 TIM2->CR1 = TIM_CR1_CEN; //запустить таймер
 while((TIM2->SR & TIM_SR_UIF)==0){} //дождаться конца задержки
 TIM2->SR &= ~TIM_SR_UIF;	      //сбросить флаг

};
