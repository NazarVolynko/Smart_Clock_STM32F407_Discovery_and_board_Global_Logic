#include "stm32f4xx.h"
#include <stdint.h>
#include "lcd.h"
 
volatile uint32_t second = 00;
volatile uint32_t minute = 30;
volatile uint32_t hour = 12;
volatile uint32_t day = 05;
volatile uint32_t month = 06;
volatile uint32_t check_month = 00;
volatile uint32_t year = 2020;
double  temperature = 00;
volatile uint32_t alarm_h = 12;
volatile uint32_t alarm_m = 20;
volatile uint32_t style = 1;
int adc_result;

volatile uint32_t one_sec = 950;
volatile uint32_t delay_cnt = 0;

void SysTick_Handler(void) { //1ms, створюємо затримку на SysTick для годинника
	
	if(delay_cnt > 0){
		delay_cnt--;
	}
}

void delay__ms(uint32_t delay_temp) {
	
	delay_cnt = delay_temp;
	while(delay_cnt);
}

void delay(volatile uint32_t i){
	while(--i != 0);
    
}

void adc_init(void) //ініциалізуємо АЦП 1 для зчитування напруги з термодатчика
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	ADC_DeInit();
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	GPIO_InitTypeDef portb_setup;                         
	GPIO_StructInit(&portb_setup);
	portb_setup.GPIO_Mode = GPIO_Mode_AN;
	portb_setup.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &portb_setup);
 
	ADC_InitTypeDef adc_setup;
	adc_setup.ADC_ContinuousConvMode = DISABLE;
	adc_setup.ADC_Resolution = ADC_Resolution_12b;
	adc_setup.ADC_ScanConvMode = DISABLE;
	adc_setup.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	adc_setup.ADC_DataAlign = ADC_DataAlign_Right;
 
	ADC_Init(ADC1, &adc_setup);
	ADC_Cmd(ADC1,ENABLE);
 
	GPIO_InitTypeDef port_setup;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	port_setup.GPIO_Mode = GPIO_Mode_OUT;
	port_setup.GPIO_OType = GPIO_OType_PP;
	port_setup.GPIO_PuPd = GPIO_PuPd_DOWN;
	port_setup.GPIO_Pin = GPIO_Pin_All;
	port_setup.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &port_setup);
}

u16 readADC1(u8 channel) // функція для зчитування даних з дев'ятого каналу АЦП
{
  ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_3Cycles);
  ADC_SoftwareStartConv(ADC1);
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
  return ADC_GetConversionValue(ADC1);
}

void port_ini(void)
{
	GPIO_InitTypeDef InitС; //для кнопки
	GPIO_InitTypeDef InitD; //для світлодіодів
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//включим такттування порта D
	
	//світлодіоди
	InitD.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	InitD.GPIO_Mode = GPIO_Mode_OUT;
	InitD.GPIO_OType = GPIO_OType_PP;
	InitD.GPIO_Speed = GPIO_Speed_2MHz;
	InitD.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&InitD); //ініцилізуємо ножки порта

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//включим тактирование порта С

	//кнопка
	InitС.GPIO_Pin = GPIO_Pin_6;
	InitС.GPIO_Mode = GPIO_Mode_IN;
	InitС.GPIO_OType = GPIO_OType_OD;
	InitС.GPIO_Speed = GPIO_Speed_2MHz;
	InitС.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&InitС); //ініцилізуємо ножки порта
	
	InitС.GPIO_Pin = GPIO_Pin_8;
	InitС.GPIO_Mode = GPIO_Mode_IN;
	InitС.GPIO_OType = GPIO_OType_OD;
	InitС.GPIO_Speed = GPIO_Speed_2MHz;
	InitС.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&InitС); //ініцилізуємо ножки порта
	
	InitС.GPIO_Pin = GPIO_Pin_9;
	InitС.GPIO_Mode = GPIO_Mode_IN;
	InitС.GPIO_OType = GPIO_OType_OD;
	InitС.GPIO_Speed = GPIO_Speed_2MHz;
	InitС.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&InitС); //ініцилізуємо ножки порта
	
	InitС.GPIO_Pin = GPIO_Pin_11;
	InitС.GPIO_Mode = GPIO_Mode_IN;
	InitС.GPIO_OType = GPIO_OType_OD;
	InitС.GPIO_Speed = GPIO_Speed_2MHz;
	InitС.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&InitС); //ініцилізуємо ножки порта
	
	InitС.GPIO_Pin = GPIO_Pin_15;
	InitС.GPIO_Mode = GPIO_Mode_IN;
	InitС.GPIO_OType = GPIO_OType_OD;
	InitС.GPIO_Speed = GPIO_Speed_2MHz;
	InitС.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&InitС); //ініцилізуємо ножки порта
}


int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	RCC->AHB1ENR |= LCD_GPIO_RCC | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN; // for lcd
	lcd_init();
	adc_init();
	port_ini();
	
	int check = 0;
	int d_check = 0;
	int kursor = 11;
	
    lcd_clear();
	delay__ms(500);
	lcd_set_cursor(0, 0);
    lcd_printf(" TIME: %d:%d:%d", hour, minute, second);
    lcd_set_cursor(0, 1);
    lcd_printf(" DATE: %d/%d/%d", day, month, year);
 
    while(1)
	{	
		//будиьник спрацював
		if(hour == alarm_h && minute == alarm_m && check == 0)
		{
			GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 );
			delay__ms(one_sec);
			check = ~check;
			second++;	
		}
		else if(hour == alarm_h && minute == alarm_m)
		{
			GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15 );
			delay__ms(one_sec);
			check = ~check;
			second++;
		}
		
		else//звичайний режим, час будильника не співпадає
		{
			delay__ms(one_sec);
			second++;
		}
		
		//визначаємо температуру
		adc_result = readADC1(ADC_Channel_9);
		temperature = -55*adc_result/2*0.0001+101;
		
		if(second == 60) //зміна хвилин, годин і дати
		{
			second  = 0;
			minute++;
			if(minute == 60) 
			{
				minute  = 0;
				hour++;
				if(hour == 24) 
				{
					hour  = 0;
					day++;
					if(day == 30 && check_month == 0) 
					{
						day  = 0;
						month++;
						check_month = ~check_month;
					}
					if(day == 31 && check_month == 1) 
					{
						day  = 0;
						month++;
						check_month = ~check_month;
						if(month == 13 ) 
						{
							month  = 0;
							year++;
							check_month = ~check_month;
						
						}
					}
				}
			}
			lcd_clear();
		}
		
		if(style == 1)//перший стиль відображення увімкнено
		{
			lcd_set_cursor(0, 0);
			lcd_printf(" TIME: %d:%d:%d", hour, minute, second);
			lcd_set_cursor(0, 1);
			lcd_printf(" DATE: %d/%d/%d", day, month, year);
		}
		else if (style == 2)//другий стиль відображення увімкнено
		{
			lcd_set_cursor(0, 0);
			lcd_printf("%d:%d:%d", hour, minute, second);
			lcd_set_cursor(14, 0);
			lcd_printf("*");
			lcd_set_cursor(0, 1);
			lcd_printf("%d/%d/%d   +%.0f^C", day, month, year, temperature);
		} 
		else//третій стиль відображення увімкнено
		{
			lcd_set_cursor(0, 0);
			lcd_printf(" TIME: %d:%d:%d", hour, minute, second);
			lcd_set_cursor(0, 1);
			lcd_printf(" TEMP: +%.2f^C", temperature);
		}
		
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0) //зміна стилю вгору
		{
			style++;
			if(style == 4) style = 1;
			lcd_clear();
		}
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0) //зміна стилю вниз
		{
			style--;
			if(style == 0) style = 3;
			lcd_clear();
		}
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==0) //налаштування будильника
		{
			lcd_clear();
			check = 1;
			while(check)
			{
				delay__ms(one_sec);
				second++;
				if(d_check == 0)//сигналізуємо про режим налаштувань
				{
					GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				else
				{
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				
				if(second == 60) 
				{
					second  = 0;
					minute++;
					if(minute == 60) 
					{
						minute  = 0;
						hour++;
						if(hour == 24) 
						{
							hour  = 0;
							day++;
							if(day == 30 && check_month == 0) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
							}
							if(day == 31 && check_month == 1) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
								if(month == 13 ) 
								{
									month  = 0;
									year++;
									check_month = ~check_month;
						
								}
							}
						}
					}
					lcd_clear();
				}
				
				lcd_set_cursor(0, 0);
				lcd_printf(" TIME:  %d:%d:%d", hour, minute, second);
				lcd_set_cursor(0, 1);
				lcd_printf(" ALARM: %d", alarm_h);
				if(kursor == 9) lcd_printf("^"); else lcd_printf(" ");
				lcd_printf(": %d", alarm_m);
				if(kursor == 11) lcd_printf("^"); else lcd_printf(" ");
				
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0) //переміщення курсору вліво
				{
					kursor = kursor - 2;
					if(kursor < 9) kursor = 11;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==0) //переміщення курсору вправо
				{
					kursor = kursor + 2;
					if(kursor > 11) kursor = 9;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0) //зміна значення вгору
				{
					if(kursor == 9) alarm_h++;
					if(kursor == 11) alarm_m++;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0) //зміна значення вниз
				{
					if(kursor == 9) alarm_h--;
					if(kursor == 11) alarm_m--;
				}
				if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==0) //прийняти встановлене значення 
				{												//будильника і вийти з режиму налаштування
					check = 0; d_check = 0;
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					lcd_clear();
				}
				
			}
		}
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0) //налаштування часу
		{
			lcd_clear();
			check = 1;
			while(check)
			{
				delay__ms(one_sec);
				second++;
				if(d_check == 0)//сигналізуємо про режим налаштувань
				{
					GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				else
				{
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				
				if(second == 60) 
				{
					second  = 0;
					minute++;
					if(minute == 60) 
					{
						minute  = 0;
						hour++;
						if(hour == 24) 
						{
							hour  = 0;
							day++;
							if(day == 30 && check_month == 0) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
							}
							if(day == 31 && check_month == 1) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
								if(month == 13 ) 
								{
									month  = 0;
									year++;
									check_month = ~check_month;
						
								}
							}
						}
					}
					lcd_clear();
				}
				
				lcd_set_cursor(0, 0);
				lcd_printf("  TIME SETTING:");
				lcd_set_cursor(0, 1);
				lcd_printf("    %d", hour);
				if(kursor == 9) lcd_printf("^"); else lcd_printf(" ");
				lcd_printf(": %d", minute);
				if(kursor == 11) lcd_printf("^"); else lcd_printf(" ");
				
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0) //переміщення курсору вліво
				{
					kursor = kursor - 2;
					if(kursor < 9) kursor = 11;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==0) //переміщення курсору вправо
				{
					kursor = kursor + 2;
					if(kursor > 11) kursor = 9;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0) //зміна значення вгору
				{
					if(kursor == 9) hour++;
					if(kursor == 11) minute++;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0) //зміна значення вниз
				{
					if(kursor == 9) hour--;
					if(kursor == 11) minute--;
				}
				if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==0) //прийняти встановлене значення 
				{												//часу і вийти з режиму налаштування
					check = 0; d_check = 0;
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					lcd_clear();
				}
			}
		}
		
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==0) //налаштування дати
		{
			lcd_clear();
			check = 1;
			while(check)
			{
				delay__ms(one_sec);
				second++;
				if(d_check == 0)//сигналізуємо про режим налаштувань
				{
					GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				else
				{
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					d_check = ~d_check;
				}
				
				if(second == 60) 
				{
					second  = 0;
					minute++;
					if(minute == 60) 
					{
						minute  = 0;
						hour++;
						if(hour == 24) 
						{
							hour  = 0;
							day++;
							if(day == 30 && check_month == 0) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
							}
							if(day == 31 && check_month == 1) 
							{
								day  = 0;
								month++;
								check_month = ~check_month;
								if(month == 13 ) 
								{
									month  = 0;
									year++;
									check_month = ~check_month;
						
								}
							}
						}
					}
					lcd_clear();
				}
				
				lcd_set_cursor(0, 0);
				lcd_printf("  DATE SETTING:");
				lcd_set_cursor(0, 1);
				lcd_printf("  %d", day);
				if(kursor == 9) lcd_printf("^"); else lcd_printf(" ");
				lcd_printf(": %d", month);
				if(kursor == 11) lcd_printf("^"); else lcd_printf(" ");
				lcd_printf(": %d", year);
				if(kursor == 13) lcd_printf("^"); else lcd_printf(" ");
				
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0) //переміщення курсору вліво
				{
					kursor = kursor - 2;
					if(kursor < 9) kursor = 13;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)==0) //переміщення курсору вправо
				{
					kursor = kursor + 2;
					if(kursor > 13) kursor = 9;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)==0) //зміна значення вгору
				{
					if(kursor == 9) day++;
					if(kursor == 11) month++;
					if(kursor == 13) year++;
				}
				if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0) //зміна значення вниз
				{
					if(kursor == 9) day--;
					if(kursor == 11) month--;
					if(kursor == 13) year--;
				}
				if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)==0) //прийняти встановлене значення 
				{												//дати і вийти з режиму налаштування
					check = 0; d_check = 0;
					GPIO_ResetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_14);
					lcd_clear();
				}
			}
		}
	}
	
	return 0;
}
