#include <avr/io.h>
#include <stdint.h>

void i2c_init()
{
	TWBR = 0x62;//baud rate
	TWCR = 1<<TWEN;//enable i2c
	TWSR = 0x00;//seting prescaler to zero	
}

void i2c_start()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA) | (1<<TWEA);//start condition
	while(!(TWCR &(1<<TWINT)));	
}
void i2c_write(char Data)
{
	TWDR = Data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
}

char i2c_read()
{
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}

void i2c_stop()
{
	
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<TWSTO);
	while(!(TWCR & (1<<TWINT)));
}
