#include <MKL25Z4.H>
#include "LEDs.h"
#include "gpio_defs.h"
#include "delay.h"
#include "mma8451.h"




void Init_RGB_LEDs(void) {
	// Enable clock to ports B and D
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;;
	
	// Make 3 pins GPIO
	PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
	PTD->PDDR |= MASK(BLUE_LED_POS);
	
	Control_RGB_LEDs(1, 1, 1);
	Delay(10);
	Control_RGB_LEDs(0, 0, 0);
}

void Control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	if (red_on) {
			PTB->PCOR = MASK(RED_LED_POS);
	} else {
			PTB->PSOR = MASK(RED_LED_POS); 
	}
	if (green_on) {
			PTB->PCOR = MASK(GREEN_LED_POS);
	}	else {
			PTB->PSOR = MASK(GREEN_LED_POS); 
	} 
	if (blue_on) {
			PTD->PCOR = MASK(BLUE_LED_POS);
	}	else {
			PTD->PSOR = MASK(BLUE_LED_POS); 
	}
}	

void Light_LEDs(void) {
	
	if((pitch < DEG15_RADIANS) && (roll < DEG15_RADIANS))
			Control_RGB_LEDs(0, 1, 0);
	else if(((pitch >= DEG15_RADIANS) && (pitch < DEG30_RADIANS))
						||((roll >= DEG15_RADIANS) && (roll < DEG30_RADIANS)))
			Control_RGB_LEDs(1, 1, 0);
	else if(((pitch >= DEG30_RADIANS) || (roll >= DEG30_RADIANS)))
			Control_RGB_LEDs(1, 0, 0);
	else
			Control_RGB_LEDs(0, 0, 0);
		
	Delay(T_ON);
	Control_RGB_LEDs(0, 0, 0);
	
	//Delay(T_OFF);

}
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************   
