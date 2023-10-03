/*!
 \file
 \brief Файл с реализацией API для работы с АЦП 1273ПВ19Т
*/

#include "math.h"
#include "1273pv19t.h"
#include "TIMER.h"
#include "SPI.h"

void adc_gpio_config(void)
{
	// Включение тактирования портов
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);	
	
	PORT_InitTypeDef GPIO_init_structADC;
	
	//инициализация RESET PD9 для аппаратного сброса ацп
	GPIO_init_structADC.PORT_Pin = PIN_ADC_RST;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_RST, &GPIO_init_structADC);
	//установка RESET в лог единицу
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
	
	//инициализация ножек выбора режима работы (0-10В,4-20мА)
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A0;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	GPIO_init_structADC.PORT_Pin = PIN_ADC_MODE_A1;
	GPIO_init_structADC.PORT_FUNC = PORT_FUNC_PORT;
	GPIO_init_structADC.PORT_OE = PORT_OE_OUT;
	PORT_Init(PORT_ADC_MODE, &GPIO_init_structADC);
	
	PORT_SetBits(PORT_ADC_MODE,PIN_ADC_MODE_A0);
	PORT_ResetBits(PORT_ADC_MODE,PIN_ADC_MODE_A1);
	
}

void adc_init(void)
{
	PORT_SetBits(PORT_SSP1,PIN_SSP1_SS);
	adc_gpio_config();
	
	adc_reset();
	delay_milli(1);
	
	//режим control, запись в регистр D управление питанием АЦП1 и АЦП2
	SSP_SendData(MDR_SSP1, 0x8388);
	delay_milli(1);
	//режим control, запись в регистр E управление питанием АЦП3 и АЦП4
	SSP_SendData(MDR_SSP1, 0x8488);
	delay_milli(1);
	//режим control, запись в регистр F управление питанием АЦП5 и АЦП6
	SSP_SendData(MDR_SSP1, 0x8588);
	delay_milli(1);
	//режим control, запись в регистр C вкл. 5В режима,использ. вывода REFOUT, вкл. опорное напряж.
	SSP_SendData(MDR_SSP1, 0x82E0);
	delay_milli(1);
	//режим control, запись в регистр А - перевод в режим данных
	SSP_SendData(MDR_SSP1, 0x8001);
	delay_milli(1);
}
void adc_reset(void)
{
	PORT_ResetBits(PORT_ADC_RST,PIN_ADC_RST);
	delay_milli(100);
	PORT_SetBits(PORT_ADC_RST,PIN_ADC_RST);
}
float adc_read(void)
{
	int16_t adc_code;
	float U, U1, delta, I, I1;
	//adc_code = get_FIFO_SPI_RX();
	
	
	//для двуполярного вх напр (-5.4 ... 5.4 В ) на мультиплексоре A0=A1=0
//	adc_code = ~get_FIFO_SPI_RX()+1;
//	U = 1.6474f*pow(10,-4)*adc_code;
//	delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;
//	U1 = U - delta; 
	
	
//	//для однополярного вх напр (0 ... 10.8 В ) на мультиплексоре A0=1;A1=0
//	adc_code = ~get_FIFO_SPI_RX()+1;
//	U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
//	delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
//	U1 = U - delta; 
	
	
//		//для однополярного вх тока (0 ... 21.6 мА ) на мультиплексоре A0=1;A1=0
//		adc_code = ~get_FIFO_SPI_RX()+1;
//		I = 3.052f*pow(10,-4)*adc_code + 10.0f;
//		delta = -11.9006f*pow(10,-6)*adc_code + 0.03072506f;
//		I1 = I - delta; 
	
//		//самодиагностика для двуполярного случая	на мультиплексоре A0=1;A1=1 (на выходе должно быть 0В)
//		adc_code = ~get_FIFO_SPI_RX()+1;
//		U = 1.6474f*pow(10,-4)*adc_code;
//		delta = 6.5627f*pow(10,-6)*adc_code + 0.00039f;
//		U1 = U - delta; 
//		
		
		//самодиагностика для однополярного случая	на мультиплексоре A0=0;A1=1 (на выходе должно быть 0В)
//		adc_code = ~get_FIFO_SPI_RX()+1;
		U = 1.6474f*pow(10,-4)*adc_code + 5.398f;
		delta = 6.6962f*pow(10,-6)*adc_code + 0.4252307f;
		U1 = U - delta; 
	
	//U = 2.08*pow(10,-4)*adc_code + 6.812f;
	//после аппроксимации
//	if (U <= 1.0f)
//	{
//		delta = 0.0f;
//	}
//	else
//	{
		//delta = 5*pow(10,-5)*adc_code + 1.8407f;
		//delta = 7*pow(10,-28)*pow(adc_code,6) - 7*pow(10,-24)*pow(adc_code,5) - 8*pow(10,-19)*pow(adc_code,4) + 6*pow(10,-15)*pow(adc_code,3) + 2*pow(10,-10)*pow(adc_code,2) + 5*pow(10,-5)*adc_code + 1.3873f;
//	}
	//delta = 5*pow(10,-5)*adc_code + 1.395f;
	//U1 = U - delta; 
	
	return U1;
}