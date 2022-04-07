/*
 * CV-Ketel.c
 *
 * Created: 05/04/2022 15:25:24
 * Author : Robin Kolk
 */ 

//Define includes
#define AVR_ATmega32
#define  _OPEN_SYS_ITOA_EXT
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/interrupt.h>
#include "avr/iom32.h"
//F_CPU
#define F_CPU 1000000UL // oscillator frequency

//Define lcd
#define lcdcommand(d) (lcdwrite((d),PORTD&0xDF));  // RS low
#define lcdputc(d) (lcdwrite((d),PORTD|0x20));    // RS high

//Global var
int adcResult[4]; // 4 digit number
int IngesteldeWaarde = 20;
//Set lcd to write data
void lcdwrite(char d, char type)
{
	PORTD = type;			// RS low or high
	PORTD = PORTD|0x80;		// make E high
	PORTC = d;				// assign data
	PORTD = PORTD & 0x7F;	// make E low
	_delay_us(1520);		// wait 1,52 ms worst case delay
}


// initialiseert het LCD display
void initlcd(void)
{
	DDRC = 0xFF;		// port D: 8-bit data
	DDRD = 0xFF;		// pin 0 and 1 port B: RS en E
	_delay_ms(15);		// delay of 15 ms
	lcdcommand(0x38);   // 8-bits, 2 lines, 5x8 font
	lcdcommand(0x38);   // 8-bits, 2 lines, 5x8 font
	lcdcommand(0x38);   // 8-bits, 2 lines, 5x8 font
	lcdcommand(0x38);   // 8-bits, 2 lines, 5x8 font
	lcdcommand(0x0C);   // display on, cursor off, blink off
	lcdcommand(0x06);   // move cursor right
	lcdcommand(0x01);   // clear display
}


// Zorgt ervoor dat de cursor verplaatst kan worden tussen de regels 1 t/m 4.
void moveCursor(int row, int column){
	if(row == 1){
		lcdcommand(0x80); // Begin 1ste rij
		}else if(row == 2){
		lcdcommand(0xC0); // Begin 2de rij
		}else if(row == 3){
		lcdcommand(0x90); // Begin 3de rij
		}else if(row == 4){
		lcdcommand(0xD0); // Begin 4de rij
	}
	
	int i = 1;
	while(i < column){
		lcdcommand(0x14);
		i++;
	}
}
void putString(char * string){  //(text, row, column)
	
	int j = 0;

	
	while(string[j] != '\0'){

		
		lcdputc(string[j]);
		j++;
	}
}

int main(void)
{
	initlcd();
	
	//Configure the ADC
	ADCSRA |= 1<<ADPS2; // ADC prescaler 16 division factor
	ADMUX |= 1<<ADLAR; // ADC Data on 8 Bit nummer
	//ADMUX |= 0<<ADLAR; //
	ADMUX |= 1<<REFS0; // AVCC external capacitoy at AREF pin
	ADCSRA |= 1<<ADIE; // ADC Interrupt enable 
	ADCSRA |= 1<<ADEN; // Turn on ADC control and status register A 

	sei(); //Global Enable interrupts

	ADCSRA |= 1<<ADSC; // // ADC Start first conversion
	
	DDRB = 0xff;
	
	while (1)
	{
	}
	    return (0);
}

ISR(ADC_vect) // interrupt service routine 
{
	char waarde[4]; 
	
	int adch = 0.15* ADCH; //waarde van ADCH in var zetten
	
	if (adch <= 15) // zorgen dat het niet onder een tempratuur kan komen
	{
		adch =15;
	}
	
	if (adch <= IngesteldeWaarde) // onder de ingestelde waarde led laten branden
	{
		PORTB = 0x00;
	} 
	else // anders leds uit
	{
		PORTB = 0xff;
	}
	
	itoa(adch, adcResult, 10); // Base10 nummer leesbaar voor mensen maakt eem STRING
	
	//int x = atoi(adcResult); // maakt een int
	
	moveCursor(1,1);  // Print waarde op LCD scherm eerste rij, eerste colum
	putString("tempatuur NU:");
	moveCursor(2,1);
	putString(adcResult);
	putString(" "); // exstra nul weg halen
	
	moveCursor(3,1);  // Print waarde op LCD scherm eerste rij, eerste colum
	putString("Ingestelde waarde"); 
	itoa(IngesteldeWaarde,waarde,10),
	moveCursor(4,1);
	putString(waarde);
	
	ADCSRA |= 1<<ADSC; // Start next conversion
}
