/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "LEDs.h"
#include "timers.h"
#include "delay.h"
//#include "ADC.h"
#include "config.h"
#include "i2c.h"
#include "mma8451.h"

void Init_Debug_Signals(void) {
	// Enable clock to ports E
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Make 3 pins GPIO
	PORTE->PCR[DEBUG_RUNNING_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[DEBUG_RUNNING_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	PTE->PDDR |= MASK(DEBUG_RUNNING_POS);
	
	PTE->PSOR |= MASK(DEBUG_RUNNING_POS);
}	
	

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {


// Regular code
//		Init_Debug_Signals();
		Init_RGB_LEDs();
		//Init_ADC();
		i2c_init();
	
						/* init i2c	*/
#if 1
	if(!init_mma())
	{
		Control_RGB_LEDs(1,0,0);
		while(1);
		// Signal error condition
	}
#endif
			
	
	

#if 1

	// Allow low leakage stop mode
		SMC->PMPROT = SMC_PMPROT_ALLS_MASK; // 
		// Enable low-leakage stop mode and regular run mode
		SMC->PMCTRL = SMC_PMCTRL_STOPM(3) | SMC_PMCTRL_RUNM(0);
		SMC->STOPCTRL = SMC_STOPCTRL_PSTOPO(0) | SMC_STOPCTRL_VLLSM(3);
	
		// Enable LLWU
		// allow LPTMR0 to wake LLWU
		LLWU->ME |= LLWU_ME_WUME0_MASK;
	
	// Enable stop mode (deep sleep)
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#endif
	
		Init_LPTMR();
		Start_LPTMR();
	
		__enable_irq();
	
		// work is in interrupt
		while (1) {
		//	PTE->PCOR |= MASK(DEBUG_RUNNING_POS);
			
#if USE_SLEEP_MODES
			__wfi() ; // then go to sleep	
#endif
		//	PTE->PSOR |= MASK(DEBUG_RUNNING_POS);
		}
}

// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
