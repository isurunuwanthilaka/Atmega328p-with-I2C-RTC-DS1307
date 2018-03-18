#ifndef ButtonPress
#define ButtonPress
#include <avr/io.h>

char ButtonPressed(int buttonNumber,unsigned char pinOfButton, unsigned char portBit, int confidenceLevel);

char Pressed[numberOfButtons];
int Pressed_Confidence_Level[numberOfButtons];
int Released_Confidence_Level[numberOfButtons];

char ButtonPressed(int buttonNumber, unsigned char pinOfButton, unsigned char portBit, int confidenceLevel)
{
	if(bit_is_clear(pinOfButton,portBit))
	{
		Pressed_Confidence_Level[buttonNumber]++;
		Released_Confidence_Level[buttonNumber] = 0;
		if(Pressed_Confidence_Level[buttonNumber]> confidenceLevel)
		{
			if (Pressed[buttonNumber]==0)
			{
				Pressed[buttonNumber] = 1;
				return 1;
			}
			Pressed_Confidence_Level[buttonNumber] = 0;
		}
	}
	else
	{
		Released_Confidence_Level[buttonNumber]++;
		Pressed_Confidence_Level[buttonNumber] = 0;
		if (Released_Confidence_Level> confidenceLevel)
		{
			Pressed[buttonNumber] = 0;
			Released_Confidence_Level[buttonNumber] = 0;
		}
		
	}
	return 0;
	
}

#endif