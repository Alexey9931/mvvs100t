#include "ADC.h"
#include "REGISTER_SPACE.h"

// Объявление структур для общей настройки АЦП3
ADCx_InitTypeDef ADC3;
// Объявление структур для общей настройки АЦП
ADC_InitTypeDef ADC;

static char TX_data[100];

void Delay(int waitTicks)
{
   int i;
   for (i = 0; i < waitTicks; i++)
  {
   __NOP();
  }
}

void ADCInit(void)
{
	// Включение тактирования АЦП
	RST_CLK_PCLKcmd(RST_CLK_PCLK_ADC, ENABLE);
	// Загрузка значений по умолчанию в структуру ADC
	ADC_StructInit(&ADC);
	// Инициализация АЦП объявленной структурой
	ADC_Init(&ADC);
}

// Процедура настройки каналов АЦП
void ADC_Channel_Init(void)
{
	// Загрузка значений по умолчанию в структуру ADC3
	ADCx_StructInit(&ADC3);
	// Установка номера канала АЦП,
	// подключенного к резистору R1 платы
	ADC3.ADC_ChannelNumber = ADC_CH_ADC3;
	ADC3.ADC_Prescaler = ADC_CLK_div_4;
	// Инициализация 3 АЦП объявленной структурой
	ADC1_Init(&ADC3);
	// Включение АЦП
	//ADC1_ITConfig (ADC1_IT_END_OF_CONVERSION, ENABLE);
	//NVIC_SetPriority (ADC_IRQn, 1);
	//NVIC_EnableIRQ (ADC_IRQn);
	ADC1_Cmd(ENABLE);
}

void ADC_start_mesuare(void)
{
	uint32_t RESULT;
	float U[ADC_Mesures_Number];
	//char V[10] = {0};
	uint8_t AI_CodeADC[2];
	uint8_t AI_PhysQuantInt[2];

	
	//memset(TX_data,0,strlen(TX_data));
	for (uint16_t Channel = 0; Channel < ADC_Channels_Number; Channel++)
	{
		if (Channel == ADC_CH_ADC0)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC0);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC1)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC1);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC2)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC2);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC3)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC3);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC4)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC4);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC5)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC5);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC6)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC6);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_CodeADC[0] = (uint8_t)(RESULT&0xFF);
			AI_CodeADC[1] = (uint8_t)(RESULT>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
		else if (Channel == ADC_CH_ADC7)
		{
			for (int i = 0; i < ADC_Mesures_Number; i++)
			{
				ADC1_SetChannel(ADC_CH_ADC7);
				ADC1_Start(); // Начало преобразования
				while ((ADC1_GetStatus() & ADC_STATUS_FLG_REG_EOCIF) == 0);
				RESULT = ADC1_GetResult() & 0x00000FFF;
				U[i] = (float)RESULT*0.000814f+0.00438f;
			}
			//запись сырого кода ацп в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_CodeADC,1662+2*Channel, 2, MPA);
			//запись физ величины целочисленной в регистры
			AI_PhysQuantInt[0] = (uint8_t)((int)averaging(U)&0xFF);
			AI_PhysQuantInt[1] = (uint8_t)((int)averaging(U)>>8);
			write_register(AI_PhysQuantInt,1710+2*Channel, 2, MPA);
			//sprintf(V, "%.4f\t", averaging(U));
			//strcat(TX_data,V);
		}
	}
	//strcat(TX_data, "\r");
	//UART_send_string(TX_data);
}
float averaging(float *U)
{
	float result;
	for (int i = 0; i < ADC_Mesures_Number;i++)
	{
		result += U[i];
	}
	result /= ADC_Mesures_Number;
	
	return result;
}
uint32_t averaging_result(uint32_t RESULT[])
{
	uint32_t result;
	for (int i = 0; i < ADC_Mesures_Number;i++)
	{
		result += RESULT[i];
	}
	result /= ADC_Mesures_Number;
	
	return result;
}
/*void ADC_IRQHandler (void)
{
	uint32_t RESULT;
	float U;
	char V[10] = {0};
	if (ADC1_GetITStatus (ADC1_IT_END_OF_CONVERSION))
	{
		memset(V,0,strlen(V));
		RESULT = ADC1_GetResult() & 0x00000FFF;
		U = (float)RESULT / KALIBR;
		sprintf(V, "%.2f ", U);
		memset(TX_data,0,strlen(TX_data));
		sprintf(TX_data, "%.2f\r", U);
		strcat(TX_data,"h ");
		UART_send_string("h ");
	}
}*/