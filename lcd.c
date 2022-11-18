#include "lcd.h"
#include "gpio.h"
#include "stm32f10x.h"
#include "stdio.h"
/******************************************************************************/
#define LCD_RS_ON()     SET_PIN(Lcd_RS.GPIOx, Lcd_RS.Pin)
#define LCD_RS_OFF()    CLR_PIN(Lcd_RS.GPIOx, Lcd_RS.Pin)
#define LCD_EN_ON()     SET_PIN(Lcd_EN.GPIOx, Lcd_EN.Pin)
#define LCD_EN_OFF()    CLR_PIN(Lcd_EN.GPIOx, Lcd_EN.Pin)
#define SET_DATA(d)  do{                                                \
    if((d)&1)           SET_PIN(Lcd_Data_4.GPIOx, Lcd_Data_4.Pin);      \
    else                CLR_PIN(Lcd_Data_4.GPIOx, Lcd_Data_4.Pin);      \
    if((d)&2)           SET_PIN(Lcd_Data_5.GPIOx, Lcd_Data_5.Pin);      \
    else                CLR_PIN(Lcd_Data_5.GPIOx, Lcd_Data_5.Pin);      \
    if((d)&4)           SET_PIN(Lcd_Data_6.GPIOx, Lcd_Data_6.Pin);      \
    else                CLR_PIN(Lcd_Data_6.GPIOx, Lcd_Data_6.Pin);      \
    if((d)&8)           SET_PIN(Lcd_Data_7.GPIOx, Lcd_Data_7.Pin);      \
    else                CLR_PIN(Lcd_Data_7.GPIOx, Lcd_Data_7.Pin);      \
}while(0)
/************************************/
void Delay_Us(unsigned int v){
while(v--)
  __asm volatile 
  (
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
      " NOP              \n"
	" NOP              \n"
	" NOP              \n"
	" NOP              \n"
	" NOP              \n"
  );
}
//#define Delay_Us(x)     do{unsigned long i=x*4.8 ; while(i--); }while(0)
#define Delay_Ms(x)     do{unsigned long i=x; while(i--){ Delay_Us(1000);}}while(0)
#define Lcd_Strobe()    do{LCD_EN_ON(); Delay_Us(1); LCD_EN_OFF();}while(0)
/******************************************************************************/
typedef struct{
    GPIO_TypeDef        *GPIOx;
    uint32_t            Pin;
}tLcd_GPIO_Pin;

tLcd_GPIO_Pin   Lcd_RS=GPIO_RS, Lcd_EN=GPIO_EN;
tLcd_GPIO_Pin   Lcd_Data_4=GPIO_DATA_4, Lcd_Data_5=GPIO_DATA_5, Lcd_Data_6=GPIO_DATA_6, Lcd_Data_7=GPIO_DATA_7;
/******************************************************************************/
const unsigned char cyr_table[]={
'A',	// À
160,	// Á
'B',	// Â
161,	// Ã
224,	// Ä
'E',	// Å
//	162,	// ¨
163,	// Æ
164,	// Ç
165,	// È
166,	// É
'K',	// K
167,	// Ë
'M',	// M
'H',	// H
'O',	// O
168,	// Ï
'P',	// P
'C',	// C
'T',	// T
169,	// Ó
170,	// Ô
'X',	// X
225,	// Ö
171,	// ×
172,	// Ø
226,	// Ù
173,	// Ú
174,	// Û
'b',	// Ü
175,	// Ý
176,	// Þ
177,	// ß
'a',	// à
178,	// á
179,	// â
180,	// ã
227,	// ä
'e',	// e
//	181,	// ¸
182,	// æ
183,	// ç
184,	// è
185,	// é
186,	// ê
187,	// ë
188,	// ì
189,	// í
'o',	// o
190,	// ï
'p',	// ð
'c',	// ñ
191,	// ò
'y',	// y
228,	// ô
'x',	// x
229,	// ö
192,	// ÷
193,	// ø
230,	// ù
194,	// ú
195,	// û
196,	// ü
197,	// ý
198,	// þ
199,	// ÿ
};
/*******************************************************/
/*******************************************************/
unsigned char chr_to_cyr (unsigned char ch){
if (ch==168)    return 162;
if (ch==184)    return 181;
if (ch>=192)    return cyr_table[ch-192];
return ch;
}
/*******************************************************/
void lcd_write(unsigned char c){
Delay_Us(250); //50
SET_DATA(c>>4);             // H
Lcd_Strobe();
SET_DATA(c);                // L
Lcd_Strobe();
SET_DATA(0);
}
/*******************************************************/
void lcd_clear(void){
LCD_RS_OFF();
lcd_write(0x01);
Delay_Ms(5);
}
/*******************************************************/
void lcd_puts(char * s){
while(*s)   lcd_putch(*s++);
}
/*******************************************************/
void putch(unsigned char byte){
lcd_putch(byte);
}
/*******************************************************/
void lcd_putch(char c){
LCD_RS_ON();
lcd_write(chr_to_cyr(c));
}
/*******************************************************/
void lcd_gotoxy (char row, char col){
const char LCD_ROW_ADDRESS[]= {
    #if LCD_MODE_1x8
        0x00
    #elif LCD_MODE_1x16_A
        0x00
    #elif LCD_MODE_1x16_B
        0x00,0x40
    #elif LCD_MODE_1x40
        0x00
    #elif LCD_MODE_2x8
        0x00,0x40
    #elif LCD_MODE_2x12
        0x00,0x40
    #elif LCD_MODE_2x16
        0x00,0x40
    #elif LCD_MODE_2x20
        0x00,0x40
    #elif LCD_MODE_2x24
        0x00,0x40
    #elif LCD_MODE_2x40
        0x00,0x40
    #elif LCD_MODE_4x16
        0x00,0x40,0x10,0x50
    #elif LCD_MODE_4x20
        0x00,0x40,0x14,0x54
    #elif LCD_MODE_4x24
        0x00,0x40,0x80,0xc0
    #endif
};
row = LCD_ROW_ADDRESS[row];					
row += col ;
LCD_RS_OFF();
lcd_write(0x80 | row);				
}
/*******************************************************/
void lcd_init(){

PORT_TAKT_ON(Lcd_RS.GPIOx);
CONFIG_PIN_OUT(Lcd_RS.GPIOx, Lcd_RS.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);       // push-pull & general, 50 Mhz
CLR_PIN(Lcd_RS.GPIOx, Lcd_RS.Pin);
PORT_TAKT_ON(Lcd_EN.GPIOx);
CONFIG_PIN_OUT(Lcd_EN.GPIOx, Lcd_EN.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);
CLR_PIN(Lcd_EN.GPIOx, Lcd_EN.Pin);

PORT_TAKT_ON(Lcd_Data_4.GPIOx);
CONFIG_PIN_OUT(Lcd_Data_4.GPIOx, Lcd_Data_4.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);
CLR_PIN(Lcd_Data_4.GPIOx, Lcd_Data_4.Pin);
PORT_TAKT_ON(Lcd_Data_5.GPIOx);
CONFIG_PIN_OUT(Lcd_Data_5.GPIOx, Lcd_Data_5.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);
CLR_PIN(Lcd_Data_5.GPIOx, Lcd_Data_5.Pin);
PORT_TAKT_ON(Lcd_Data_6.GPIOx);
CONFIG_PIN_OUT(Lcd_Data_6.GPIOx, Lcd_Data_6.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);
CLR_PIN(Lcd_Data_6.GPIOx, Lcd_Data_6.Pin);
PORT_TAKT_ON(Lcd_Data_7.GPIOx);
CONFIG_PIN_OUT(Lcd_Data_7.GPIOx, Lcd_Data_7.Pin, OUT_MODE_PP | OUT_MODE_GEN, OUT_50_MHZ);
CLR_PIN(Lcd_Data_7.GPIOx, Lcd_Data_7.Pin);

 
SET_DATA(0x00);
Delay_Ms(50); //15
SET_DATA(0x03);
Lcd_Strobe();
Delay_Ms(25); //10
Lcd_Strobe();
Delay_Us(1000); //200
Lcd_Strobe();
Delay_Us(1000); //200
SET_DATA(0x02);
Lcd_Strobe();
Delay_Us(1000); //200
lcd_write(0x28);        // Set interface length
lcd_write(0x0C);        // Display On, Cursor off, Cursor Blink off
lcd_write(0x06);        // Set entry Mode
}
/*******************************************************/
void lcd_cgram(char *arr, unsigned char adr){
unsigned char i;
adr<<=3;
adr|=0x40;
LCD_RS_OFF();
lcd_write(adr);
LCD_RS_ON();
for(i=0;i<8;i++)    lcd_write(arr[i]);
LCD_RS_OFF();
lcd_write(0x80);
}























