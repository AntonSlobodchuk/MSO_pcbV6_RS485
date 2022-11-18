//сделаем замену для визуального восприятия кода для семисегментного LED индикатора
#define SDI_on;     GPIOA->BSRR|=GPIO_BSRR_BS12
#define SDI_off;     GPIOA->BSRR|=GPIO_BSRR_BR12
#define CLK_on;     GPIOA->BSRR|=GPIO_BSRR_BS15
#define CLK_off;     GPIOA->BSRR|=GPIO_BSRR_BR15
#define LE_on;     GPIOB->BSRR|=GPIO_BSRR_BS3
#define LE_off;     GPIOB->BSRR|=GPIO_BSRR_BR3

const unsigned int led_ind_digit_1_3 [10] = { // + 0x2000 Dp
		0x5807, //0
		0x0804,	//1
		0x9803,//2
		0x9806,//3
		0xc804,//4
		0xd006,//5
		0xd007,//6
		0x1804,//7
		0xd807,//8
		0xd806//9
};

const unsigned int led_ind_digit_2_4 [10] = { // + 0x0100 Dp
		0x02f8, //0
		0x0060,	//1
		0x04d8,//2
		0x04f0,//3
		0x0660,//4
		0x06b0,//5
		0x06b8,//6
		0x00e0,//7
		0x06f8,//8
		0x06f0//9
};

void mbi5026_write(int data){
	unsigned char led_i=0;
	unsigned int led_sdvig=0;


	for(led_i=0; led_i<16; led_i++){
					if(led_sdvig==0){led_sdvig=0x0001;};
					if(data & led_sdvig){SDI_on;}else{SDI_off;};
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					CLK_on;
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					CLK_off;
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					asm("nop");
					led_sdvig=led_sdvig<<1;
											   };


};

void led_7_segm_indication_4_razr(float led_data){
	unsigned char led_i=0, led_sdvig=0, led_digit=0, digit_a, digit_b;
	unsigned int led_data_int, led_temp;



	if(led_data<10){

		led_data_int=led_data*100;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);

		led_data_int=led_data_int/10;

		digit_a=led_data_int%10;

		led_temp=led_ind_digit_2_4[digit_a] + 0x0100; //+Dp

		mbi5026_write(led_temp);

		};

	if((led_data>=10)&(led_data<100)){

		led_data_int=led_data*100;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);

		led_data_int=led_data_int/10;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a] + 0x0100; //+Dp

		mbi5026_write(led_temp);

	};

	if((led_data>=100)&(led_data<1000)){

		led_data_int=led_data*10;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a] +0x2000; //+Dp

		mbi5026_write(led_temp);

		led_data_int=led_data_int/10;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);

				};

	if((led_data>=1000)&(led_data<10000)){

		led_data_int=led_data;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);

		led_data_int=led_data_int/10;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);


					};

	LE_on;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LE_off;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

};


void pauza_7_segm_4_razr(int led_data){
	unsigned char led_i=0, led_sdvig=0, led_digit=0, digit_a, digit_b;
	unsigned int led_data_int, led_temp;




		led_data_int=led_data;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;

		led_temp=led_ind_digit_1_3[digit_b]|led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);

		led_data_int=led_data_int/10;

		digit_a=led_data_int%10;

		led_data_int=led_data_int/10;

		digit_b=led_data_int%10;;

		led_temp= 0x5805 | led_ind_digit_2_4[digit_a];

		mbi5026_write(led_temp);




	LE_on;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LE_off;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");

};

