
#define i2c_soft_sda_hight	GPIOA->BSRR=GPIO_BSRR_BS5
#define i2c_soft_sda_low	GPIOA->BSRR=GPIO_BSRR_BR5
#define i2c_soft_scl_hight	GPIOA->BSRR=GPIO_BSRR_BS6
#define i2c_soft_scl_low	GPIOA->BSRR=GPIO_BSRR_BR6




void zaderjka(unsigned int us){
	us=us*24; //Умножаем на число Fcpu/2 в МГц
	while(us){us--;};
};

void i2c_soft_start(void){
	i2c_soft_sda_hight;
	zaderjka(1);
	i2c_soft_scl_hight;//
	zaderjka(1);
	i2c_soft_sda_low;
	zaderjka(1);
	i2c_soft_scl_low;
	zaderjka(1);
};

void i2c_soft_stop(void){
	i2c_soft_scl_hight;
	zaderjka(1);
	i2c_soft_sda_hight;
	zaderjka(1);
};

void i2c_soft_init(void){
	i2c_soft_stop();
};

void i2c_soft_ask(void){

	i2c_soft_scl_hight; //отпустили линию SCL
	zaderjka(1);
	i2c_soft_scl_low;
	zaderjka(1);
};

void i2c_soft_noask(void){
	i2c_soft_sda_hight; //отпустили линию SDA
	zaderjka(1);
	i2c_soft_scl_hight; //отпустили линию SCL
	zaderjka(1);
	i2c_soft_scl_low;
	zaderjka(1);
	i2c_soft_sda_low;
	zaderjka(1);
};

void i2c_soft_write_byte(unsigned char data){
	unsigned char i;

	for(i=0;i<8;i++){
		if(data & 0x80){i2c_soft_sda_hight;}else{i2c_soft_sda_low;};
		data=data<<1;
		zaderjka(1);
		i2c_soft_scl_hight; //отпустили линию SCL
		zaderjka(1);
		i2c_soft_scl_low;
		zaderjka(1);

	};

};

unsigned char i2c_soft_read_byte(void){
	unsigned char i, data=0;

	for(i=0;i<8;i++){
		i2c_soft_scl_hight; //отпустили линию SCL
		zaderjka(1);
		data=data<<1;
		if(GPIOA->IDR & GPIO_IDR_IDR5){data=data | 0x01;};
		i2c_soft_scl_low;
		zaderjka(1);
	};

	return data;

};

unsigned char i2c_24cxx_read_byte(unsigned char address){
	unsigned char temp;
	i2c_soft_start();
	i2c_soft_write_byte(0xA0); //addrees 24cxx + write
	i2c_soft_ask();
	i2c_soft_write_byte(address);
	i2c_soft_ask();
	i2c_soft_start(); // Повторный старт
	i2c_soft_write_byte(0xA1); //addrees + read byte
	i2c_soft_ask();
	temp=i2c_soft_read_byte();
	i2c_soft_noask();
	i2c_soft_stop();

	return temp;
};

void i2c_24cxx_write_byte(unsigned char address, unsigned char data){
	i2c_soft_start();
	i2c_soft_write_byte(0xA0); //addrees 24cxx + write
	i2c_soft_ask();
	i2c_soft_write_byte(address);
	i2c_soft_ask();
	i2c_soft_write_byte(data);
	i2c_soft_ask();

	i2c_soft_sda_low;

	i2c_soft_stop();

	zaderjka(1000);
};

void i2c_24cxx_write_u16(unsigned char start_address, unsigned int data){
	unsigned char temp;

	temp= data & 0xff;
	i2c_24cxx_write_byte(start_address, temp);
	data=data>>8;
	temp= data & 0xff;
	i2c_24cxx_write_byte(start_address+1, temp);
};

unsigned int i2c_24cxx_read_u16(unsigned char start_address){
	unsigned char temp;
	unsigned int data;

	temp= i2c_24cxx_read_byte(start_address+1);
	data=temp;
	data=data<<8;
	temp= i2c_24cxx_read_byte(start_address);
	data=data+temp;

	return data;
};


void eeprom_write_32bit_ulong(unsigned char address, unsigned long data_w){

	i2c_24cxx_write_byte(address, data_w & 0xff); // запишем 0-й байт
	i2c_24cxx_write_byte(address+1, (data_w>>8) & 0xff); // запишем 1-й байт
	i2c_24cxx_write_byte(address+2, (data_w>>16) & 0xff); // запишем 2-й байт
	i2c_24cxx_write_byte(address+3, (data_w>>24) & 0xff); // запишем 3-й байт

};

unsigned long eeprom_read_32bit_ulong(unsigned char address){
	unsigned long data_r=0;
	data_r=data_r+(i2c_24cxx_read_byte(address+3)<<24);
	data_r=data_r+(i2c_24cxx_read_byte(address+2)<<16);
	data_r=data_r+(i2c_24cxx_read_byte(address+1)<<8);
	data_r=data_r+i2c_24cxx_read_byte(address);
	return data_r;
};

void eeprom_write_32bit_float(unsigned char address, float data){
	eeprom_write_32bit_ulong(address, (data*100));
};

float eeprom_read_32bit_float(unsigned char address){
	return eeprom_read_32bit_ulong(address)*0.01;
};

