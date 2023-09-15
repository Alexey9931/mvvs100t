//������ ��������� ��� ������ �������� �� ��������� MT-12864
#include "MT-12864Av6.h"

// ������� ��������� ������������ ������(-��) ����� GPIO
PORT_InitTypeDef GPIOInitStruct;

const char Logo128[8][128];

static uint8_t	page = 0;				//����� ������� �������� ����������
static uint8_t	numChar = 0;		//������� �� ����������� ���������� �����

//��������� ����������� ������������� ����������
void LCDInit(void)
{	
	// �������� ������������ ������
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
	
	// �������������� ��������� ������������ ������(-��) ����� ���������� �� ���������
	PORT_StructInit(&GPIOInitStruct);
	
	// �������� �������� �� ��������� �� ����������� ��� ���������
	GPIOInitStruct.PORT_OE		= PORT_OE_OUT;
	GPIOInitStruct.PORT_SPEED	= PORT_SPEED_SLOW;
	GPIOInitStruct.PORT_MODE	= PORT_MODE_DIGITAL;
	
	// ��������� ����������� ���� ���������
	PORT_Init(LCD_PORT_Data, &GPIOInitStruct);
	PORT_Init(LCD_PORT_RW_E_RES, &GPIOInitStruct);
	PORT_Init(LCD_PORT_A0_E1_E2, &GPIOInitStruct);
	
	PORT_WriteBit(LCD_PORT_RW_E_RES, LCD_E, 0);				//��������� �������� ������� ����������

	Delay_micro(15);																	//�������� �� ����� ������ 10 ���
	
	WriteCodeL(0xC0);		//������� ������ �� 0 
	WriteCodeR(0xC0);		//������� ������ �� 0
	
	WriteCodeL(0x3F);		//Display on
	WriteCodeR(0x3F);		//Display on

	LCD_Clear();
}

//��������� ����������� ������������� ���������� (��� ������� ����� ����� ������ ������������ ���� �� ���)
void LCDReInit(void)
{	

	// �������������� ��������� ������������ ������(-��) ����� ���������� �� ���������
	PORT_StructInit(&GPIOInitStruct);
	
	// �������� �������� �� ��������� �� ����������� ��� ���������
	GPIOInitStruct.PORT_OE		= PORT_OE_OUT;
	GPIOInitStruct.PORT_SPEED	= PORT_SPEED_SLOW;
	GPIOInitStruct.PORT_MODE	= PORT_MODE_DIGITAL;
	
	// ��������� ����������� ���� ���������
	PORT_Init(LCD_PORT_Data, &GPIOInitStruct);
	PORT_Init(LCD_PORT_RW_E_RES, &GPIOInitStruct);
	PORT_Init(LCD_PORT_A0_E1_E2, &GPIOInitStruct);
}

void WriteCodeL(char byte) { WriteByte(byte,0,1,0); }	//������� � ����� �������� ����������
void WriteCodeR(char byte) { WriteByte(byte,0,0,1); }	//������� � ������ �������� ����������

void WriteDataL(char byte) { WriteByte(byte,1,1,0); }	//������ � ����� �������� ����������
void WriteDataR(char byte) { WriteByte(byte,1,0,1); }	//������ � ������ �������� ����������

char ReadDataL(void) { return ReadByte(1,1,0); }	//��������� ���� ������ �� ����� �������� ����������
char ReadDataR(void) { return ReadByte(1,0,1); }	//��������� ���� ������ �� ������ �������� ����������

void SetBusData(char byte)
{	
	// �������� �������� �� ��������� �� ����������� ��� ���������
	GPIOInitStruct.PORT_OE		= PORT_OE_OUT;
	
	// ��������� ����������� ���� ���������
	PORT_Init(LCD_PORT_Data, &GPIOInitStruct);
	
	PORT_WriteBit(LCD_PORT_Data, LCD_D0, (byte & 0x01));
	PORT_WriteBit(LCD_PORT_Data, LCD_D1, (byte & 0x02));
	PORT_WriteBit(LCD_PORT_Data, LCD_D2, (byte & 0x04));
	PORT_WriteBit(LCD_PORT_Data, LCD_D3, (byte & 0x08));
	PORT_WriteBit(LCD_PORT_Data, LCD_D4, (byte & 0x010));
	PORT_WriteBit(LCD_PORT_Data, LCD_D5, (byte & 0x020));
	PORT_WriteBit(LCD_PORT_Data, LCD_D6, (byte & 0x040));
	PORT_WriteBit(LCD_PORT_Data, LCD_D7, (byte & 0x080));
}

char ReadBusData()
{
	uint8_t byte = 0;
	
	// �������� �������� �� ��������� �� ����������� ��� ���������
	GPIOInitStruct.PORT_OE		= PORT_OE_IN;
	
	// ��������� ����������� ���� ���������
	PORT_Init(LCD_PORT_Data, &GPIOInitStruct);
	
	byte = (char)PORT_ReadInputData(LCD_PORT_Data) & 0xff;
	
	return (char)byte;
}

//��������� ������ ����� � ���������
void WriteByte(char byte, uint8_t cd, uint8_t left, uint8_t right)
{
	if ((left==1)&&(right==1)) return;	//������ (��� ������ ����������) ������������ �� ����� ���������� �����������!
	WaitReady(left,right);							//����� ���������� ����������
	
	//��� ������������� ��������� ����� ���� ������ �� �����
	PORT_ResetBits(LCD_PORT_RW_E_RES, LCD_RW);
	
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_A0, cd);	//������ ����� � ��������� ��� ������ ��� �������
	
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E1, left);
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E2, right);		//������� ������ ������ ��������� � ����������
	
	SetBusData(byte);													//������ ���� �� ���� ������ ����������
	Delay_micro(1);														//��� ����� ������������� ������ (tAW) >140ns
	PORT_SetBits(LCD_PORT_RW_E_RES, LCD_E);		//������������ ����� ������ � ��������� E=1
	Delay_micro(1);														// >450ns ������������ ������� E=1 (����� ������������� ������ ������ ���� (tDS))
	PORT_ResetBits(LCD_PORT_RW_E_RES, LCD_E);	//�������� ������ E
	Delay_micro(1);														//���������� ���������� �������� ����� ��������� E=1 >(1000ns-140ns-450ns)
}

char ReadByte(uint8_t cd, uint8_t left, uint8_t right)
{
	char byte;
	if ((left==1)&&(right==1)) return 0;	//������ ������������ �� ����� ���������� �����������!
	WaitReady(left,right);								//����� ���������� ����������
	PORT_SetBits(LCD_PORT_RW_E_RES, LCD_RW);
	
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_A0, cd);	//������ ����� � ��������� ��� ������ ��� �������
	
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E1, left);
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E2, right);		//������� ������ ������ ��������� � ����������
	
	Delay_micro(1);										//��� ����� ������������� ������ (tAW) >140ns
	PORT_SetBits(LCD_PORT_RW_E_RES, LCD_E);		//������ ����� � ���������
	Delay_micro(1);										//>450ns ���������� ���������� ������������ ������� E=1 (����� ������� (tDDR) ������ ����)
	byte = ReadBusData();							//��������� ������ � ���� ���������� (��� �� ��� ��� ������� 130��)
	PORT_ResetBits(LCD_PORT_RW_E_RES, LCD_E);	//�������� ������ E
	Delay_micro(1);										//���������� ���������� �������� ����� ��������� E=1 >(1000ns-140ns-450ns)
	return byte;											//������� ����������� ����
}

void WaitReady(uint8_t left, uint8_t right)
{	//����� ���������� ����������, ��������� ���� �������
	//��� ������������� ��������� ����� ���� ������ �� ����
	
	PORT_SetBits(LCD_PORT_RW_E_RES, LCD_RW);
	PORT_ResetBits(LCD_PORT_A0_E1_E2, LCD_A0);	//������ ����� ���������
	
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E1, left);
	PORT_WriteBit(LCD_PORT_A0_E1_E2, LCD_E2, right);		//������� ������ ������ ��������� � ����������	
	
	Delay_micro(1);		//��� ����� ������������� ������ (tAW) >140ns
	
	PORT_SetBits(LCD_PORT_RW_E_RES, LCD_E);
	Delay_micro(1);										//����������� ������������ ������� E=1 >450ns (���������� �� ���� ������ ����������� ����� ������ ������, �� ����� ��� ����� 320��)
	
	// �������� �������� �� ��������� �� ����������� ��� ���������
	GPIOInitStruct.PORT_OE		= PORT_OE_IN;
	
	// ��������� ����������� ���� ���������
	PORT_Init(LCD_PORT_Data, &GPIOInitStruct);
	
	//Delay_micro(100);
	//while(PORT_ReadInputDataBit (LCD_PORT_Data, LCD_D7) == 1);	//����� ������ ����� ���������
	
	PORT_ResetBits(LCD_PORT_RW_E_RES, LCD_E);	//�������� ������ E
	
	Delay_micro(1);										//���������� ���������� �������� ����� ��������� E=1 >(1000ns-140ns-450ns)
}

void PrintLogo()
{	
	// ��������� �� ����� ���� ������ ������
	page = 0;		//����� ������� �������� ����������
	numChar = 0;		//������� �� ����������� ���������� �����
	for(page = 0; page < 8; page++)
	{//���� �� ���� 8-�� ��������� ����������
		WriteCodeL(page|0xB8);		//��������� ������� �������� ��� ������ ��������� ���������� (Set Page)
		WriteCodeL(0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������ ��������� ���������� (Set Address)
		for(numChar = 0; numChar < 64; numChar++) {			//���� ������ ������ � ����� �������� ����������
			WriteDataL(Logo128[page][numChar]);	//����� ���������� ����� � ���������
		}
		WriteCodeR(page|0xB8);		//��������� ������� �������� ��� ������� ��������� ����������
		WriteCodeR(0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������� ��������� ����������
		for(numChar=64; numChar<128; numChar++) {				//���� ������ ������ � ������ �������� ����������
			WriteDataR(Logo128[page][numChar]);	//����� ���������� ����� � ���������
		}
	}
}


void LCD_Clear()
{
	// ��������� �� ����� ���� ������ ������
	page = 0;		//����� ������� �������� ����������
	numChar = 0;		//������� �� ����������� ���������� �����
	
	WriteCodeL(0|0xB8);		//��������� ������� �������� ��� ������ ��������� ���������� (Set Page)
	WriteCodeL(0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������ ��������� ���������� (Set Address)
	
	for(page = 0; page < 8; page++)
	{//���� �� ���� 8-�� ��������� ����������
		WriteCodeL(page|0xB8);		//��������� ������� �������� ��� ������ ��������� ���������� (Set Page)
		WriteCodeL(0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������ ��������� ���������� (Set Address)
		for(numChar = 0; numChar < 64; numChar++) {			//���� ������ ������ � ����� �������� ����������
			WriteDataL(0x00);	//����� ���������� ����� � ���������
		}
		WriteCodeR(page|0xB8);		//��������� ������� �������� ��� ������� ��������� ����������
		WriteCodeR(0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������� ��������� ����������
		for(numChar=64; numChar<128; numChar++) {				//���� ������ ������ � ������ �������� ����������
			WriteDataR(0x00);	//����� ���������� ����� � ���������
		}
	}
	// ��������� �� ����� ���� ������ ������
	page = 0;		//����� ������� �������� ����������
	numChar = 0;		//������� �� ����������� ���������� �����
}

void LCD_PrintMessageInPosition(wchar_t *message, uint32_t length, uint8_t user_page, uint8_t user_numChar)
{	// ������ ������� ��� �������� �������� ����
	
	// ��������� �� ����� ���� ������ ������
	page = user_page;		//����� ������� �������� ����������
	numChar = user_numChar * (uint8_t)MyFont.width;		//������� �� ����������� ���������� �����
	
	LCD_PrintMessageContinue(message, length);
}

void LCD_ClearLine(uint8_t user_page)
{	// ������ ������� ��� �������� �������� ����
	
	// ��������� �� ����� ���� ������ ������
	page = user_page;		//����� ������� �������� ����������
	numChar = 0;		//������� �� ����������� ���������� �����
		
	WriteCodeL(page|0xB8);		//��������� ������� �������� ��� ������ ��������� ���������� (Set Page)
	WriteCodeL(numChar | 0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������ ��������� ���������� (Set Address)
	
	for(; numChar < 64; numChar++) 
	{			//���� ������ ������ � ����� �������� ����������
		WriteDataL(0x00);	//����� ���������� ����� � ���������
	}
	WriteCodeR(page | 0xB8);		//��������� ������� �������� ��� ������� ��������� ����������
	WriteCodeR(numChar | 0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������� ��������� ����������
	
	for(numChar=64; numChar<128; numChar++) {				//���� ������ ������ � ������ �������� ����������
		WriteDataR(0x00);	//����� ���������� ����� � ���������
	}

	// ��������� �� ����� ���� ������ ������
	page = user_page;	//����� ������� �������� ����������
	numChar = 0;			//������� �� ����������� ���������� �����
}

void LCD_PrintMessageNew(wchar_t *message, uint32_t length)
{	// ������ ������� ��� �������� �������� ����
	
	// ��������� �� ����� ���� ������ ������
	page = 0;		//����� ������� �������� ����������
	numChar = 0;		//������� �� ����������� ���������� �����
	
	LCD_PrintMessageContinue(message, length);
}

void LCD_PrintMessageContinue(wchar_t *message, uint32_t length)
{	// ������ ������� ��� �������� �������� ����
	
	uint32_t cntBytes = 0;	// ���������� ���������� ����
	uint8_t flagSpace = 1;	// ���� ������������� �������
	
	if(length == 0) return; // ���� ����� ��������� ����� 0, �� �������
	
	for(; page < 8; page++)
	{//���� �� ���� 8-�� ��������� ����������
		WriteCodeL(page|0xB8);		//��������� ������� �������� ��� ������ ��������� ���������� (Set Page)
		WriteCodeL(numChar|0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������ ��������� ���������� (Set Address)
		for(; numChar < 64; numChar++) {			//���� ������ ������ � ����� �������� ����������
			
			// ������ �������� ��� ��������� (� � � ��� ��� ������ ��� ������� � ������� �� 0x390)
			if(message[cntBytes / MyFont.width] > 0xFF) message[cntBytes / MyFont.width] = message[cntBytes / MyFont.width] - 0x390; 
			
			if(flagSpace != 0) // ����� ������ �������� ������
			{
				WriteDataL(0x00);
				flagSpace = 0;
			}
			else 
			{	
				WriteDataL(MyFont.fontArray[MyFont.width * ((char)message[cntBytes / MyFont.width] - 0x20) + cntBytes  - (cntBytes / MyFont.width) * MyFont.width]);	//����� ���������� ����� � ���������
				++cntBytes;
				if((cntBytes % MyFont.width) == 0)	flagSpace = 1;
			}
			if(cntBytes == (MyFont.width) * length) 
			{
				numChar++;
				return;
			}
		}
		WriteCodeR(page|0xB8);		//��������� ������� �������� ��� ������� ��������� ����������
		WriteCodeR(numChar|0x40);			//��������� �������� ������ ��� ������ ������ � 0 ��� ������� ��������� ����������
		for(; numChar<128; numChar++) {				//���� ������ ������ � ������ �������� ����������
			if(message[cntBytes / MyFont.width] > 0xFF) message[cntBytes / MyFont.width] = message[cntBytes / MyFont.width] - 0x390;
			
			if(flagSpace != 0) // ����� ������ �������� ������ (������� � +1 � � ������ �����������)
			{
				WriteDataR(0x00);
				flagSpace = 0;
			}
			else 
			{	
				WriteDataR(MyFont.fontArray[MyFont.width * ((char)message[cntBytes / MyFont.width] - 0x20) + cntBytes  - (cntBytes / MyFont.width) * MyFont.width]);	//����� ���������� ����� � ���������
				++cntBytes;	
				if((cntBytes % MyFont.width) == 0)	flagSpace = 1;				
			}
			if(cntBytes == (MyFont.width) * length)
			{
				numChar++;
				return;
			}
		}
		numChar = 0;
	}
	page = 0;
}

void LCD_EndLine()
{	// ������ ������� ��� �������� �������� ����
	++page;
	numChar = 0;
}

//������ �����������, ������� �� ������� � ����� ����� �� ���������� �����������.
//��� ������ ������������������ ���� ��� ������ � ��������� ������� � ������� ��������.
//��������� ������������� �������� ������������� ��� � ������������ �� ������.
const char Logo128[8][128] = //128x64 pixel, ������ 8 ������������ ����� ������� � ����
{
	{	0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
		0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF
	},
	{	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xC0,0xE0,0xF0,0xF0,0xF0,
		0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xE0,0xE0,0xE0,0xC0,0x80,0x00,
		0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
		0x00,0x80,0x80,0x00,0x00,0x80,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x80,0x00,0x00,
		0x80,0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x00,0x00,0x80,
		0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,
		0x00,0x00,0x80,0x00,0x80,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x00,0x00,0x80,0x00,
		0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x3F
	},
	{	0x1F,0xC0,0xE0,0xE0,0xFC,0xFE,0xF3,0xE3,0xE3,0xE3,0xE3,0xAF,0x6F,0xFF,0xFF,0xFF,
		0xBF,0xBF,0xFF,0xFF,0x7F,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xF6,0xF4,0x1F,0x21,0xE2,0x61,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,
		0x60,0x6F,0x62,0x61,0x67,0x68,0x67,0x60,0x6A,0x67,0x60,0x6E,0x61,0x6F,0x60,0x67,
		0x6A,0x6B,0x60,0x6F,0x65,0x68,0x60,0x60,0x6F,0x60,0x60,0x6F,0x62,0x61,0x67,0x68,
		0x67,0x60,0x6F,0x62,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,0x64,0x6A,0x6F,
		0x60,0x60,0x6F,0x64,0x6F,0x60,0x60,0x60,0x6F,0x60,0x67,0x6A,0x6B,0x60,0x6D,0x62,
		0x6D,0x60,0x6F,0x62,0x6F,0x60,0x6F,0x64,0x6F,0x60,0x6F,0x65,0x68,0x64,0x6A,0x6F
	},
	{	0xFE,0x3F,0x3F,0x3F,0xBF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xF9,0x77,0x37,
		0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x01,0x07,0x0F,0x13,0x01,0x00,0x00,0x01,0x07,
		0xD7,0x0E,0x08,0xC0,0x00,0x10,0x0F,0xF8,0x00,0x00,0x80,0xE0,0xF8,0xFE,0xFF,0xFF,
		0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xFE,0xFF,0xFF,0xFF,0xFE,0x00,0x00,
		0x00,0xE0,0xF0,0xF8,0xFC,0x7C,0x3E,0x1E,0x1F,0x0F,0x0F,0x0F,0x0F,0x1F,0x1E,0x1E,
		0x3E,0x7C,0xFC,0xF8,0xF0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x80,0xC0,0xF0,0xFC,0xFE,0xFF,0xFF,0xFE,0x00,0x00,0x06,0x0F,0x0F,0x0F,0x0F,0x0F,
		0x0F,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x06,0xF0
	},
	{	0x3F,0xE0,0x00,0x00,0x01,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F,0x3F,0x00,0x00,0xE0,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x01,0x01,0xC1,0xE0,0xE0,0xF9,0xFF,0xFC,0xFF,0xFF,0x7F,0x1F,0xFF,0xFF,0xFF,
		0xFF,0xFF,0x80,0xE0,0xF8,0xFE,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
		0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x30,0x78,0x78,0xFC,0xFC,0xFC,0xFC,0x78,0x78,
		0x30,0x00,0x01,0x07,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFE,
		0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFC,0x01,0x03,0x04,0x18,0x20,0x40,0x81,0x87,0x07,0x07,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
		0x80,0xE4,0xE7,0xFB,0xFF,0xFF,0x7F,0x3F,0x0F,0x03,0x01,0x00,0x00,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0x7F,0x1F,0x07,0x03,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
		0x00,0x70,0xF8,0xF8,0xF0,0xE0,0xE0,0xC0,0xC0,0x80,0x80,0x80,0x80,0xC0,0xC0,0xC0,
		0xE0,0xF0,0xFC,0xFF,0x7F,0x3F,0x0F,0x81,0xE0,0xF8,0xFC,0xFF,0xFF,0x3F,0x1F,0x07,
		0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x04,0x04,0x04,0x0C,
		0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x04,0x04,0x04,0x02,0x03,0x01,0x00,
		0x00,0x03,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,
		0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,
		0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x07,0x07,0x07,0x07,0x07,0x07,0x03,0x03,
		0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x03,0x07,0x07,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF
	},
	{	0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
		0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF
	}
};
