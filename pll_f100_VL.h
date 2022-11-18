void pll_init (void){

RCC->CR|=RCC_CR_HSEON; // Включить генератор HSE.
while (!(RCC->CR & RCC_CR_HSERDY)) {}; // Ожидание готовности HSE.
RCC->CFGR &=~RCC_CFGR_SW; // Очистить биты SW0, SW1.
RCC->CFGR |= RCC_CFGR_SW_HSE; // Выбрать HSE для тактирования SW0=1.
/*
char PLL_MUL=3; // Коэффициент умножения PLL. 8*3 = 24 MHz

RCC->CFGR2 &=~(RCC_CFGR2_PREDIV1); // Предочистка делителя HSE.
RCC->CFGR2|= RCC_CFGR2_PREDIV1_DIV1; // Делить частоту HSE на 1.
RCC->CFGR &=~((RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMULL)); // Предочистка.
RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1; // Тактировать PLL от HSE/PREDIV1.
RCC->CFGR|=((PLL_MUL - 2)<<18); // Умножать частоту на PLL_MUL.
RCC->CR |= RCC_CR_PLLON; // Запустить PLL.
while ((RCC->CR & RCC_CR_PLLRDY)==0) {} // Ожидание готовности PLL.
RCC->CFGR &=~RCC_CFGR_SW; // Очистить биты SW0, SW1.
RCC->CFGR |= RCC_CFGR_SW_PLL; // Тактирование с выхода PLL.
while ((RCC->CFGR&RCC_CFGR_SWS)!=0x08) {} // Ожидание переключения на PLL.
*/
};
