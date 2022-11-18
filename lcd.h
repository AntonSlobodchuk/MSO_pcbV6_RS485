#ifndef _LCD_H_
#define _LCD_H_
/******************************************************************************/

#define GPIO_RS         {GPIOB, 5}
#define GPIO_EN         {GPIOB, 4}
#define GPIO_DATA_4     {GPIOB, 9}
#define GPIO_DATA_5     {GPIOB, 8}
#define GPIO_DATA_6     {GPIOB, 7}
#define GPIO_DATA_7     {GPIOB, 6}

//------------------------- Разрешение экрана ----------------------------//
#define	LCD_MODE_1x8		0	
#define	LCD_MODE_1x16_A		0	
#define	LCD_MODE_1x16_B		0	
#define	LCD_MODE_1x40		0
#define	LCD_MODE_2x8		0
#define	LCD_MODE_2x12		0
#define	LCD_MODE_2x16		1
#define	LCD_MODE_2x20		0
#define	LCD_MODE_2x24		0
#define	LCD_MODE_2x40		0
#define	LCD_MODE_4x16		0
#define	LCD_MODE_4x20		0
#define LCD_MODE_4x24		0
#define	LCD_MODE_4x40		0
//------------------------- Прототипы -------------------------------------//
extern void lcd_init(void);
extern void lcd_clear(void);	
extern void lcd_gotoxy(char, char);
extern void lcd_puts(char *);
extern void lcd_putch(char);
extern void lcd_cgram(char *, unsigned char);
//-------------------------------------------------------------------------//












/******************************************************************************/
#endif  // _LCD_H_
