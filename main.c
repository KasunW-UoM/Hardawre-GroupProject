
#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif


#define D4 eS_PORTB4
#define D5 eS_PORTB5
#define D6 eS_PORTB6
#define D7 eS_PORTB7
#define RS eS_PORTB1
#define EN eS_PORTB0

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include "i2cmaster.h"
#include "i2c_lcd.h"

void adc_init(void);
uint16_t adc_read(uint8_t ch);	
char keyfind();
double voltage();
double current ();
double Expend();
double KWh();
void led();
char c;
double k;

#define KEY_PRT 	PORTB
#define KEY_DDR		DDRB
#define KEY_PIN		PINB

unsigned char keypad[4][4] = {	{'D','C','B','A'},
								{'#','9','6','3'},
								{'0','8','5','2'},
								{'*','7','4','1'}};

unsigned char colloc, rowloc;

	double sensorValue=0;
	double sensorValue1=0;
	int crosscount=0;
	int climbhill=0;
	double VmaxD=0;
	double VeffD;
	double Veff;
	double maxVoltage = 0;
	double maxCurrent = 0;
	int adcVoltValInt, adcCurrValInt;
	double sum;
	double total=0;
	char analogValueString[20];


int main(void)
{
	
	
	
	lcd_init(LCD_BACKLIGHT_ON);
	lcd_clear();
	lcd_goto_xy(0, 0);
	lcd_puts("   Welcome..!");
	_delay_ms(500);
	lcd_clear();
//	lcd_puts("Enter price");
//	_delay_ms(500);
// 	char key;
// 	key = keyfind();
// 	lcd_putc(key); /* Display which key is pressed */
// 	if(key == '-')
// 	lcd_clear();
	
	while(1){
		
 	
		maxVoltage = 0;
		adc_init();
		lcd_goto_xy(0, 0);
		lcd_puts("Current");
		lcd_goto_xy(0,1);
		lcd_puts("Voltage");
		lcd_goto_xy(-4,2);
		lcd_puts("KWh");
		lcd_goto_xy(-4,3);
		lcd_puts("Expend");
 		adcVoltValInt = adc_read(3);
 		
		 
		dtostrf(voltage(), 7, 3, analogValueString);
		lcd_goto_xy(9,1);
		lcd_puts(analogValueString);

		
		
		lcd_goto_xy(9, 0);
		dtostrf(current(), 7, 3, analogValueString);
		lcd_puts(analogValueString);
		
		
		dtostrf(KWh(), 7, 4, analogValueString);
		lcd_goto_xy(5,2);
		lcd_puts(analogValueString);
		
		dtostrf(Expend(), 7, 3, analogValueString);
		lcd_goto_xy(5,3);
		lcd_puts(analogValueString);
		
		led();
//		_delay_ms(1000);
	
		}

}




char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				}while(colloc == 0x0F);        /* check for any key press */
				
				_delay_ms (40);	            /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}
			
			KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}

		if(colloc == 0x0E)
		return(keypad[rowloc][0]);
		else if(colloc == 0x0D)
		return(keypad[rowloc][1]);
		else if(colloc == 0x0B)
		return(keypad[rowloc][2]);
		else
		return(keypad[rowloc][3]);
	
}
		
	
	void adc_init()
	{
		// AREF = AVcc
		ADMUX = (1<<REFS0);
		// ADC Enable and prescaler of 128
		ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	}
	// read adc value
	uint16_t adc_read(uint8_t ch)
	{
		// select the corresponding channel 0~7
		ch &= 0b00000111;  // AND operation with 7
		ADMUX = (ADMUX & 0xF8)|ch;
		// start single conversion
		// write '1' to ADSC
		ADCSRA |= (1<<ADSC);
		// wait for conversion to complete
		// ADSC becomes '0' again
		while(ADCSRA & (1<<ADSC));
		return (ADC);
		
	}
	
	double current (){
		maxCurrent = 0;
		for (int i = 0; i< 100 ; i++)
		{
			adcCurrValInt = adc_read(2);
			_delay_ms(1);
			if (maxCurrent < adcCurrValInt)
			maxCurrent = adcCurrValInt;
		}
		if(maxCurrent - 511.0 < 20)
		maxCurrent = 511.0;
		return (maxCurrent - 511.0)/30;
	}
	
	double voltage(){
		for(int i = 0;i < 100;i++){
			sensorValue1 = sensorValue;
			_delay_ms(1);
			sensorValue = adc_read(3);
			if (sensorValue>sensorValue1 && sensorValue>511){
				climbhill=1;
				VmaxD=sensorValue;
			}
			if (sensorValue<sensorValue1 && climbhill==1){
				climbhill=0;
				VmaxD=sensorValue1;
				VeffD=VmaxD/sqrt(2);
				Veff=(((VeffD-420.76)/-90.24)*-210.2)+210.2 - 40.0;
				VmaxD=0;
			}
			if (maxVoltage < Veff)
			maxVoltage = Veff;
			if(maxVoltage < 60.0)
			maxVoltage = 0.0;
			
		}
		return maxVoltage;
		
	}
	double KWh(){
		for(int i=0; i<3600; i++){
			sum=current()*voltage()/(1000.0*3600);
			total= sum +total;
			return total;
		}
	}
	double Expend(){
		if(KWh()<61)
		return KWh()*3+30;
		
		if(KWh()<91 && KWh>=61)
		return KWh()*10+30;
		
		if(KWh()<120 && KWh>=91)
		return KWh()*27.5+30;
		
	}
	
	

	
	// double k = double (Expend()*100)/1000;
  void led(){
	  DDRD=0xff;
	  
	 // while(1){
		int t =3600000;
		if((Expend()*100)/1000<=12.5){
			PORTD=0xFE;
				
		}
		if((Expend()*100)/1000<=25 && (Expend()*100)/1000<=12.5){
			PORTD=0xFC;
			
		}
		if((Expend()*100)/1000<=37.5 && (Expend()*100)/1000>25 ){
			PORTD=0xF8;
		}
		if((Expend()*100)/1000<=50 && (Expend()*100)/1000>37.5){
			PORTD=0xF7;
		}
		if((Expend()*100)/1000<=62.5 && (Expend()*100)/1000>50){
			PORTD=0xE7;
		}
		if((Expend()*100)/1000<=75 && (Expend()*100)/1000>62.5){
			PORTD=0xDF;
		}
		if((Expend()*100)/1000<=87.5 && (Expend()*100)/1000>75){
			PORTD=0x9F;
		}
		if((Expend()*100)/1000<=100 && (Expend()*100)/1000>87.5){
			PORTD=0x1F;
		}
		
		
		//lcd_init(LCD_BACKLIGHT_ON);
		//_delay_ms(1000);
		//adc_init();
	  //}
		
  }
		
	
		
		
	
		
	
	
