void pll_init (void){

RCC->CR|=RCC_CR_HSEON; // �������� ��������� HSE.
while (!(RCC->CR & RCC_CR_HSERDY)) {}; // �������� ���������� HSE.
RCC->CFGR &=~RCC_CFGR_SW; // �������� ���� SW0, SW1.
RCC->CFGR |= RCC_CFGR_SW_HSE; // ������� HSE ��� ������������ SW0=1.
/*
char PLL_MUL=3; // ����������� ��������� PLL. 8*3 = 24 MHz

RCC->CFGR2 &=~(RCC_CFGR2_PREDIV1); // ����������� �������� HSE.
RCC->CFGR2|= RCC_CFGR2_PREDIV1_DIV1; // ������ ������� HSE �� 1.
RCC->CFGR &=~((RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMULL)); // �����������.
RCC->CFGR |= RCC_CFGR_PLLSRC_PREDIV1; // ����������� PLL �� HSE/PREDIV1.
RCC->CFGR|=((PLL_MUL - 2)<<18); // �������� ������� �� PLL_MUL.
RCC->CR |= RCC_CR_PLLON; // ��������� PLL.
while ((RCC->CR & RCC_CR_PLLRDY)==0) {} // �������� ���������� PLL.
RCC->CFGR &=~RCC_CFGR_SW; // �������� ���� SW0, SW1.
RCC->CFGR |= RCC_CFGR_SW_PLL; // ������������ � ������ PLL.
while ((RCC->CFGR&RCC_CFGR_SWS)!=0x08) {} // �������� ������������ �� PLL.
*/
};
