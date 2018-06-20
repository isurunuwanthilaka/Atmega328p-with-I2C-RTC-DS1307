#define numberOfButtons 4
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "ButtonPress.h"
#include "i2c.h"

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTB0
#define EN eS_PORTB1

#define Low 2
#define medium 3
#define high 4

int bcd2decimal(uint8_t);
int rtc_read(uint8_t,int);

char t ;

int bcd2decimal(uint8_t t){
	//this function gives 4-bit binary coded decimal to decimal 
	int i=0;
	if (t == 0b00000001)i=1;
	else if (t == 0b00000010)i=2;
	else if (t == 0b00000011)i=3;
	else if (t == 0b00000100)i=4;
	else if (t == 0b00000101)i=5;
	else if (t == 0b00000110)i=6;
	else if (t == 0b00000111)i=7;
	else if (t == 0b00001000)i=8;
	else if (t == 0b00001001)i=9;
	else if (t == 0b00001010)i=10;
	else if (t == 0b00001011)i=11;
	else if (t == 0b00001100)i=12;
	else if (t == 0b00001101)i=13;
	else if (t == 0b00001110)i=14;
	else if (t == 0b00001111)i=15;
	return i;
}

int rtc_read(uint8_t address,int status){
	int k=0;
	int h,l;
	uint8_t temp,temp1,temp2;
	k=0;
	h=0;
	l=0;
	i2c_start();
	i2c_write(0xD0);
	i2c_write(address);
	i2c_start();
	i2c_write(0xD1);
	temp = i2c_read();
	i2c_stop();
	
	if (status == 1)temp = temp & 0b00011111;// if hour address passed truncate the hours
	
	temp1 = temp>>4;//filter first 4-bit
	temp2 = temp & 0b00001111;//filter out second 4-bit
	l = bcd2decimal(temp1);
	h = bcd2decimal(temp2);
	
	k=10*l+h;//convert to decimal
	return k;
}
	
int main()
{
	int i,tempHour,tempMinute,flag3,flag4,flag5;
	int hour = 13;
	int minute = 14;
	int percentage = 30;
	DDRB = 0x23;
	DDRD = 0xF0;
	DDRC = 0x00;
	PORTC = 0x0F; 
	
	_delay_ms(1000);
	Lcd4_Init();
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_String("WELCOME!!!");
	_delay_ms(4000);
	
	//initializing i2c communication
	i2c_init();
	////////////////////////////////
	
	while(1)
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("1:Auto  2:Manual");
		Lcd4_Set_Cursor(2,0);
		Lcd4_Write_String("3:SemiAuto");
		while (1)
		{
			if (ButtonPressed(0,PINC,0,100))
			//////////////Auto Menu//////////
			{
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("Refill starts at");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_Integer(hour);
				Lcd4_Set_Cursor(2,2);
				Lcd4_Write_String(":");
				Lcd4_Set_Cursor(2,3);
				Lcd4_Write_Integer(minute);
				_delay_ms(1000);
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("Refill starts at");
				Lcd4_Set_Cursor(2,0);
				Lcd4_Write_Integer(percentage);
				Lcd4_Set_Cursor(2,2);
				Lcd4_Write_String(" percentage");
				_delay_ms(1000);
				//check rtc and starts process according to the defined time or percentage
				tempHour = rtc_read(0x02,1);
				tempMinute = rtc_read(0x01,0);//can be removed if needed
				if ((getWaterLevel()==0) | ((tempHour == hour) & (tempMinute == minute)))
				{
					PORTB |= 1<<PINB5;
					while (1)
					{
						showWaterLevel();
						if (getWaterLevel()==3)
						{
							PORTB &= ~(1<<PINB5);
							break;
						}
					}
				}
				//
				flag4=0;
				Lcd4_Clear();
				Lcd4_Set_Cursor(1,0);
				Lcd4_Write_String("1:Settings      4:back");
				_delay_ms(1000);
				while (1)
				{
					flag4++;
					if (flag4>60000)
					{
						flag4=0;
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Refill starts at");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_Integer(hour);
						Lcd4_Set_Cursor(2,2);
						Lcd4_Write_String(":");
						Lcd4_Set_Cursor(2,3);
						Lcd4_Write_Integer(minute);
						_delay_ms(1000);
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("Refill starts at");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_Integer(percentage);
						Lcd4_Set_Cursor(2,2);
						Lcd4_Write_String(" percentage");
						_delay_ms(1000);
						Lcd4_Write_String("1:Settings 4:back");
						//check rtc and starts process according to the defined time or percentage
						tempHour = rtc_read(0x02,1);
						tempMinute = rtc_read(0x01,0);//can be removed if needed
						if ((getWaterLevel()==0) | ((tempHour == hour) & (tempMinute == minute)))
						{
							PORTB |= 1<<PINB5;
							while (1)
							{
								showWaterLevel();
								if (getWaterLevel()==3)
								{
									PORTB &= ~(1<<PINB5);
									break;
								}
							}
						}
					}
					if (ButtonPressed(0,PINC,0,100))
					{
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("1:Set time 4:back");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("2:refill %");
						while(1)
						{
							if (ButtonPressed(0,PINC,0,100))
							{
								Lcd4_Clear();
								Lcd4_Set_Cursor(1,0);
								Lcd4_Write_String("1:Change 2:Up");
								Lcd4_Set_Cursor(2,0);
								Lcd4_Write_String("3:Down   4:Back");
								_delay_ms(500);
								flag3 = 0;
								flag5=0;
								while (1)
								{
									flag5++;
									if (flag5>2000)
									{
										flag5 = 0;	
										Lcd4_Clear();
										Lcd4_Set_Cursor(1,0);
										Lcd4_Write_Integer(hour);
										Lcd4_Set_Cursor(1,2);
										Lcd4_Write_String(":");
										Lcd4_Set_Cursor(1,3);
										Lcd4_Write_Integer(minute);
									}										
									if (ButtonPressed(0,PINC,0,100))								
									{
										if (flag3 == 0) flag3 =1;
										else flag3 = 0;
									}
									else if (ButtonPressed(1,PINC,1,100))
									{
										if (flag3==0)
										{
											hour++;
											if (hour>23)
											{
												hour=hour%24;
											}
										} 
										else
										{
											minute++;
											if (minute>59)
											{
												minute=minute%60;
											}
										}
									}
									else if (ButtonPressed(2,PINC,2,100))
									{
										if (flag3==0)
										{
											hour--;
											if (hour<0)
											{
												hour=23;
											}
										} 
										else
										{
											minute--;
											if (minute<0)
											{
												minute = 59;
											}
										}
									}
									else if (ButtonPressed(3,PINC,3,100))
									{
										Lcd4_Clear();
										Lcd4_Set_Cursor(1,0);
										Lcd4_Write_String("1:Set time 4:back");
										Lcd4_Set_Cursor(2,0);
										Lcd4_Write_String("2:refill %");
										break;
									}
								}
							}
							else if (ButtonPressed(1,PINC,1,100))
							{
								Lcd4_Clear();
								Lcd4_Write_String("1:Change 4:Back");
								_delay_ms(500);
								flag5=0;
								while (1)
								{
									flag5++;
									if (flag5>2000)
									{
										flag5=0;
										Lcd4_Clear();
										Lcd4_Write_String(" percentage:");
										Lcd4_Write_Integer(percentage);
									}
																	
									if (ButtonPressed(0,PINC,0,100))
									{
										if (percentage==30)percentage = 60;
										else percentage = 30;
									}
									else if (ButtonPressed(3,PINC,3,100))
									{
										Lcd4_Clear();
										Lcd4_Set_Cursor(1,0);
										Lcd4_Write_String("1:Set time 4:back");
										Lcd4_Set_Cursor(2,0);
										Lcd4_Write_String("2:refill %");
										break;
									}
								}
							}
							else if (ButtonPressed(3,PINC,3,100))
							{
								Lcd4_Clear();
								Lcd4_Write_String("1:Settings 4:back");
								break;	
							}
						}
						
					}
					else if (ButtonPressed(3,PINC,3,100))
					{
						PORTB &= ~1<<PINB5;
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("1:Auto  2:Manual");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("3:SemiAuto");
						break;
					}
				}
				
			}
			else if (ButtonPressed(2,PINC,2,100))
			{
				///////////////SemiAuto Menu///////////////////
				Lcd4_Clear();
				Lcd4_Write_String("1:ON 4:BACK");
				int flag1=0;
				while (1)
				{
					flag1++;
					if (flag1 >10000)
					{
						flag1=0;
						showWaterLevel();
						Lcd4_Clear();
						Lcd4_Write_String("1:ON 4:BACK");
					}
					if (ButtonPressed(0,PINC,0,100))
					{
						PORTB |= 1<<PINB5;
						Lcd4_Clear();
						Lcd4_Write_String("1:OFF");
						int flag2=0;
						while(1){
							flag2++;
							if (flag2 >10000)
							{
								flag2=0;
								Lcd4_Clear();
								Lcd4_Write_String("1:OFF");
								showWaterLevel();
							}
							
							if (ButtonPressed(0,PINC,0,100))
							{
								PORTB &= ~1<<PINB5;
								showWaterLevel();
								break;
							}
						}						
					}
					else if (ButtonPressed(3,PINC,3,100))
					{
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("1:Auto  2:Manual");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("3:SemiAuto");
						break;
					}
				}
				
			}
			else if (ButtonPressed(1,PINC,1,100))
			{
				///////////////Manual Menu///////////////////////
				Lcd4_Clear();
				int s = getWaterLevel();
				if (s==1)
				{
					Lcd4_Clear();
					Lcd4_Set_Cursor(1,0);
					Lcd4_Write_String("W/Level : Low");
					Lcd4_Set_Cursor(2,0);
					Lcd4_Write_String("1:Start 4:Back");
				}
				else if (s==2)
				{
					Lcd4_Clear();
					Lcd4_Set_Cursor(1,0);
					Lcd4_Write_String("W/Level : Medium");
					Lcd4_Set_Cursor(2,0);
					Lcd4_Write_String("1:Start 4:Back");
				}
				else if (s==3)
				{
					Lcd4_Clear();
					Lcd4_Set_Cursor(1,0);
					Lcd4_Write_String("W/Level : High");
					Lcd4_Set_Cursor(2,0);
					Lcd4_Write_String("1:Start 4:Back");
				}
				int t = 0;
				while (1)
				{
					t++;
					if (t==20000)
					{
						t=0;
						Lcd4_Clear();
						int s = getWaterLevel();
						if (s==1)
						{
							Lcd4_Clear();
							Lcd4_Set_Cursor(1,0);
							Lcd4_Write_String("W/Level : Low");
							Lcd4_Set_Cursor(2,0);
							Lcd4_Write_String("1:Start 4:Back");
						}
						else if (s==2)
						{
							Lcd4_Clear();
							Lcd4_Set_Cursor(1,0);
							Lcd4_Write_String("W/Level : Medium");
							Lcd4_Set_Cursor(2,0);
							Lcd4_Write_String("1:Start 4:Back");
						}
						else if (s==3)
						{
							Lcd4_Clear();
							Lcd4_Set_Cursor(1,0);
							Lcd4_Write_String("W/Level : High");
							Lcd4_Set_Cursor(2,0);
							Lcd4_Write_String("1:Start 4:Back");
						}
					}
					if (ButtonPressed(0,PINC,0,100))
					{
						PORTB |= 1<<PINB5;
						Lcd4_Clear();
						Lcd4_Write_String("Motor Started");
						while (1)
						{
							if (getWaterLevel()==1)
							{
								Lcd4_Clear();
								Lcd4_Set_Cursor(1,0);
								Lcd4_Write_String("Motor is ON");
								Lcd4_Set_Cursor(2,0);
								Lcd4_Write_String("W/Level : Low");
							}
							else if (getWaterLevel()==2)
							{
								Lcd4_Clear();
								Lcd4_Set_Cursor(1,0);
								Lcd4_Write_String("Motor is ON");
								Lcd4_Set_Cursor(2,0);
								Lcd4_Write_String("W/Level : Medium");
							}
							else if (getWaterLevel()==3)
							{
								PORTB &= ~1<<PINB5;
								break;
							}
						}
					}
					
					else if (ButtonPressed(3,PINC,3,100))
					{
						Lcd4_Clear();
						Lcd4_Set_Cursor(1,0);
						Lcd4_Write_String("1:Auto  2:Manual");
						Lcd4_Set_Cursor(2,0);
						Lcd4_Write_String("3:SemiAuto");
						break;
					}
				}
				
			}
		}
		
	}
	
	return 0;
}

int getWaterLevel(){
	////indicate current water level of the tank//////////////////
	if (bit_is_clear(PINB,2) & bit_is_clear(PINB,3) & bit_is_clear(PINB,4))
	{
		return 0;
	}
	else if ( bit_is_clear(PINB,3) & bit_is_clear(PINB,4))
	{
		return 1;
	}
	else if (bit_is_clear(PINB,4))
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

void showWaterLevel(){
	///////display current water level of the tank////////////
	if (getWaterLevel()==1)
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("W/Level : Low");
	}
	else if (getWaterLevel()==2)
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("W/Level : Medium");
	}
	else if (getWaterLevel()==3)
	{
		Lcd4_Clear();
		Lcd4_Set_Cursor(1,0);
		Lcd4_Write_String("W/Level : High");
	}
}