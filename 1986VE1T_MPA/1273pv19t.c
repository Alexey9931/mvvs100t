/*!
 \file
 \brief Файл с реализацией API для работы с АЦП 1273ПВ19Т
*/

#include "math.h"
#include "1273pv19t.h"
#include "TIMER.h"

void adc_init(void)
{
	SSP_SendData(MDR_SSP1, 0x8308);//регистр D(канал 1)
	Delay_milli(1);
//	SSP_SendData(MDR_SSP1, 0x8380);//регистр D(канал 2)
//	Delay_milli(1);
	SSP_SendData(MDR_SSP1, 0x82E0);//регистр C
	Delay_milli(1);
	SSP_SendData(MDR_SSP1, 0x8001);//регистр A
	Delay_milli(1);
}
void adc_reset(void)
{
	PORT_ResetBits(MDR_PORTC,PORT_Pin_0);
	Delay_milli(100);
	PORT_SetBits(MDR_PORTC,PORT_Pin_0);
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
		adc_code = ~get_FIFO_SPI_RX()+1;
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