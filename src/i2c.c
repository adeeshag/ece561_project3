#include <MKL25Z4.H>
#include "i2c.h"

int i2c_count;
const int max_i2c_count = 160;

//init i2c0
void i2c_init(void)
{
	//clock i2c peripheral and port E
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK);
	
	//set pins to I2C function
	PORTE->PCR[24] |= PORT_PCR_MUX(5);
	PORTE->PCR[25] |= PORT_PCR_MUX(5);
		
	//set to 100k baud
	//baud = bus freq/(scl_div+mul)
 	//~400k = 24M/(64); icr=0x12 sets scl_div to 64
 	I2C0->F = (I2C_F_ICR(0x0) | I2C_F_MULT(0));
	
	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK );
}


//send start sequence
void i2c_start()
{
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
}

//send device and register addresses
int i2c_read_setup(uint8_t dev, uint8_t address)
{
	I2C0->D = dev;			  /*send dev address	*/
	//I2C_WAIT;							/*wait for completion */
	if(i2c_wait()<0)
		return -1;
	
	
	I2C0->D =  address;		/*send read address	*/
	//I2C_WAIT;							/*wait for completion */
	if(i2c_wait()<0)
		return -1;
		
	I2C_M_RSTART;				   /*repeated start */
	I2C0->D = (dev|0x1);	 /*send dev address (read)	*/
	//I2C_WAIT;							 /*wait for completion */
	if(i2c_wait()<0)
		return -1;
	
	I2C_REC;						   /*set to recieve mode */
	return 0;

}

//read a byte and ack/nack as appropriate
uint16_t i2c_repeated_read(uint8_t isLastRead)
{
	uint8_t data;
	
	if(isLastRead)	{
		NACK;								/*set NACK after read	*/
	} else	{
		ACK;								/*ACK after read	*/
	}
	
	data = I2C0->D;				/*dummy read	*/
	//I2C_WAIT;							/*wait for completion */
	if(i2c_wait()<0)
		return 0xFF;
	
	if(isLastRead)	{
		I2C_M_STOP;					/*send stop	*/
	}
	data = I2C0->D;				/*read data	*/

	return  data;					
}



//////////funcs for reading and writing a single byte
//using 7bit addressing reads a byte from dev:address
uint16_t i2c_read_byte(uint8_t dev, uint8_t address)
{
	uint8_t data;
	
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
	I2C0->D = dev;			  /*send dev address	*/
	//I2C_WAIT;							/*wait for completion */
	if(i2c_wait()<0)
		return 0xFF;
	
	I2C0->D =  address;		/*send read address	*/
	//I2C_WAIT;							/*wait for completion */
	if(i2c_wait()<0)
		return 0xFF;
		
	I2C_M_RSTART;				   /*repeated start */
	I2C0->D = (dev|0x1);	 /*send dev address (read)	*/
//	I2C_WAIT;							 /*wait for completion */
	if(i2c_wait()<0)
		return 0xFF;
	
	I2C_REC;						   /*set to recieve mode */
	NACK;									 /*set NACK after read	*/
	
	data = I2C0->D;					/*dummy read	*/
	//I2C_WAIT;								/*wait for completion */
	if(i2c_wait()<0)
		return 0xFF;
	
	I2C_M_STOP;							/*send stop	*/
	data = I2C0->D;					/*read data	*/

	return data;
}



//using 7bit addressing writes a byte data to dev:address
int i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data)
{
	
	I2C_TRAN;							/*set to transmit mode */
	I2C_M_START;					/*send start	*/
	I2C0->D = dev;			  /*send dev address	*/
	//I2C_WAIT;						  /*wait for ack */
	if(i2c_wait()<0)
		return -1;
	
	I2C0->D =  address;		/*send write address	*/
	//I2C_WAIT;
	if(i2c_wait()<0)
		return -1;
		
	I2C0->D = data;				/*send data	*/
	//I2C_WAIT;
	if(i2c_wait()<0)
		return -1;
	I2C_M_STOP;
	return 0;
	
}

int i2c_wait(void)
{
	i2c_count =0;
	while((I2C0->S & I2C_S_IICIF_MASK)==0) {
		if(i2c_count == max_i2c_count)
		{
			i2c_reset();
			return -1;
		}
	i2c_count++; 
	}
			
		I2C0->S |= I2C_S_IICIF_MASK;
		return 0;
}

void i2c_reset(void)
{
	 /* send start signal */ 
	I2C0->C1 &= ~(I2C_C1_IICEN_MASK );
	i2c_start();
	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK );
	
	/* send write command to release SDA line*/ 
	I2C_M_START;
	I2C_TRAN;
	I2C0->D = 0xFF; // Send data
	while((I2C0->S & I2C_S_IICIF_MASK)==0) { };			
		I2C0->S |= I2C_S_IICIF_MASK;
	
	/* Clear arbitration error flag*/  
     I2C0->S |= I2C_S_ARBL_MASK;
#if 0	
	/* Send start*/
	I2C0->C1 &= ~(I2C_C1_IICEN_MASK );
	i2c_start();
	//enable i2c and set to master mode
	I2C0->C1 |= (I2C_C1_IICEN_MASK );
	/* wait until start is sent */
	Delay(100);
#endif
}

