/*
 * Atmega328pb_MAX7219_LED_driver.c
 *
 * Created: 8/30/2020 2:01:14 PM
 * Author : andrei
 */ 

#include <avr/io.h>
#include "max7219_8x8.h"
#include "therm_ds18b20.h"

#include <avr/interrupt.h>
#include <util/delay.h>

// Function Prototypes
uint8_t buffer [NUM_DEVICES*8];
void citire_sensor_temperatura(void);
void math( unsigned int MATH_DATA);
void math2( unsigned int MATH_DATA2);
void fill_dsplay_buffer_with_blank(void);
void addres_location_in_display_buffer(unsigned char addres_disp_buffer );

unsigned char ore; 
unsigned char minute;
unsigned char secunde;
unsigned char Caracter_1;
unsigned char Caracter_2;
unsigned char Caracter_3;
unsigned char Caracter_4;
unsigned char Caracter_5;


ISR(TIMER1_COMPA_vect)
{
	secunde++;
	if(secunde==60)
	{
		secunde=0;
		minute++;
	}
  else if(minute==60)
	{
		minute=0;
		ore++;
	}
else if(ore>23)
	{
		ore=0;
	 }
else if(PINC & (1<<PC2)) //If switch is pressed
	{
		minute=minute+1 ;
	
	}
else if(PINC & (1<<PC3))
	{
	ore=ore+1;
	} 
	 
}

int main(void)
{
    /* Replace with your application code */
	 DDRE= 0xFF ;
	 DDRB= 0xFF ;
	 DDRC= 0xF3; //DDRC= 0xF3;
	 DDRD= 0xFF ;
	 
	 PORTE=0xFF;
	 PORTB=0x00;
	 PORTC=0x00;
	 PORTD=0x00; 
	initSPI();
	initMatrix();
	clearMatrix();
	intitimmer();

	 ore=0;
	 minute=0;
	 secunde=0;
	 Temperatura_MSB_8bit=0;
	 sei();
	 
	 fill_dsplay_buffer_with_blank();
    while (1) 
	    {  

		 if (secunde<20)
		 {
			citire_sensor_temperatura();
			displayBuffer();
		 }
		 else{
			ceas(); 
			displayBuffer(); 
		 }
		 

    }
}





void ceas(void)
{   

	unsigned char Ore_zeci;
	unsigned char Ore_unitati;
	unsigned char Minute_zeci;
	unsigned char Minute_unitati;
	unsigned char Secunde_zeci;
	unsigned char Secunde_unitati;
	
	Ch_RAM_index=0;
	Ore_zeci=(ore/10)+ 0x30  ;
	Ore_unitati=(ore %10)+ 0x30  ;
	Minute_zeci=(minute /10)+ 0x30  ;
	Minute_unitati=(minute %10)+ 0x30  ;
	Secunde_zeci=(secunde/10)+ 0x30  ;
	Secunde_unitati=(secunde%10)+ 0x30  ;
	print_char(Ore_zeci);
	print_char(Ore_unitati);
	print_char(':');
	print_char(Minute_zeci);
	print_char(Minute_unitati);
	print_char(':');
	print_char(Secunde_zeci);
	print_char(Secunde_unitati);

}

void citire_sensor_temperatura(void)
{
	therm_read_temperature();
	if(data_sensor_raw_second_byte > 250)
	{ // temperatura negativa
		// FFFF - Sensor data +1 = negative value ;
		int8_t digit;
		uint16_t decimal;
		unsigned int local_LSB ;
		unsigned int local_MSB ;
		
		local_LSB=( 0xFF - data_sensor_raw_first_byte )+1 ;
		local_MSB= 0xFF - data_sensor_raw_second_byte;
		
		digit=local_LSB>>4;
		digit|=(local_MSB & 0x7)<<4;
		decimal=local_LSB & 0xf;
		decimal*=THERM_DECIMAL_STEPS_12BIT;
		math(digit); // TEMPERATURA INTREAGA
		math2(decimal); // TEMPERATURA DUPA VIRGULA
		Ch_RAM_index=0;
		print_char('-');
		print_char(Caracter_1);
		print_char(Caracter_2);
		print_char('.');
		print_char(Caracter_3);
		print_char(Caracter_4);
		print_char(Caracter_5);
		print_char(' ');
		
	}
	else {
		math(Temperatura_MSB_8bit);
		math2(Temperatura_LSB_16bit);
		
        Ch_RAM_index=0;
		print_char('+');
		print_char(Caracter_1);
		print_char(Caracter_2);
		print_char('.');
		print_char(Caracter_3);
		print_char(Caracter_4);
		print_char(Caracter_5);
		print_char(' ');
		
		
	}
	
	
}

void math( unsigned int MATH_DATA)
{
	unsigned int mat	  ;
	unsigned int zecemii ;
	unsigned char mii	   ;
	unsigned char sute ;
	unsigned char zeci ;
	unsigned char unitati;
	//unsigned char  rest  ;

	// unsigned char  a1	   ;
	unsigned char  b1    ;
	unsigned char  c1    ;
	unsigned char  d1    ;
	unsigned char  e1  ;
	
	mat= MATH_DATA ;
	
	
	zecemii=mat/10000 ;
	mat=mat%10000 ;
	mii= mat/1000;
	mat= mat%1000 ;
	sute= mat/100 ;
	mat= mat%100 ;
	zeci=	mat/10  ;
	unitati= mat%10  ;
	//rest   = mat%1 ;
	
	//a1 = zecemii + 0x30 ;
	b1 = mii+0x30 ;
	c1 = sute+0x30  ;
	d1 = zeci+0x30 ;
	e1 = unitati+0x30;
	
	//write_inst(locatie); //0xc6
	//print_char(b1) ;
	//print_char(c1) ;
	Caracter_1=d1 ;
	Caracter_2=e1;
	
}

void math2( unsigned int MATH_DATA2)
{
	unsigned int mat	  ;
	unsigned int zecemii ;
	unsigned char mii	   ;
	unsigned char sute ;
	unsigned char zeci ;
	unsigned char unitati;
	//unsigned char  rest  ;

	// unsigned char  a1	   ;
	unsigned char  b1    ;
	unsigned char  c1    ;
	unsigned char  d1    ;
	unsigned char  e1  ;
	
	mat= MATH_DATA2 ;
	
	
	zecemii=mat/10000 ;
	mat=mat%10000 ;
	mii= mat/1000;
	mat= mat%1000 ;
	sute= mat/100 ;
	mat= mat%100 ;
	zeci=	mat/10  ;
	unitati= mat%10  ;
	//rest   = mat%1 ;
	
	//a1 = zecemii + 0x30 ;
	b1 = mii + 0x30 ;
	c1 = sute + 0x30 ;
	d1 = zeci + 0x30;
	e1 = unitati + 0x30;
	
	//write_inst(locatie); //0xc6
	Caracter_3=b1;
	Caracter_4=c1;
	Caracter_5=d1;
	//print_char(e1) ;
	
}

void intitimmer(void)
{
	 TCCR1B = (1<<CS12|1<<WGM12);   //cs12= 8Mhz/256=31250   1000ms=31250
	 OCR1A = 31250-1; 
	 TIMSK1 = 1<<OCIE1A;
	
}

void initSPI(void)
{
	
	DDRC |= (1<<DDC1) | (0<<DDC0);  //  PC1-CLK
	DDRE |= (1<<DDE3);              //  PE3 MOSI
	
	SPCR1 |= (1 << MSTR1);          // Clockmaster
	SPCR1 |= (1 << SPE1);           // Enable SPI
}

void writeByte(uint8_t byte)
{
	
	SPDR1 = byte;					//Load byte to Data register
	while(!(SPSR1 & (1<<SPIF1)));; 	// Wait for transmission complete while(!(SPSR & (1<<SPIF)));
 
}


// Sends word through SPI
void writeWord(uint8_t address, uint8_t data)
{
	writeByte(address);	// Write first byte
	writeByte(data);      // Write Second byte
}


// Initializes all cascaded devices
void initMatrix(void)
{
	uint8_t i;	// Var used in for loops
	
	// // Decode mode: none
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)   // Loop through number of cascaded devices
	{
		writeByte(0x09); //
		writeByte(0x00); //
	}
	SLAVE_DESELECT;

	// Set display brighness
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)   // Loop through number of cascaded devices
	{
		writeByte(0x0A); // Select Intensity register
		writeByte(0x01); // Set brightness 1= low 7 = high
	}
	SLAVE_DESELECT;

	
	// Set display refresh
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0B); // Select Scan-Limit register
		writeByte(0x07); // Select columns 0-7
	}
	SLAVE_DESELECT;

	
	// Turn on the display
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0C); // Select Shutdown register
		writeByte(0x01); // Select Normal Operation mode
	}
	SLAVE_DESELECT;

	
	// Disable Display-Test
	SLAVE_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		writeByte(0x0F); // Select Display-Test register
		writeByte(0x00); // Disable Display-Test
	}
	SLAVE_DESELECT;
}

// Clears all columns on all devices
void clearMatrix(void)
{
	for(uint8_t x = 1; x < 9; x++) // for all columns
	{
		SLAVE_SELECT;
		for(uint8_t i = 0; i < NUM_DEVICES; i++)
		{
			writeByte(x);    // Select column x
			writeByte(0x00); // Set column to 0
		}
		SLAVE_DESELECT;
	}
}

void addres_location_in_display_buffer(unsigned char addres_disp_buffer )
{

     Ch_RAM_index=((addres_disp_buffer-1)*8);

}


void fill_dsplay_buffer_with_blank(void)
{
	unsigned char m ;
	 Ch_RAM_index=0;
	for(m=0;m<8;m++)
	{
		print_char(' ');
	}
	
	 
}
void displayBuffer(void)
{
	unsigned char rand_caracter ;
	
	
	for(rand_caracter=1; rand_caracter<NUM_DEVICES+1; rand_caracter++)
	{
	SLAVE_SELECT;

	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 7*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 6*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 5*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 4*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 3*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 2*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 1*8]);
	writeWord(rand_caracter, Caracter_RAM[(rand_caracter-1) + 0*8]);   //k
	SLAVE_DESELECT;
	}



}

void delay (int milisecond)
{
	unsigned  int i,j ;
	for(j=0;j< milisecond;j++)
	{
		for(i=0;i<1000;i++)
		{
			asm("nop");
		}
	}
}


