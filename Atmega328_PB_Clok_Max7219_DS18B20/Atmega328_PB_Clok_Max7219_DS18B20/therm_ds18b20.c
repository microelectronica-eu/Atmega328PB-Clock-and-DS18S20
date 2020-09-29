
#include <avr/io.h>
//#include "LCD_Code_2x16.h"
#include "therm_ds18b20.h"
//-------------------------------------------------------------------------------------
//void therm_read_temperature(char *buffer)
void therm_read_temperature(void)
{
	// Buffer length must be at least 12bytes long! ["+XXX.XXXX C"]
	uint8_t temperature[2];
	int8_t digit;
	uint16_t decimal;
	//Reset, skip ROM and start temperature conversion
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_CONVERTTEMP);
	//Wait until conversion is complete
	while(!therm_read_bit());
	//Reset, skip ROM and send command to read Scratchpad
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	//Read Scratchpad (only 2 first bytes)
	temperature[0]=therm_read_byte();
	temperature[1]=therm_read_byte();
	therm_reset();
	
	data_sensor_raw_first_byte = temperature[0];
	data_sensor_raw_second_byte = temperature[1];
	//Store temperature integer digits and decimal digits
	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;
	//Store decimal digits
	decimal=temperature[0]&0xf;
	decimal*=THERM_DECIMAL_STEPS_12BIT;
	
	Temperatura_MSB_8bit=digit;
	Temperatura_LSB_16bit=decimal;
	//Format temperature into a string [+XXX.XXXX C]
	//sprintf(buffer, "%+d.%04u C", digit, decimal);
}


void therm_delay(uint16_t delay)
{  //int i,j;
	//for (j=0;j<2;j++)   // 4 because is 4Mhz cloc and this is 0.25us
	//  {
	//for(i=0;i<delay; i++)
	//	{
	//asm volatile("nop");
	while(delay--) asm volatile("nop");  // nop = 125ns at 8MHZ 125ns*10=1250ns= 1.25us
	//	}
	//}
}



uint8_t therm_reset()
{
	uint8_t i;
	//Pull line low and wait for 480uS
	
	THERM_OUTPUT_MODE;
	THERM_LOW;
	therm_delay(us(480));
	//Release line and wait for 60uS
	THERM_INPUT_MODE;
	therm_delay(us(60));
	//Store line value and wait until the completion of 480uS period
	i=(THERM_PIN & (1<<THERM_DQ));
	therm_delay(us(420));
	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;
}


uint8_t therm_read_bit(void)
{
	uint8_t bit=0;
	//Pull line low for 1uS
	
	THERM_OUTPUT_MODE;
	THERM_LOW;
    therm_delay(us(1));
	//Release line and wait for 14uS
	THERM_INPUT_MODE;
	therm_delay(us(14));
	//Read line value
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	//Wait for 45uS to end and return read value
	therm_delay(us(45));
	return bit;
}


uint8_t therm_read_byte(void)
{
	uint8_t i=8, n=0;
	while(i--)
	{
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_read_bit()<<7);
	}
	return n;
}


void therm_write_byte(uint8_t byte)
{
	uint8_t i=8;
	while(i--)
	{
		//Write actual bit and shift one position right to make the next bit ready
		therm_write_bit(byte&1);
		byte>>=1;
	}
}


void therm_write_bit(uint8_t bit)
{
	//Pull line low for 1uS
	
	THERM_OUTPUT_MODE;
	THERM_LOW;
	therm_delay(us(1));
	//If we want to write 1, release the line (if not will keep low)
	if(bit) THERM_INPUT_MODE;
	//Wait for 60uS and release the line
	therm_delay(us(60));
	THERM_INPUT_MODE;
}
