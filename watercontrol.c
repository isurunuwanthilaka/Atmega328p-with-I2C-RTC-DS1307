#define numberOfButtons 4
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "ButtonPress.h"
#include "i2c.h"

#define rs PINB0    
#define en PINB1    
#define Low 2
#define medium 3
#define high 4

unsigned int s1[]={10,21,17,17,17,10,4,0};
unsigned int s2[]={0,10,0,4,4,17,10,4};
unsigned int s3[]={0,10,0,4,4,0,14,17};
unsigned int s4[]={1,3,5,9,9,11,27,24};
unsigned int s5[]={0,17,10,17,4,0,14,17};
unsigned int s6[]={0,10,0,4,4,0,31,0};
unsigned int s7[]={10,0,4,0,14,17,17,14};
unsigned int s8[]={0,10,0,31,17,14,0,0};

int bcd2decimal(uint8_t);
int rtc_read(uint8_t,int);

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
	DDRB = 0x03;
	DDRD = 0xF0;
	DDRC = 0b0000001;
	PORTC = 0b0001110; 
	PORTB &= ~(1<<PINB2 | 1<<PINB3 | 1<<PINB4);
	PORTB |=1<<PINB7;
	_delay_ms(400);
	start();
	clearScreen();
	Send_A_String("WELCOME!!!");
	_delay_ms(100);
	
	//initializing i2c communication
	i2c_init();
	////////////////////////////////
	
	while(1)
	{
		clearScreen();
		cut("1:Auto  2:Manual3:SemiAuto");
		while (1)
		{
			if (ButtonPressed(0,PINC,1,100))
			//////////////Auto Menu//////////
			{
				clearScreen();
				Send_A_String("Refill starts at");
				clearScreen();
				Send_An_Integer(hour);
				Send_A_String(":");
				Send_An_Integer(minute);
				_delay_ms(10);
				clearScreen();
				Send_A_String("Refill starts at");
				clearScreen();
				Send_An_Integer(percentage);
				Send_A_String(" percentage");
				//check rtc and starts process according to the defined time or percentage
				tempHour = rtc_read(0x02,1);
				tempMinute = rtc_read(0x01,0);//can be removed if needed
				if ((getWaterLevel()==0) | ((tempHour == hour) & (tempMinute == minute)))
				{
					PORTC |= 1<<PINC0;
					while (1)
					{
						showWaterLevel();
						if (getWaterLevel()==3)
						{
							PORTC &= ~(1<<PINC0);
							break;
						}
					}
				}
				//
				flag4=0;
				clearScreen();
				cut("1:Settings      4:back");
				while (1)
				{
					flag4++;
					if (flag4>20000)
					{
						flag4=0;
						clearScreen();
						Send_A_String("Refill starts at");
						clearScreen();
						Send_An_Integer(hour);
						Send_A_String(":");
						Send_An_Integer(minute);
						_delay_ms(10);
						clearScreen();
						Send_A_String("Refill starts at");
						clearScreen();
						Send_An_Integer(percentage);
						Send_A_String(" percentage");
						clearScreen();
						cut("1:Settings      4:back");
						//check rtc and starts process according to the defined time or percentage
						tempHour = rtc_read(0x02,1);
						tempMinute = rtc_read(0x01,0);//can be removed if needed
						if ((getWaterLevel()==0) | ((tempHour == hour) & (tempMinute == minute)))
						{
							PORTC |= 1<<PINC0;
							while (1)
							{
								showWaterLevel();
								if (getWaterLevel()==3)
								{
									PORTC &= ~(1<<PINC0);
									break;
								}
							}
						}
					}
					if (ButtonPressed(0,PINC,1,100))
					{
						clearScreen();
						cut("1:Set time 2:refill % 4:back");
						while(1)
						{
							if (ButtonPressed(0,PINC,1,100))
							{
								clearScreen();
								cut("1:Change 2:Up   3:Down   4:Back");
								_delay_ms(10);
								flag3 = 0;
								flag5=0;
								while (1)
								{
									flag5++;
									if (flag5>490)
									{
										flag5 = 0;	
										clearScreen();
										Send_An_Integer(hour);
										Send_A_String(":");
										Send_An_Integer(minute);
									}										
									if (ButtonPressed(0,PINC,1,100))								
									{
										if (flag3 == 0) flag3 =1;
										else flag3 = 0;
									}
									else if (ButtonPressed(1,PINC,2,100))
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
									else if (ButtonPressed(2,PINC,3,100))
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
									else if (ButtonPressed(3,PINB,7,100))
									{
										clearScreen();
										cut("1:Set time 2:refill % 4:back");
										break;
									}
								}
							}
							else if (ButtonPressed(1,PINC,2,100))
							{
								clearScreen();
								cut("1:Change        4:Back");
								_delay_ms(20);
								flag5=0;
								while (1)
								{
									flag5++;
									if (flag5>490)
									{
										flag5=0;
										clearScreen();
										Send_A_String(" percentage:");
										Send_An_Integer(percentage);
									}
																	
									if (ButtonPressed(0,PINC,1,100))
									{
										if (percentage==30)percentage = 60;
										else percentage = 30;
									}
									else if (ButtonPressed(3,PINB,7,100))
									{
										clearScreen();
										cut("1:Set time 2:refill % 4:back");
										break;
									}
								}
							}
							else if (ButtonPressed(3,PINB,7,100))
							{
								clearScreen();
								cut("1:Settings      4:back");
								break;	
							}
						}
						
					}
					else if (ButtonPressed(3,PINB,7,100))
					{
						PORTC &= ~1<<PINC0;
						clearScreen();
						cut("1:Auto  2:Manual3:SemiAuto");
						break;
					}
				}
				
			}
			else if (ButtonPressed(1,PINC,2,100))
			{
				///////////////SemiAuto Menu///////////////////
				clearScreen();
				cut("1:ON            4:BACK");
				int flag1=0;
				while (1)
				{
					flag1++;
					if (flag1 >10000)
					{
						flag1=0;
						showWaterLevel();
						clearScreen();
						cut("1:ON            4:BACK");
					}
					if (ButtonPressed(0,PINC,1,100))
					{
						PORTC |= 1<<PINC0;
						clearScreen();
						cut("1:OFF");
						int flag2=0;
						while(1){
							flag2++;
							if (flag2 >10000)
							{
								flag2=0;
								clearScreen();
								cut("1:OFF");
								showWaterLevel();
							}
							
							if (ButtonPressed(0,PINC,1,100))
							{
								PORTC &= ~1<<PINC0;
								showWaterLevel();
								break;
							}
						}						
					}
					else if (ButtonPressed(3,PINB,7,100))
					{
						clearScreen();
						cut("1:Auto  2:Manual3:SemiAuto");
						break;
					}
				}
				
			}
			else if (ButtonPressed(2,PINC,3,100))
			{
				///////////////Manual Menu///////////////////////
				clearScreen();
				int s = getWaterLevel();
				if (s==1)
				{
					cut("W/Level : Low   1: start 4:Back");
				}
				else if (s==2)
				{
					cut("W/Level : Medium1: start 4:Back");
				}
				else if (s==3)
				{
					cut("W/Level : High  1: start 4:Back");
				}
				int t = 0;
				while (1)
				{
					t++;
					if (t==20000)
					{
						t=0;
						clearScreen();
						int s = getWaterLevel();
						if (s==1)
						{
							cut("W/Level : Low   1: start 4:Back");
						}
						else if (s==2)
						{
							cut("W/Level : Medium1: start 4:Back");
						}
						else if (s==3)
						{
							cut("W/Level : High  1: start 4:Back");
						}
					}
					if (ButtonPressed(0,PINC,1,100))
					{
						PORTC |= 1<<PINC0;
						clearScreen();
						cut("Motor Started");
						while (1)
						{
							if (getWaterLevel()==1)
							{
								clearScreen();
								cut("Motor is ON     W/Level : Low");
							}
							else if (getWaterLevel()==2)
							{
								clearScreen();
								cut("Motor is ON     W/Level : Medium");
							}
							else if (getWaterLevel()==3)
							{
								PORTC &= ~1<<PINC0;
								break;
							}
						}
					}
					
					else if (ButtonPressed(3,PINB,7,100))
					{
						clearScreen();
						cut("1:Auto  2:Manual3:SemiAuto");
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
	if (bit_is_clear(PINB,2) & bit_is_clear(PINB,4) & bit_is_clear(PINB,3))
	{
		return 0;
	}
	else if ( bit_is_clear(PINB,4) & bit_is_clear(PINB,3))
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
		clearScreen();
		cut("W/Level : Low");
	}
	else if (getWaterLevel()==2)
	{
		clearScreen();
		cut("W/Level : Medium");
	}
	else if (getWaterLevel()==3)
	{
		clearScreen();
		cut("W/Level : High");
	}
}