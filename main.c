#include "stm32f10x.h"
#include "delay.h"
#include "pll_f100_VL.h"
#include "itoa_ltoa.h"
#include "lcd.h"
#include "led_7_segm_4_razr.h"
#include "i2c_software.h"
#include "init_gpio.h"

#define All_led_off 7
#define STOP_on 0
#define F1_on 1
#define F2_on 2
#define F3_on 3
#define F4_on 4
#define F5_on 5
#define F6_on 6

#define buzer_on GPIOA->BSRR=GPIO_BSRR_BS11
#define buzer_off GPIOA->BSRR=GPIO_BSRR_BR11

#define HC595_SCL_on GPIOC->BSRR=GPIO_BSRR_BS13
#define HC595_SCL_off GPIOC->BSRR=GPIO_BSRR_BR13
#define HC595_LCL_on GPIOC->BSRR=GPIO_BSRR_BS14
#define HC595_LCL_off GPIOC->BSRR=GPIO_BSRR_BR14
#define HC595_A_on GPIOC->BSRR=GPIO_BSRR_BS15
#define HC595_A_off GPIOC->BSRR=GPIO_BSRR_BR15

//"Сумма общая" 0-3 (4 байта)
//"Сумма суточная" 4-7 (4 байта)
//"F1" 8-11 (4 байта)
//"F2" 12-15 (4 байта)
//"F3" 16-19 (4 байта)
//"F4" 20-23 (4 байта)
//"F5" 24-27 (4 байта)
//"F6" 28-31 (4 байта)
//"Цена купюры" 32-35 (4 байта)
//"Цена монеты" 36-39 (4 байта)
//"Время паузы" 40-43 (4 байта)
//Моточасы в секундах 44-47 (4 байта)

//F1_rele 100 (1 байт)
//F2_rele 101 (1 байт)
//F3_rele 102 (1 байт)
//F4_rele 103 (1 байт)
//F5_rele 104 (1 байт)
//F6_rele 105 (1 байт)
//F1_MotPow 106 (1 байт)
//F2_MotPow 107 (1 байт)
//F3_MotPow 108 (1 байт)
//F4_MotPow 109 (1 байт)
//F5_MotPow 110 (1 байт)
//F6_MotPow 111 (1 байт)

unsigned char SUPER_REJIM=0;

unsigned char count_svet_eff;

unsigned char ind_vr_pauzi=0;

unsigned char TEST_REJIM=0;

char lcd_buff[33]; // строковая переменная для вывода на символьный дмсплей

unsigned char flag_zavershenia_prer_t3, flag_zavershenia_prer_t4;

//unsigned char flag_kup=0, flag_mon=0, flag_kn1_servise, flag_kn2_servise, flag_kn3_servise, flag_kn4_servise, flag_kn5_servise; // флаги нажатия кнопок
unsigned char flag_F1, flag_F2, flag_F3, flag_F4, flag_F5, flag_F6=0, flag_stop=1;
unsigned char flag_na_zapis_eeprom;
unsigned char F_vibor=0, s_vibor=1, s_flag_indication=1, flag_indication=1; // переменные системного режима
float summa_obshiya, summa_sutochnaya; //переменная для хранения общей суммы 2^32
float stoim_F1, stoim_F2, stoim_F3, stoim_F4, stoim_F5, stoim_F6, cena_kupyri, cena_moneti, vremia_pauzi;

float balance;
unsigned int vremia_pauzi_temp;

unsigned char system_poloscanie;
unsigned char s_vverh=0;
unsigned char flag_na_zapis_summ;

//float temp;
unsigned char i; //переменная для цикла очистки еепром памяти

unsigned char test_stop;

unsigned char flag_time_indication;
float time_uslugi;
unsigned int time_uslugi_min,time_uslugi_sec;
unsigned long int moto_sec;


unsigned int key_cnt, money_cnt;

float balance;

unsigned char F1_rele, F2_rele, F3_rele, F4_rele, F5_rele, F6_rele;
unsigned char F1_MotPow, F2_MotPow, F3_MotPow, F4_MotPow, F5_MotPow, F6_MotPow;

void hc595_led(unsigned char data){
	unsigned char i, temp;
	switch(data)
	{
	    case 0: data=0x02; break;
	    case 1: data=0x04; break;
	    case 2: data=0x08; break;
	    case 3: data=0x10; break;
	    case 4: data=0x20; break;
	    case 5: data=0x40; break;
	    case 6: data=0x80; break;

	    case 7: data=0x00; break;

	    default: data=0x00; break;
	}

	temp=0x80;
	for(i=0; i<8; i++){
		HC595_SCL_off;
		if(data & temp){HC595_A_on;}else{HC595_A_off;};
		HC595_SCL_on;
		HC595_A_off;
		HC595_SCL_off;

		temp=temp>>1;
	};

	HC595_LCL_on;
	HC595_LCL_off;



};

void EXTI0_IRQHandler(void)// Обработчик прерывания EXTI0 Купюроприемник
{
	EXTI->PR|=0x01; //Очищаем флаг
	NVIC_DisableIRQ (EXTI0_IRQn);

	money_cnt=0;
	//Избавимся от дребезга контакта
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};

	if(money_cnt==5){

		balance=balance+cena_kupyri;

		flag_na_zapis_summ=1;

		led_7_segm_indication_4_razr(balance);

		flag_indication=1;

		vremia_pauzi_temp=vremia_pauzi; //скопируем значение переменной паузы, для дайнейшер работы с нею


		if((GPIOA->IDR & GPIO_IDR_IDR7)==0){//стоп
			SUPER_REJIM=1;
		};

	};


	NVIC_EnableIRQ (EXTI0_IRQn);

}

void EXTI1_IRQHandler(void)// Обработчик прерывания EXTI1 Монетоприемник
	{
	EXTI->PR|=0x02; //Очищаем флаг
	NVIC_DisableIRQ (EXTI1_IRQn);

	money_cnt=0;
	//Избавимся от дребезга контакта
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};
	if((GPIOA->IDR & GPIO_IDR_IDR0)==0){money_cnt++;};

	if(money_cnt==5){


		balance=balance+cena_moneti;

		flag_na_zapis_summ=1;

		led_7_segm_indication_4_razr(balance);

		flag_indication=1;

		vremia_pauzi_temp=vremia_pauzi; //скопируем значение переменной паузы, для дайнейшер работы с нею


		if((GPIOA->IDR & GPIO_IDR_IDR7)==0){//стоп
			SUPER_REJIM=1;
		};

	};



	NVIC_EnableIRQ (EXTI1_IRQn);
	};

void usart_print(char *usart_string){
	unsigned char tmp=0;
	while (usart_string[tmp]) // Пока не конец строки...
		{
		 while (!(USART1->SR & USART_SR_TXE)) {} // Ждать освобождения буфера.
		 USART1->DR=usart_string[tmp]; tmp++; // Отправить байт.
		}
};


void usart2_tx_data(char data){

		 while (!(USART2->SR & USART_SR_TXE)) {} // Ждать освобождения буфера.
		 USART2->DR=data; // Отправить байт.

};

void tim3_interrupt(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->ARR = 1000; // ведем счет до 1000
	TIM3->PSC = 11000-1; // пределитель таймера 11.0592МГц/11000=1кГц
	TIM3->CNT = 0;

	TIM3->CR1 = TIM_CR1_OPM; //режим одного импульса

	TIM3->DIER &=~ TIM_DIER_UIE; //запрещаем прерывание от таймера
	TIM3->EGR |= TIM_EGR_UG; //Update generation (Генерируем Событие обновления для записи данных в регистры PSC и ARR)
	TIM3->SR &=~ (TIM_SR_UIF); //очистим флаг вызова прерывания
	TIM3->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера
	NVIC_EnableIRQ(TIM3_IRQn); //разрешаем прерывание
	TIM3->CR1 |= TIM_CR1_CEN; // начинаем отсчет :)
};

void tim4_interrupt(void){ // Таймер для световых эффектов
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->ARR = 200; // ведем счет до 200
	TIM4->PSC = 11000-1; // пределитель таймера 11.0592МГц/11000=1кГц
	TIM4->CNT = 0;

	TIM4->CR1 = TIM_CR1_OPM; //режим одного импульса

	TIM4->DIER &=~ TIM_DIER_UIE; //запрещаем прерывание от таймера
	TIM4->EGR |= TIM_EGR_UG; //Update generation (Генерируем Событие обновления для записи данных в регистры PSC и ARR)
	TIM4->SR &=~ (TIM_SR_UIF); //очистим флаг вызова прерывания
	TIM4->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера
	NVIC_EnableIRQ(TIM4_IRQn); //разрешаем прерывание
	TIM4->CR1 |= TIM_CR1_CEN; // начинаем отсчет :)
};

void Rele_AtoG_OFF(void){

	hc595_led(STOP_on);

	usart2_tx_data(0x30); // Rele A OFF
	delay_us(10);
	usart2_tx_data(0x31); // Rele B OFF
	delay_us(10);
	usart2_tx_data(0x32); // Rele C OFF
	delay_us(10);
	usart2_tx_data(0x33); // Rele D OFF
	delay_us(10);
	usart2_tx_data(0x34); // Rele P OFF
	delay_us(10);
	usart2_tx_data(0x35); // Rele V OFF
	delay_us(10);
	usart2_tx_data(0x36); // Rele E OFF
	delay_us(10);
	usart2_tx_data(0x37); // Rele G OFF
	delay_us(10);

};

//функция вывода на экран FLOAT
void ftoa_print(float f, unsigned char n){ // n в диапазоне 1..3 (это знаки после запятой)
		long temp;
		temp=f;
		ltoa(temp, lcd_buff);
		lcd_puts(lcd_buff);
		f=f-temp;

			if(n==1){
				sprintf(lcd_buff,",");
				lcd_puts(lcd_buff);
			f=f*10;
			temp=f; if(temp<0){temp=(-1)*temp;};
			ltoa(temp, lcd_buff);
			lcd_puts(lcd_buff);
			};
			if(n==2){
				sprintf(lcd_buff,",");
				lcd_puts(lcd_buff);
			f=f*100;
			temp=f; if(temp<0){temp=(-1)*temp;};
				if(temp<10){sprintf(lcd_buff,"0"); lcd_puts(lcd_buff);};
			ltoa(temp, lcd_buff);
			lcd_puts(lcd_buff);
			};
			if(n==3){
				sprintf(lcd_buff,",");
				lcd_puts(lcd_buff);
			f=f*1000;
			temp=f; if(temp<0){temp=(-1)*temp;};
			ltoa(temp, lcd_buff);
			lcd_puts(lcd_buff);
			};
};

void TIM3_IRQHandler (void) // обработчик прерывания по таймеру 3
{
TIM3->SR &=~ TIM_SR_UIF; //очистим флаг вызова прерывания
flag_zavershenia_prer_t3=0;

IWDG->KR = 0xAAAA; // refresh IWDG

if(balance>0){
	if(flag_stop==0){
		moto_sec++;
		flag_time_indication=1;
		vremia_pauzi_temp=vremia_pauzi;

		if(SUPER_REJIM==0){
			if(flag_F1==1){time_uslugi=balance/(stoim_F1*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F1*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F1=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F2==1){time_uslugi=balance/(stoim_F2*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F2*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F2=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F3==1){time_uslugi=balance/(stoim_F3*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F3*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F3=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F4==1){time_uslugi=balance/(stoim_F4*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F4*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F4=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F5==1){time_uslugi=balance/(stoim_F5*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F5*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F5=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F6==1){time_uslugi=balance/(stoim_F6*0.0167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F6*0.0167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F6=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
		}else{
			if(flag_F1==1){time_uslugi=balance/(stoim_F1*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F1*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F1=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F2==1){time_uslugi=balance/(stoim_F2*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F2*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F2=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F3==1){time_uslugi=balance/(stoim_F3*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F3*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F3=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F4==1){time_uslugi=balance/(stoim_F4*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F4*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F4=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F5==1){time_uslugi=balance/(stoim_F5*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F5*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F5=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};
			if(flag_F6==1){time_uslugi=balance/(stoim_F6*0.00167); time_uslugi_min=time_uslugi/60; time_uslugi_sec=time_uslugi-(60*time_uslugi_min); time_uslugi=time_uslugi_min+(time_uslugi_sec*0.01); balance=balance-(stoim_F6*0.00167); if(balance<0.01){flag_time_indication=0; balance=0; flag_stop=1; flag_F6=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);};};

		};


		if(flag_F1==1){
			if(F1_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F1_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F1_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F1_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F1_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F1_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F1_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F1_MotPow);
		};

		if(flag_F2==1){
			if(F2_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F2_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F2_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F2_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F2_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F2_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F2_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F2_MotPow);
		};

		if(flag_F3==1){
			if(F3_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F3_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F3_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F3_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F3_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F3_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F3_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F3_MotPow);
		};

		if(flag_F4==1){
			if(F4_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F4_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F4_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F4_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F4_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F4_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F4_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F4_MotPow);
		};

		if(flag_F5==1){
			if(F5_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F5_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F5_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F5_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F5_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F5_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F5_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F5_MotPow);
		};

		if(flag_F6==1){
			if(F6_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
			delay_us(10);
			if(F6_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
			delay_us(10);
			if(F6_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
			delay_us(10);
			if(F6_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
			delay_us(10);
			if(F6_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
			delay_us(10);
			if(F6_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
			delay_us(10);
			if(F6_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
			delay_us(10);

			usart2_tx_data(150+F6_MotPow);
		};




	}else{  hc595_led(STOP_on); flag_time_indication=0; if(vremia_pauzi_temp>0){vremia_pauzi_temp--;}else{balance=balance-(stoim_F1*0.017);}; if(balance<0.10){balance=0; NVIC_DisableIRQ (EXTI0_IRQn); NVIC_DisableIRQ (EXTI1_IRQn); Rele_AtoG_OFF(); delay_ms(500); NVIC_EnableIRQ (EXTI0_IRQn); NVIC_EnableIRQ (EXTI1_IRQn);}; };

	if(balance==0){
			if(vremia_pauzi_temp==0){vremia_pauzi_temp=vremia_pauzi;};
			Rele_AtoG_OFF();
			usart2_tx_data(150+0);

			SUPER_REJIM=0;

			ind_vr_pauzi=0;
		};



};


		if(balance==0){
				if(vremia_pauzi_temp==0){vremia_pauzi_temp=vremia_pauzi;};
				Rele_AtoG_OFF();
				usart2_tx_data(150+0);

				SUPER_REJIM=0;

				ind_vr_pauzi=0;
			};



flag_indication=1;
};

void TIM4_IRQHandler (void) // обработчик прерывания по таймеру 4
{
TIM4->SR &=~ TIM_SR_UIF; //очистим флаг вызова прерывания

count_svet_eff++;
if(count_svet_eff==12){count_svet_eff=0;};

switch(count_svet_eff)
	{
		case 0: hc595_led(STOP_on); IWDG->KR = 0xAAAA; break;
		case 1: hc595_led(F1_on); IWDG->KR = 0xAAAA; break;
		case 2: hc595_led(F2_on); IWDG->KR = 0xAAAA; break;
		case 3: hc595_led(F3_on); IWDG->KR = 0xAAAA; break;
		case 4: hc595_led(F4_on); IWDG->KR = 0xAAAA; break;
		case 5: hc595_led(F5_on); IWDG->KR = 0xAAAA; break;
		case 6: hc595_led(F6_on); IWDG->KR = 0xAAAA; break;
		case 7: hc595_led(F5_on); IWDG->KR = 0xAAAA; break;
		case 8: hc595_led(F4_on); IWDG->KR = 0xAAAA; break;
		case 9: hc595_led(F3_on); IWDG->KR = 0xAAAA; break;
		case 10: hc595_led(F2_on); IWDG->KR = 0xAAAA; break;
		case 11: hc595_led(F1_on); IWDG->KR = 0xAAAA; break;


	};

flag_zavershenia_prer_t4=0;


};



int main(void)
{

	pll_init (); // 11.0592MHz

	init_gpio();

	buzer_off;






	led_7_segm_indication_4_razr(8888);
	led_7_segm_indication_4_razr(8888);

	balance=0;



	hc595_led(7);


	lcd_init(); // wh1602b
	lcd_clear(); // очистим дисплей

	i2c_soft_init();

	/*
								eeprom_write_32bit_ulong(0, 5); //Сумма общая
					    		delay_ms(5);
					    		eeprom_write_32bit_ulong(4, 1); //Сумма суточная
					    		delay_ms(5);
					    		eeprom_write_32bit_float(8, 4); //Тариф F1
					    		delay_ms(5);
					    		eeprom_write_32bit_float(12, 4); //Тариф F2
					    		delay_ms(5);
					    		eeprom_write_32bit_float(16, 4); //Тариф F3
					    		delay_ms(5);
					    		eeprom_write_32bit_float(20, 4); //Тариф F4
					    		delay_ms(5);
					    		eeprom_write_32bit_float(24, 4); //Тариф F5
					    		delay_ms(5);
					    		eeprom_write_32bit_float(28, 4); //Тариф F6
					    		delay_ms(5);
					    		eeprom_write_32bit_float(32, 5); //Цена купюры
					    		delay_ms(5);
					    		eeprom_write_32bit_float(36, 1); //Цена монеты
					    		delay_ms(5);
					    		eeprom_write_32bit_ulong(40, 180); //Время паузы
					    		delay_ms(5);
					    		eeprom_write_32bit_ulong(44, 0); //Моточасы - в секундах
					    		delay_ms(5);

					    		i2c_24cxx_write_byte(100, 0x01); //F1_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(101, 0x02); //F2_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(102, 0x04); //F3_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(103, 0x08); //F4_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(104, 0x10); //F5_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(105, 0x20); //F6_rele
					    		delay_ms(5);

					    		i2c_24cxx_write_byte(106, 100); //F1_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(107, 80); //F2_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(108, 0); //F3_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(109, 0); //F4_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(110, 0); //F5_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(111, 0); //F6_MotPow
					    		delay_ms(5);


*/




		//перед выходом в бесконечный цикл прочитаем из еепром-а значения переменных
		summa_obshiya=eeprom_read_32bit_ulong(0);
		delay_ms(10);
		summa_sutochnaya=eeprom_read_32bit_ulong(4);
		delay_ms(10);
		stoim_F1=eeprom_read_32bit_float(8);
		delay_ms(10);
		stoim_F2=eeprom_read_32bit_float(12);
		delay_ms(10);
		stoim_F3=eeprom_read_32bit_float(16);
		delay_ms(10);
		stoim_F4=eeprom_read_32bit_float(20);
		delay_ms(10);
		stoim_F5=eeprom_read_32bit_float(24);
		delay_ms(10);
		stoim_F6=eeprom_read_32bit_float(28);
		delay_ms(10);
		cena_kupyri=eeprom_read_32bit_float(32);
		delay_ms(10);
		cena_moneti=eeprom_read_32bit_float(36);
		delay_ms(10);
		vremia_pauzi=eeprom_read_32bit_ulong(40);
		delay_ms(10);

		moto_sec=eeprom_read_32bit_ulong(44);
		delay_ms(10);
		if(moto_sec>14500000){eeprom_write_32bit_ulong(72, 0); delay_ms(5); moto_sec=0;};

		F1_rele=i2c_24cxx_read_byte(100); //F1_rele
		delay_ms(5);
		F2_rele=i2c_24cxx_read_byte(101); //F2_rele
		delay_ms(5);
		F3_rele=i2c_24cxx_read_byte(102); //F3_rele
		delay_ms(5);
		F4_rele=i2c_24cxx_read_byte(103); //F4_rele
		delay_ms(5);
		F5_rele=i2c_24cxx_read_byte(104); //F5_rele
		delay_ms(5);
		F6_rele=i2c_24cxx_read_byte(105); //F6_rele
		delay_ms(5);

		F1_MotPow=i2c_24cxx_read_byte(106); //F1_MotPow
		delay_ms(5);
		F2_MotPow=i2c_24cxx_read_byte(107); //F2_MotPow
		delay_ms(5);
		F3_MotPow=i2c_24cxx_read_byte(108); //F3_MotPow
		delay_ms(5);
		F4_MotPow=i2c_24cxx_read_byte(109); //F4_MotPow
		delay_ms(5);
		F5_MotPow=i2c_24cxx_read_byte(110); //F5_MotPow
		delay_ms(5);
		F6_MotPow=i2c_24cxx_read_byte(111); //F6_MotPow
		delay_ms(5);





		////////// div 32 - 3.3s /////
		IWDG->KR = 0x5555; // разрешаю доступ
		IWDG->PR = 0x0003; // div32
		IWDG->KR = 0x5555; // разрешаю доступ
		IWDG->KR = 0xAAAA; //релоад

		IWDG->KR = 0xCCCC; // запускаю собаку

	IWDG->KR = 0xAAAA; // refresh IWDG

	//lcd_gotoxy(0, 1); sprintf(lcd_buff,"lithium.com.ua"); lcd_puts(lcd_buff);
	lcd_gotoxy(1, 1); sprintf(lcd_buff,"Loading"); lcd_puts(lcd_buff);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 8); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(STOP_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 9); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F1_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 10); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F2_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 11); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F3_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 12); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F4_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 13); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F5_on);
	delay_ms(400);
	IWDG->KR = 0xAAAA; // refresh IWDG
	lcd_gotoxy(1, 14); sprintf(lcd_buff,"."); lcd_puts(lcd_buff);
	hc595_led(F6_on);



	//eeprom_write_32bit_ulong(0,1234);



	delay_ms(1000);

	led_7_segm_indication_4_razr(0);
	hc595_led(STOP_on);

	IWDG->KR = 0xAAAA; // refresh IWDG

	lcd_clear(); // очистим дисплей
	lcd_gotoxy(0, 1); sprintf(lcd_buff,"Working..."); lcd_puts(lcd_buff);

	buzer_on;
	delay_ms(5);
	buzer_off;

	IWDG->KR = 0xAAAA; // refresh IWDG

	Rele_AtoG_OFF();

	usart2_tx_data(150+0);

	vremia_pauzi_temp=vremia_pauzi; //скопируем значение переменной паузы, для дальнейшей работы с нею

	//Разрешаем оба прерывания от купюро и монетоприемника
	NVIC_EnableIRQ (EXTI0_IRQn);
	NVIC_EnableIRQ (EXTI1_IRQn);

	flag_stop=1;

    while(1)
    {

    	IWDG->KR = 0xAAAA; // refresh IWDG

    	    	// если баланс пользователя положителен, тогда предоставляем ему услуги

    	    	if(balance>0){

    	    		if(flag_zavershenia_prer_t3==0){
    	    			flag_zavershenia_prer_t3=1;
    	    			tim3_interrupt();
    	    										};

    						//flag_indication=1;
    						if((GPIOA->IDR & GPIO_IDR_IDR7)==0){//стоп
    							delay_ms(50);
    							if((GPIOA->IDR & GPIO_IDR_IDR7)==0){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    								ind_vr_pauzi=1;

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    									flag_stop=1;
    									flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;

    									Rele_AtoG_OFF();

        								usart2_tx_data(150+0);

    									delay_ms(100); //задержка на переходные процессы коммутации
    									NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    									NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    							};




    						};

    						if((GPIOB->IDR & GPIO_IDR_IDR0)==0){ //F1
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR0)==0){

    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;

    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F1_on);

    							if(flag_F1==0){ flag_F1=1;
    								if(F1_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
    								delay_us(10);
    								if(F1_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
    								delay_us(10);
    								if(F1_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
    								delay_us(10);
    								if(F1_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
    								delay_us(10);
    								if(F1_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
    								delay_us(10);
    								if(F1_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
    								delay_us(10);
    								if(F1_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
    								delay_us(10);

    								usart2_tx_data(150+F1_MotPow);

    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////

    							};

    						if((GPIOB->IDR & GPIO_IDR_IDR1)==0){ //F2
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR1)==0){



    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;
    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F2_on);

    							if(flag_F2==0){ flag_F2=1;
									if(F2_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
									delay_us(10);
									if(F2_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
									delay_us(10);
									if(F2_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
									delay_us(10);
									if(F2_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
									delay_us(10);
									if(F2_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
									delay_us(10);
									if(F2_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
									delay_us(10);
									if(F2_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
									delay_us(10);

									usart2_tx_data(150+F2_MotPow);
    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////
    							};

    						if((GPIOB->IDR & GPIO_IDR_IDR2)==0){ //F3
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR2)==0){

    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;
    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F3_on);

    							if(flag_F3==0){ flag_F3=1;
									if(F3_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
									delay_us(10);
									if(F3_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
									delay_us(10);
									if(F3_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
									delay_us(10);
									if(F3_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
									delay_us(10);
									if(F3_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
									delay_us(10);
									if(F3_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
									delay_us(10);
									if(F3_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
									delay_us(10);

									usart2_tx_data(150+F3_MotPow);
    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////
    							};

    						if((GPIOB->IDR & GPIO_IDR_IDR14)==0){ //F4
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR14)==0){

    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;
    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F4_on);

    							if(flag_F4==0){ flag_F4=1;
									if(F4_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
									delay_us(10);
									if(F4_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
									delay_us(10);
									if(F4_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
									delay_us(10);
									if(F4_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
									delay_us(10);
									if(F4_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
									delay_us(10);
									if(F4_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
									delay_us(10);
									if(F4_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
									delay_us(10);

									usart2_tx_data(150+F4_MotPow);
    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////
    							};

    						if((GPIOB->IDR & GPIO_IDR_IDR13)==0){ //F5
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR13)==0){

    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;
    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F5_on);

    							if(flag_F5==0){ flag_F5=1;
									if(F5_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
									delay_us(10);
									if(F5_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
									delay_us(10);
									if(F5_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
									delay_us(10);
									if(F5_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
									delay_us(10);
									if(F5_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
									delay_us(10);
									if(F5_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
									delay_us(10);
									if(F5_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
									delay_us(10);

								usart2_tx_data(150+F5_MotPow);
    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////
    							};

    						if((GPIOB->IDR & GPIO_IDR_IDR12)==0){ //F6
    							delay_ms(10);

    							if((GPIOB->IDR & GPIO_IDR_IDR12)==0){

    								/////////При включении функции происходит предварительный СТОП//////////
    								flag_stop=1;
    								flag_F1=0; flag_F2=0; flag_F3=0; flag_F4=0; flag_F5=0; flag_F6=0;
    								////////////////////////////////////////////////////////////////////////

    							if (flag_stop==1){
    								NVIC_DisableIRQ (EXTI0_IRQn); // Отключим прерывание купюроприемника
    								NVIC_DisableIRQ (EXTI1_IRQn); // Отключим прерывание монетоприемника

    		    	    			buzer_on;
    		    	    			delay_ms(50);
    		    	    			buzer_off;

    							flag_stop=0;

    							//TIM3->CNT = 0;
    							TIM3->SR |= TIM_SR_UIF; //установим флаг вызова прерывания TIM3

    							hc595_led(F6_on);

    							if(flag_F6==0){ flag_F6=1;
									if(F6_rele & 0x01){usart2_tx_data(0x61);}else{usart2_tx_data(0x30);};
									delay_us(10);
									if(F6_rele & 0x02){usart2_tx_data(0x62);}else{usart2_tx_data(0x31);};
									delay_us(10);
									if(F6_rele & 0x04){usart2_tx_data(0x63);}else{usart2_tx_data(0x32);};
									delay_us(10);
									if(F6_rele & 0x08){usart2_tx_data(0x64);}else{usart2_tx_data(0x33);};
									delay_us(10);
									if(F6_rele & 0x10){usart2_tx_data(0x65);}else{usart2_tx_data(0x34);};
									delay_us(10);
									if(F6_rele & 0x20){usart2_tx_data(0x66);}else{usart2_tx_data(0x35);};
									delay_us(10);
									if(F6_rele & 0x40){usart2_tx_data(0x67);}else{usart2_tx_data(0x36);};
									delay_us(10);

									usart2_tx_data(150+F6_MotPow);
    							}; //

    							delay_ms(100); //задержка на переходные процессы коммутации
    							NVIC_EnableIRQ (EXTI0_IRQn); // Включим прерывание купюроприемника
    							NVIC_EnableIRQ (EXTI1_IRQn); // Включим прерывание монетоприемника
    											};

    							};

    							////////////Ждем отпускания кнопки/////////////////
    							while((GPIOB->IDR & GPIO_IDR_IDR0)==0){
    								buzer_off;
    								IWDG->KR = 0xAAAA; // refresh IWDG
    								};
    							///////////////////////////////////////////////////
    							};








    	    	}  else{

    	    		ind_vr_pauzi=0;





    	    		if(flag_zavershenia_prer_t4==0){
    	    		   flag_zavershenia_prer_t4=1;
    	    		   tim4_interrupt();
    	    		};
    	    		//

/*
    	    		hc595_led(STOP_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F1_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F2_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F3_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F4_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F5_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F6_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F5_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F4_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F3_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F2_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG

    	    		hc595_led(F1_on);
    	    		delay_ms(200);
    	    		IWDG->KR = 0xAAAA; // refresh IWDG
    	    		*/

    	    		SUPER_REJIM=0;





    	    			flag_stop=1;



    	    			 // Отключим прерывание купюроприемника
    	    			 // Отключим прерывание монетоприемника


    	    		if((flag_na_zapis_summ==1)&(balance==0)){
    	    		flag_na_zapis_summ=0;
    	    		eeprom_write_32bit_ulong(0, summa_obshiya);
    	    		delay_ms(5);
    	    		eeprom_write_32bit_ulong(4, summa_sutochnaya);
    	    		delay_ms(5);
    	    		eeprom_write_32bit_ulong(44, moto_sec); //моточасы (в секундах)
    	    		delay_ms(5);
    	    		};
    	    	};



    	    	//если кнопка нажата, то переходим в системный режим и находимся в нем до размыкания кнопки.
    	    	while((GPIOB->IDR & GPIO_IDR_IDR15)==0){ //PC0

    	    		IWDG->KR = 0xAAAA; // refresh IWDG






    	    		flag_indication=1; // что бы при выходе из этого режима прорисовать дисплей рабочего режима

    	    		if((GPIOA->IDR & GPIO_IDR_IDR7)==0){ //PA7 "СТОП"

    	    			buzer_on;
    	    			delay_ms(50);
    	    			buzer_off;

    	    		s_flag_indication=1;
    	    			switch(s_vibor) //case 3...8 - это 6 функций
    	    				{
    	    					case 2: summa_sutochnaya=0; flag_na_zapis_eeprom=1; break;
    	    					case 3: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    					case 4: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    					case 5: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    					case 6: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    					case 7: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    					case 8: F_vibor++; if(F_vibor==9){F_vibor=0;}; break;
    	    				};
    	    			delay_ms(250);

    	    		};

    	    		if((GPIOB->IDR & GPIO_IDR_IDR0)==0){ //PB0 "вверх"

    	    			buzer_on;
    	    			delay_ms(50);
    	    			buzer_off;

    	    			F_vibor=0;

    	    			s_vibor++; if(s_vibor==13){s_vibor=1;}; s_flag_indication=1;

    	    			delay_ms(250);

    	    		};


    	    		if((GPIOB->IDR & GPIO_IDR_IDR1)==0){ //PB1 "вниз"

    	    			buzer_on;
    	    			delay_ms(50);
    	    			buzer_off;

    	    			F_vibor=0;

    	    			s_vibor--; if(s_vibor==0){s_vibor=12;}; s_flag_indication=1;

    	    			delay_ms(250);

    	    		};

    	    		if((GPIOB->IDR & GPIO_IDR_IDR2)==0){ //PB2 "+"

    	    			buzer_on;
    	    			delay_ms(50);
    	    			buzer_off;

    	    		s_flag_indication=1;
    	    		flag_na_zapis_eeprom=1; //взведем бит запроса на запись в еепром
    					switch(s_vibor)
    					{
    						case 3:
    							if(F_vibor==0){stoim_F1=stoim_F1+0.1;};
    							if(F_vibor==1){if(F1_rele & 0x01){F1_rele = F1_rele &=~ 0x01;}else{F1_rele = F1_rele |= 0x01;};};
    							if(F_vibor==2){if(F1_rele & 0x02){F1_rele = F1_rele &=~ 0x02;}else{F1_rele = F1_rele |= 0x02;};};
    							if(F_vibor==3){if(F1_rele & 0x04){F1_rele = F1_rele &=~ 0x04;}else{F1_rele = F1_rele |= 0x04;};};
    							if(F_vibor==4){if(F1_rele & 0x08){F1_rele = F1_rele &=~ 0x08;}else{F1_rele = F1_rele |= 0x08;};};
    							if(F_vibor==5){if(F1_rele & 0x10){F1_rele = F1_rele &=~ 0x10;}else{F1_rele = F1_rele |= 0x10;};};
    							if(F_vibor==6){if(F1_rele & 0x20){F1_rele = F1_rele &=~ 0x20;}else{F1_rele = F1_rele |= 0x20;};};
    							if(F_vibor==7){if(F1_rele & 0x40){F1_rele = F1_rele &=~ 0x40;}else{F1_rele = F1_rele |= 0x40;};};
    							if(F_vibor==8){F1_MotPow=F1_MotPow+1; if(F1_MotPow>100){F1_MotPow=100;};};
    							break;

    						case 4:
    							if(F_vibor==0){stoim_F2=stoim_F2+0.1;};
    							if(F_vibor==1){if(F2_rele & 0x01){F2_rele = F2_rele &=~ 0x01;}else{F2_rele = F2_rele |= 0x01;};};
    							if(F_vibor==2){if(F2_rele & 0x02){F2_rele = F2_rele &=~ 0x02;}else{F2_rele = F2_rele |= 0x02;};};
    							if(F_vibor==3){if(F2_rele & 0x04){F2_rele = F2_rele &=~ 0x04;}else{F2_rele = F2_rele |= 0x04;};};
    							if(F_vibor==4){if(F2_rele & 0x08){F2_rele = F2_rele &=~ 0x08;}else{F2_rele = F2_rele |= 0x08;};};
    							if(F_vibor==5){if(F2_rele & 0x10){F2_rele = F2_rele &=~ 0x10;}else{F2_rele = F2_rele |= 0x10;};};
    							if(F_vibor==6){if(F2_rele & 0x20){F2_rele = F2_rele &=~ 0x20;}else{F2_rele = F2_rele |= 0x20;};};
    							if(F_vibor==7){if(F2_rele & 0x40){F2_rele = F2_rele &=~ 0x40;}else{F2_rele = F2_rele |= 0x40;};};
    							if(F_vibor==8){F2_MotPow=F2_MotPow+1;};
    							break;

    						case 5:
    							if(F_vibor==0){stoim_F3=stoim_F3+0.1;};
    							if(F_vibor==1){if(F3_rele & 0x01){F3_rele = F3_rele &=~ 0x01;}else{F3_rele = F3_rele |= 0x01;};};
    							if(F_vibor==2){if(F3_rele & 0x02){F3_rele = F3_rele &=~ 0x02;}else{F3_rele = F3_rele |= 0x02;};};
    							if(F_vibor==3){if(F3_rele & 0x04){F3_rele = F3_rele &=~ 0x04;}else{F3_rele = F3_rele |= 0x04;};};
    							if(F_vibor==4){if(F3_rele & 0x08){F3_rele = F3_rele &=~ 0x08;}else{F3_rele = F3_rele |= 0x08;};};
    							if(F_vibor==5){if(F3_rele & 0x10){F3_rele = F3_rele &=~ 0x10;}else{F3_rele = F3_rele |= 0x10;};};
    							if(F_vibor==6){if(F3_rele & 0x20){F3_rele = F3_rele &=~ 0x20;}else{F3_rele = F3_rele |= 0x20;};};
    							if(F_vibor==7){if(F3_rele & 0x40){F3_rele = F3_rele &=~ 0x40;}else{F3_rele = F3_rele |= 0x40;};};
    							if(F_vibor==8){F3_MotPow=F3_MotPow+1;};
    							break;

    						case 6:
    							if(F_vibor==0){stoim_F4=stoim_F4+0.1;};
    							if(F_vibor==1){if(F4_rele & 0x01){F4_rele = F4_rele &=~ 0x01;}else{F4_rele = F4_rele |= 0x01;};};
    							if(F_vibor==2){if(F4_rele & 0x02){F4_rele = F4_rele &=~ 0x02;}else{F4_rele = F4_rele |= 0x02;};};
    							if(F_vibor==3){if(F4_rele & 0x04){F4_rele = F4_rele &=~ 0x04;}else{F4_rele = F4_rele |= 0x04;};};
    							if(F_vibor==4){if(F4_rele & 0x08){F4_rele = F4_rele &=~ 0x08;}else{F4_rele = F4_rele |= 0x08;};};
    							if(F_vibor==5){if(F4_rele & 0x10){F4_rele = F4_rele &=~ 0x10;}else{F4_rele = F4_rele |= 0x10;};};
    							if(F_vibor==6){if(F4_rele & 0x20){F4_rele = F4_rele &=~ 0x20;}else{F4_rele = F4_rele |= 0x20;};};
    							if(F_vibor==7){if(F4_rele & 0x40){F4_rele = F4_rele &=~ 0x40;}else{F4_rele = F4_rele |= 0x40;};};
    							if(F_vibor==8){F4_MotPow=F4_MotPow+1;};
    							break;

    						case 7:
    							if(F_vibor==0){stoim_F5=stoim_F5+0.1;};
    							if(F_vibor==1){if(F5_rele & 0x01){F5_rele = F5_rele &=~ 0x01;}else{F5_rele = F5_rele |= 0x01;};};
    							if(F_vibor==2){if(F5_rele & 0x02){F5_rele = F5_rele &=~ 0x02;}else{F5_rele = F5_rele |= 0x02;};};
    							if(F_vibor==3){if(F5_rele & 0x04){F5_rele = F5_rele &=~ 0x04;}else{F5_rele = F5_rele |= 0x04;};};
    							if(F_vibor==4){if(F5_rele & 0x08){F5_rele = F5_rele &=~ 0x08;}else{F5_rele = F5_rele |= 0x08;};};
    							if(F_vibor==5){if(F5_rele & 0x10){F5_rele = F5_rele &=~ 0x10;}else{F5_rele = F5_rele |= 0x10;};};
    							if(F_vibor==6){if(F5_rele & 0x20){F5_rele = F5_rele &=~ 0x20;}else{F5_rele = F5_rele |= 0x20;};};
    							if(F_vibor==7){if(F5_rele & 0x40){F5_rele = F5_rele &=~ 0x40;}else{F5_rele = F5_rele |= 0x40;};};
    							if(F_vibor==8){F5_MotPow=F5_MotPow+1;};
    							break;

    						case 8:
    							if(F_vibor==0){stoim_F6=stoim_F6+0.1;};
    							if(F_vibor==1){if(F6_rele & 0x01){F6_rele = F6_rele &=~ 0x01;}else{F6_rele = F1_rele |= 0x01;};};
    							if(F_vibor==2){if(F6_rele & 0x02){F6_rele = F6_rele &=~ 0x02;}else{F6_rele = F1_rele |= 0x02;};};
    							if(F_vibor==3){if(F6_rele & 0x04){F6_rele = F6_rele &=~ 0x04;}else{F6_rele = F1_rele |= 0x04;};};
    							if(F_vibor==4){if(F6_rele & 0x08){F6_rele = F6_rele &=~ 0x08;}else{F6_rele = F1_rele |= 0x08;};};
    							if(F_vibor==5){if(F6_rele & 0x10){F6_rele = F6_rele &=~ 0x10;}else{F6_rele = F1_rele |= 0x10;};};
    							if(F_vibor==6){if(F6_rele & 0x20){F6_rele = F6_rele &=~ 0x20;}else{F6_rele = F1_rele |= 0x20;};};
    							if(F_vibor==7){if(F6_rele & 0x40){F6_rele = F6_rele &=~ 0x40;}else{F6_rele = F1_rele |= 0x40;};};
    							if(F_vibor==8){F6_MotPow=F6_MotPow+1;};
    							break;

    						case 9: cena_kupyri++; break;
    						case 10: cena_moneti=cena_moneti+0.25; break;
    						case 11: vremia_pauzi++; break;
    					};

    	    			delay_ms(200);
    	    		};

    	    		if((GPIOB->IDR & GPIO_IDR_IDR14)==0){ //PB14 "-"

    	    			buzer_on;
    	    			delay_ms(50);
    	    			buzer_off;

    	    		s_flag_indication=1;
    	    		flag_na_zapis_eeprom=1; //взведем бит запроса на запись в еепром
    	    			switch(s_vibor)
    	    			{
    	    				case 3:
    							if(F_vibor==0){stoim_F1=stoim_F1-0.1; if(stoim_F1<0){stoim_F1=0;};};
    							if(F_vibor==1){if(F1_rele & 0x01){F1_rele = F1_rele &=~ 0x01;}else{F1_rele = F1_rele |= 0x01;};};
    							if(F_vibor==2){if(F1_rele & 0x02){F1_rele = F1_rele &=~ 0x02;}else{F1_rele = F1_rele |= 0x02;};};
    							if(F_vibor==3){if(F1_rele & 0x04){F1_rele = F1_rele &=~ 0x04;}else{F1_rele = F1_rele |= 0x04;};};
    							if(F_vibor==4){if(F1_rele & 0x08){F1_rele = F1_rele &=~ 0x08;}else{F1_rele = F1_rele |= 0x08;};};
    							if(F_vibor==5){if(F1_rele & 0x10){F1_rele = F1_rele &=~ 0x10;}else{F1_rele = F1_rele |= 0x10;};};
    							if(F_vibor==6){if(F1_rele & 0x20){F1_rele = F1_rele &=~ 0x20;}else{F1_rele = F1_rele |= 0x20;};};
    							if(F_vibor==7){if(F1_rele & 0x40){F1_rele = F1_rele &=~ 0x40;}else{F1_rele = F1_rele |= 0x40;};};
    							if(F_vibor==8){F1_MotPow=F1_MotPow-1; if(F1_MotPow>200){F1_MotPow=0;};};
    							break;

    	    				case 4:
    							if(F_vibor==0){stoim_F2=stoim_F2-0.1; if(stoim_F2<0){stoim_F2=0;};};
    							if(F_vibor==1){if(F2_rele & 0x01){F2_rele = F2_rele &=~ 0x01;}else{F2_rele = F2_rele |= 0x01;};};
    							if(F_vibor==2){if(F2_rele & 0x02){F2_rele = F2_rele &=~ 0x02;}else{F2_rele = F2_rele |= 0x02;};};
    							if(F_vibor==3){if(F2_rele & 0x04){F2_rele = F2_rele &=~ 0x04;}else{F2_rele = F2_rele |= 0x04;};};
    							if(F_vibor==4){if(F2_rele & 0x08){F2_rele = F2_rele &=~ 0x08;}else{F2_rele = F2_rele |= 0x08;};};
    							if(F_vibor==5){if(F2_rele & 0x10){F2_rele = F2_rele &=~ 0x10;}else{F2_rele = F2_rele |= 0x10;};};
    							if(F_vibor==6){if(F2_rele & 0x20){F2_rele = F2_rele &=~ 0x20;}else{F2_rele = F2_rele |= 0x20;};};
    							if(F_vibor==7){if(F2_rele & 0x40){F2_rele = F2_rele &=~ 0x40;}else{F2_rele = F2_rele |= 0x40;};};
    							if(F_vibor==8){F2_MotPow=F2_MotPow-1; if(F2_MotPow>200){F2_MotPow=0;};};
    							break;

    	    				case 5:
    							if(F_vibor==0){stoim_F3=stoim_F3-0.1; if(stoim_F3<0){stoim_F3=0;};};
    							if(F_vibor==1){if(F3_rele & 0x01){F3_rele = F3_rele &=~ 0x01;}else{F3_rele = F3_rele |= 0x01;};};
    							if(F_vibor==2){if(F3_rele & 0x02){F3_rele = F3_rele &=~ 0x02;}else{F3_rele = F3_rele |= 0x02;};};
    							if(F_vibor==3){if(F3_rele & 0x04){F3_rele = F3_rele &=~ 0x04;}else{F3_rele = F3_rele |= 0x04;};};
    							if(F_vibor==4){if(F3_rele & 0x08){F3_rele = F3_rele &=~ 0x08;}else{F3_rele = F3_rele |= 0x08;};};
    							if(F_vibor==5){if(F3_rele & 0x10){F3_rele = F3_rele &=~ 0x10;}else{F3_rele = F3_rele |= 0x10;};};
    							if(F_vibor==6){if(F3_rele & 0x20){F3_rele = F3_rele &=~ 0x20;}else{F3_rele = F3_rele |= 0x20;};};
    							if(F_vibor==7){if(F3_rele & 0x40){F3_rele = F3_rele &=~ 0x40;}else{F3_rele = F3_rele |= 0x40;};};
    							if(F_vibor==8){F3_MotPow=F3_MotPow-1; if(F3_MotPow>200){F3_MotPow=0;};};
    							break;

    	    				case 6:
    							if(F_vibor==0){stoim_F4=stoim_F4-0.1; if(stoim_F4<0){stoim_F4=0;};};
    							if(F_vibor==1){if(F4_rele & 0x01){F4_rele = F4_rele &=~ 0x01;}else{F4_rele = F4_rele |= 0x01;};};
    							if(F_vibor==2){if(F4_rele & 0x02){F4_rele = F4_rele &=~ 0x02;}else{F4_rele = F4_rele |= 0x02;};};
    							if(F_vibor==3){if(F4_rele & 0x04){F4_rele = F4_rele &=~ 0x04;}else{F4_rele = F4_rele |= 0x04;};};
    							if(F_vibor==4){if(F4_rele & 0x08){F4_rele = F4_rele &=~ 0x08;}else{F4_rele = F4_rele |= 0x08;};};
    							if(F_vibor==5){if(F4_rele & 0x10){F4_rele = F4_rele &=~ 0x10;}else{F4_rele = F4_rele |= 0x10;};};
    							if(F_vibor==6){if(F4_rele & 0x20){F4_rele = F4_rele &=~ 0x20;}else{F4_rele = F4_rele |= 0x20;};};
    							if(F_vibor==7){if(F4_rele & 0x40){F4_rele = F4_rele &=~ 0x40;}else{F4_rele = F4_rele |= 0x40;};};
    							if(F_vibor==8){F4_MotPow=F4_MotPow-1; if(F4_MotPow>200){F4_MotPow=0;};};
    							break;

    	    				case 7:
    							if(F_vibor==0){stoim_F5=stoim_F5-0.1; if(stoim_F5<0){stoim_F5=0;};};
    							if(F_vibor==1){if(F5_rele & 0x01){F5_rele = F5_rele &=~ 0x01;}else{F5_rele = F5_rele |= 0x01;};};
    							if(F_vibor==2){if(F5_rele & 0x02){F5_rele = F5_rele &=~ 0x02;}else{F5_rele = F5_rele |= 0x02;};};
    							if(F_vibor==3){if(F5_rele & 0x04){F5_rele = F5_rele &=~ 0x04;}else{F5_rele = F5_rele |= 0x04;};};
    							if(F_vibor==4){if(F5_rele & 0x08){F5_rele = F5_rele &=~ 0x08;}else{F5_rele = F5_rele |= 0x08;};};
    							if(F_vibor==5){if(F5_rele & 0x10){F5_rele = F5_rele &=~ 0x10;}else{F5_rele = F5_rele |= 0x10;};};
    							if(F_vibor==6){if(F5_rele & 0x20){F5_rele = F5_rele &=~ 0x20;}else{F5_rele = F5_rele |= 0x20;};};
    							if(F_vibor==7){if(F5_rele & 0x40){F5_rele = F5_rele &=~ 0x40;}else{F5_rele = F5_rele |= 0x40;};};
    							if(F_vibor==8){F5_MotPow=F5_MotPow-1; if(F5_MotPow>200){F5_MotPow=0;};};
    							break;

    	    				case 8:
    							if(F_vibor==0){stoim_F6=stoim_F6-0.1; if(stoim_F6<0){stoim_F6=0;};};
    							if(F_vibor==1){if(F6_rele & 0x01){F6_rele = F6_rele &=~ 0x01;}else{F6_rele = F6_rele |= 0x01;};};
    							if(F_vibor==2){if(F6_rele & 0x02){F6_rele = F6_rele &=~ 0x02;}else{F6_rele = F6_rele |= 0x02;};};
    							if(F_vibor==3){if(F6_rele & 0x04){F6_rele = F6_rele &=~ 0x04;}else{F6_rele = F6_rele |= 0x04;};};
    							if(F_vibor==4){if(F6_rele & 0x08){F6_rele = F6_rele &=~ 0x08;}else{F6_rele = F6_rele |= 0x08;};};
    							if(F_vibor==5){if(F6_rele & 0x10){F6_rele = F6_rele &=~ 0x10;}else{F6_rele = F6_rele |= 0x10;};};
    							if(F_vibor==6){if(F6_rele & 0x20){F6_rele = F6_rele &=~ 0x20;}else{F6_rele = F6_rele |= 0x20;};};
    							if(F_vibor==7){if(F6_rele & 0x40){F6_rele = F6_rele &=~ 0x40;}else{F6_rele = F6_rele |= 0x40;};};
    							if(F_vibor==8){F6_MotPow=F6_MotPow-1; if(F6_MotPow>200){F6_MotPow=0;};};
    							break;

    	    				case 9: cena_kupyri--; if(cena_kupyri<=1){cena_kupyri=1;}; break;
    	    				case 10: cena_moneti=cena_moneti-0.25; if(cena_moneti<=0.25){cena_moneti=0.25;}; break;
    	    				case 11: vremia_pauzi--; if(vremia_pauzi<=1){vremia_pauzi=1;}; break;
    	    			};

    	    			delay_ms(200);
    	    		};

    	    		if(s_flag_indication==1){ // если бит на индикацию взведен, то произведем вывод на дисплей системной информации
    	    			s_flag_indication=0;
    	    			switch(s_vibor)
    	    			{
    	    			  case 1 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Sum Obsh"); lcd_puts(lcd_buff); //Сумма общая
    	    			  	  lcd_gotoxy(1, 0); ftoa_print(summa_obshiya, 2); break;

    	    			  case 2 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Sum sut"); lcd_puts(lcd_buff); //Сумма суточная
    	    			  	  lcd_gotoxy(1, 0); ftoa_print(summa_sutochnaya, 2); break;

    	    			  case 3 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F1"); lcd_puts(lcd_buff);
    	    			  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F1, 1);};
    	    			  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F1_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
    	    			  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F1_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;

    	    			  case 4 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F2"); lcd_puts(lcd_buff);
	    			  	  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F2, 1);};
	    			  	  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F2_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F2_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;

    	    			  case 5 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F3"); lcd_puts(lcd_buff);
	    			  	  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F3, 1);};
	    			  	  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F3_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F3_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;

    	    			  case 6 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F4"); lcd_puts(lcd_buff);
	    			  	  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F4, 1);};
	    			  	  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F4_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F4_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;

    	    			  case 7 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F5"); lcd_puts(lcd_buff);
	    			  	  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F5, 1);};
	    			  	  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F5_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F5_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;

    	    			  case 8 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"F6"); lcd_puts(lcd_buff);
	    			  	  	  if(F_vibor==0){sprintf(lcd_buff," cena ye/min:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(stoim_F6, 1);};
	    			  	  	  if(F_vibor==1){sprintf(lcd_buff," Rele_1:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x01){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==2){sprintf(lcd_buff," Rele_2:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x02){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==3){sprintf(lcd_buff," Rele_3:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x04){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==4){sprintf(lcd_buff," Rele_4:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x08){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==5){sprintf(lcd_buff," Rele_5:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x10){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==6){sprintf(lcd_buff," Rele_6:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x20){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==7){sprintf(lcd_buff," Rele_7:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); if(F6_rele & 0x40){sprintf(lcd_buff,"ON");}else{sprintf(lcd_buff,"OFF");}; lcd_puts(lcd_buff);};
	    			  	  	  if(F_vibor==8){sprintf(lcd_buff," MotPow 0..100:"); lcd_puts(lcd_buff); lcd_gotoxy(1, 0); ftoa_print(F6_MotPow, 0); sprintf(lcd_buff," persent"); lcd_puts(lcd_buff);};
    	    			  	  break;


    	    			  case 9 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Cena Kup"); lcd_puts(lcd_buff); //Цена купюры
    	    			  	  lcd_gotoxy(1, 0); ftoa_print(cena_kupyri, 1); break;

    	    			  case 10 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Cena mon"); lcd_puts(lcd_buff); //Цена монеты
    	    			  	  lcd_gotoxy(1, 0); ftoa_print(cena_moneti, 2);	break;

    	    			  case 11 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Vrem Pauzi"); lcd_puts(lcd_buff); //Время паузы
    	    			  	  lcd_gotoxy(1, 0); ftoa_print(vremia_pauzi, 0); break;

    	    			  case 12 : lcd_clear(); lcd_gotoxy(0, 0); sprintf(lcd_buff,"Motor time"); lcd_puts(lcd_buff); //Счетчик моточасов
    	    			      lcd_gotoxy(1, 0); ftoa_print(moto_sec/3600, 0); sprintf(lcd_buff," h "); lcd_puts(lcd_buff); ftoa_print((moto_sec-((moto_sec/3600)*3600))/60, 0); sprintf(lcd_buff," min"); lcd_puts(lcd_buff); break;

    	    			}

    	    			led_7_segm_indication_4_razr(balance); //Обновим информацию на дисплее клиента - на всякий случай :)

    	    		};

    	    		//БЕСПЛ. РЕЖ. "ВОДА" В СЕРВИСЕ //
    	    		//if(s_vibor==1){ if((GPIOB->IDR & GPIO_IDR_IDR14)==0){if(system_poloscanie==0){usart2_tx_data(0x61); delay_ms(500); usart2_tx_data(0x62); system_poloscanie=1; }else{usart2_tx_data(0x31); delay_ms(500); usart2_tx_data(0x30); system_poloscanie=0;}; while(GPIOC->IDR & GPIO_IDR_IDR1){;}; delay_ms(500); };}; //если выбран режим полоскания, тогда можно открыть основной клапан воды (режим "Полоскание")

    	    	};

    	    	if(flag_na_zapis_eeprom==1){
								eeprom_write_32bit_ulong(0, summa_obshiya); //Сумма общая
					    		delay_ms(5);
					    		eeprom_write_32bit_ulong(4, summa_sutochnaya); //Сумма суточная
					    		delay_ms(5);
					    		eeprom_write_32bit_float(8, stoim_F1); //Тариф F1
					    		delay_ms(5);
					    		eeprom_write_32bit_float(12, stoim_F2); //Тариф F2
					    		delay_ms(5);
					    		eeprom_write_32bit_float(16, stoim_F3); //Тариф F3
					    		delay_ms(5);
					    		eeprom_write_32bit_float(20, stoim_F4); //Тариф F4
					    		delay_ms(5);
					    		eeprom_write_32bit_float(24, stoim_F5); //Тариф F5
					    		delay_ms(5);
					    		eeprom_write_32bit_float(28, stoim_F6); //Тариф F6
					    		delay_ms(5);
					    		eeprom_write_32bit_float(32, cena_kupyri); //Цена купюры
					    		delay_ms(5);
					    		eeprom_write_32bit_float(36, cena_moneti); //Цена монеты
					    		delay_ms(5);
					    		eeprom_write_32bit_ulong(40, vremia_pauzi); //Время паузы
					    		delay_ms(5);

					    		i2c_24cxx_write_byte(100, F1_rele); //F1_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(101, F2_rele); //F2_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(102, F3_rele); //F3_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(103, F4_rele); //F4_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(104, F5_rele); //F5_rele
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(105, F6_rele); //F6_rele
					    		delay_ms(5);

					    		i2c_24cxx_write_byte(106, F1_MotPow); //F1_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(107, F2_MotPow); //F2_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(108, F3_MotPow); //F3_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(109, F4_MotPow); //F4_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(110, F5_MotPow); //F5_MotPow
					    		delay_ms(5);
					    		i2c_24cxx_write_byte(111, F6_MotPow); //F6_MotPow
					    		delay_ms(5);
    	    	};

    	    	s_flag_indication=1; s_vibor=1; // "взведем" бит индикации и бит выбора стартового меню системного режима

    	    	//


    	    	if((GPIOA->IDR & GPIO_IDR_IDR8)==0){ // Переключатель СЕРВИС №2

    	    		if(TEST_REJIM==0){TEST_REJIM=1; flag_indication=1;};
    	    		balance=9999; vremia_pauzi_temp=vremia_pauzi;

    	    	}else{ if(TEST_REJIM==1){
    	    		TEST_REJIM=0; balance=0; flag_indication=1; flag_stop=1;

    	    		//Разрешаем оба прерывания от купюро и монетоприемника
    	    		NVIC_EnableIRQ (EXTI0_IRQn);
    	    		NVIC_EnableIRQ (EXTI1_IRQn);

    	    	}; };




    	    	if(flag_indication==1){
    	    	    	flag_indication=0;
    	    	    	lcd_clear();

    	    	    	lcd_gotoxy(0, 0);
    	    	    	if(flag_F1==1){sprintf(lcd_buff,"F1"); lcd_puts(lcd_buff); };
    	    	    	if(flag_F2==1){sprintf(lcd_buff,"F2"); lcd_puts(lcd_buff); };
    	    	    	if(flag_F3==1){sprintf(lcd_buff,"F3"); lcd_puts(lcd_buff); };
    	    	    	if(flag_F4==1){sprintf(lcd_buff,"F4"); lcd_puts(lcd_buff); };
    	    	    	if(flag_F5==1){sprintf(lcd_buff,"F5"); lcd_puts(lcd_buff); };
    	    	    	if(flag_F6==1){sprintf(lcd_buff,"F6"); lcd_puts(lcd_buff); };

    	    	    	if(flag_stop==1){sprintf(lcd_buff,"STOP:   "); lcd_puts(lcd_buff); ftoa_print(vremia_pauzi_temp, 0); };



    	    	    	lcd_gotoxy(1, 0);
    	    	    	sprintf(lcd_buff,"Balance: ");
    	    	    	lcd_puts(lcd_buff);





    	    	    	//if((GPIOA->IDR & GPIO_IDR_IDR8)==0){

    	    	    		//if(flag_time_indication==1){led_7_segm_indication_4_razr(time_uslugi);}else{led_7_segm_indication_4_razr(balance);};
    	    	    		//if(flag_time_indication==1){led_7_segm_indication_4_razr(balance);}else{led_7_segm_indication_4_razr(balance);};


    	    	    	//}else{
/*

    	    	    		if(flag_time_indication==1){led_7_segm_indication_4_razr(time_uslugi);}else{if(balance>0){if(ind_vr_pauzi==1){if(vremia_pauzi_temp>0){pauza_7_segm_4_razr(vremia_pauzi_temp);}else{led_7_segm_indication_4_razr(balance);};};}else{led_7_segm_indication_4_razr(balance);};
    	    	    			if(TEST_REJIM==1){led_7_segm_indication_4_razr(balance);};
    	    	    		};
*/
    	    	    		if(flag_time_indication==1){led_7_segm_indication_4_razr(balance);}else{if(balance>0){if(ind_vr_pauzi==1){if(vremia_pauzi_temp>0){pauza_7_segm_4_razr(vremia_pauzi_temp);}else{led_7_segm_indication_4_razr(balance);};};}else{led_7_segm_indication_4_razr(balance);};
    	    	    			if(TEST_REJIM==1){led_7_segm_indication_4_razr(balance);};
    	    	    		};


    	    	    	//};

    	    	    	//
    	    	    	ftoa_print(balance, 2);
    	    	    	lcd_gotoxy(1, 16); //переведем курсор в невидимую область видеопамяти
    	    	    	};



    }
}
