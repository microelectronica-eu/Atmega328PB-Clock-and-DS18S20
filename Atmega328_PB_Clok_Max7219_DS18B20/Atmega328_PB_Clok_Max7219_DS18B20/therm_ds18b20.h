#include <avr/io.h> 
#include <stdio.h>


#ifndef F_CPU
#define F_CPU 8000000UL 		     //System clock speed in Hz 
#endif

#define LOOP_CYCLES 6				//Number of cycles that the loop takes
#define us(num) (num/(LOOP_CYCLES*(1/(F_CPU/1000000.0))))
//-------------------------------------------------------------------
#define THERM_PIN PIND
#define THERM_DQ PD0
/* Utils */
#define THERM_INPUT_MODE  DDRD &=~(1<<PD0)
#define THERM_OUTPUT_MODE DDRD |=(1<<PD0)

#define THERM_LOW  PORTD &=~(1<<PD0)
#define THERM_HIGH PORTD |=(1<<PD0)
//---------------------------------------------------------------------

//-------------------------------------------------------------------
/* list of these commands translated into C defines:*/
#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec
/* constants */
#define THERM_DECIMAL_STEPS_12BIT 625 //.0625
//----------------------------------------------------------
void therm_delay(uint16_t delay);
void therm_delay2(unsigned int data_us);
uint8_t therm_reset();
uint8_t therm_read_bit(void);
uint8_t therm_read_byte(void);
void therm_write_byte(uint8_t byte);
void therm_write_bit(uint8_t bit);
//
void therm_read_temperature(void);

unsigned int Temperatura_MSB_8bit  ;
unsigned int  Temperatura_LSB_16bit;

unsigned int data_sensor_raw_first_byte;
unsigned int data_sensor_raw_second_byte;
