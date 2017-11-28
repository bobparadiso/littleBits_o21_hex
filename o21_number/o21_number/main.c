/*
 * main.c
 *
 * Created: 8/23/2017 5:07:16 PM
 * Author : bobparadiso
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

const uint8_t bitmap[16] = {
	0b0111111,//0
	0b0000110,//1
	0b1011011,//2
	0b1001111,//3
	0b1100110,//4
	0b1101101,//5
	0b1111101,//6
	0b0000111,//7
	0b1111111,//8
	0b1101111,//9
	0b1110111,//a
	0b1111100,//b
	0b0111001,//c
	0b1011110,//d
	0b1111001,//e
	0b1110001,//f
};

//
void setDisplayHex(uint8_t val)
{
	uint8_t d1 = bitmap[val & 0x0f]; //right digit
	uint8_t d2 = bitmap[(val & 0xf0) >> 4]; //left digit

	//macro to create a mask depending on if the given bit is set in the source
	#define m(dstPos,src,srcPos) (((src & _BV(srcPos)) >> srcPos) * _BV(dstPos))
	
	//doing it this way so I can directly set each port to what its state should be
	//uses both digits at once since PORTD is used in both digits
	PORTB = m(0,d2,4) | m(2,d2,6) | m(3,d2,5) | m(4,d2,0) | m(5,d2,1);
	PORTC = m(2,d1,6) | m(3,d1,5) | m(4,d1,0);
	PORTD = m(0,d1,1) | m(2,d1,2) | m(3,d1,3) | m(4,d1,4) | m(6,d2,2) | m(7,d2,3);
}

//
void adc_init()
{
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); //enable ADC and set 128 prescale
	ADMUX = _BV(REFS0); //AVCC with external capacitor at AREF pin
}

//
uint16_t adc_read(uint8_t adcx)
{
	ADMUX = (ADMUX & 0xf0) | adcx; //select channel
	ADCSRA |= _BV(ADSC); //start conversion
	
	while (ADCSRA & _BV(ADSC)); //spin till conversion is complete
	
	return(ADC);
}

//
int main(void)
{
    wdt_reset();
	wdt_enable(WDTO_120MS);
	
	adc_init();
	
	//set display pins to output
	DDRB = 0b00111101;
	DDRC = 0b00011100;
	DDRD = 0b11111111;
		
    //main loop
	float val = 0;
	while (1)
    {
	    wdt_reset();
		float target = adc_read(0) >> 2;
		val = val + ((target - val) * 0.1f); //lerp towards target
		setDisplayHex(val + 0.5f);
		_delay_ms(10);
    }
}
