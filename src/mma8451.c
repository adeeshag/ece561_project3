#include <MKL25Z4.H>
#include "mma8451.h"
#include "i2c.h"
#include "delay.h"
#include <math.h>
//#include <arm_math.h>

int16_t acc_X=0, acc_Y=0, acc_Z=0;
float roll=0.0, pitch=0.0;

//mma data ready
extern uint32_t DATA_READY;



//initializes mma8451 sensor
//i2c has to already be enabled
int init_mma()
{
	  //check for device
		if(i2c_read_byte(MMA_ADDR, REG_WHOAMI) == WHOAMI)	{
			
		  Delay(100);
		  //turn on data ready irq; defaults to int2 (PTA15)
		  if(i2c_write_byte(MMA_ADDR, REG_CTRL4, 0x01)<0)
				return 0;
		  Delay(100);
		  //set active 14bit mode and 100Hz (0x19)
		  if(i2c_write_byte(MMA_ADDR, REG_CTRL1, 0x33)<0)
				return 0;
				
		  //enable the irq in the NVIC
		  //NVIC_EnableIRQ(PORTA_IRQn);
		  return 1;
		}
		
		//else error
		return 0;
	
}

int read_full_xyz()
{
	int i;
	uint16_t data[6];
	int16_t temp[3];
	
	i2c_start();
	if(i2c_read_setup(MMA_ADDR , REG_XHI)<0)
		return -1;
	
	for( i=0;i<3;i++)	{
		if(i==2)
		{
			data[i] = i2c_repeated_read(1);
			if(data[i] == 0xFF)
				return -1;
		}
		else
		{
			data[i] = i2c_repeated_read(0);
			if(data[i] == 0xFF)
				return -1;
		}
	}
	
	#if 0 //buggy code
	temp[0] = (int16_t)((data[0]<<8) | (data[1]<<2));
	temp[1] = (int16_t)((data[2]<<8) | (data[3]<<2));
	temp[2] = (int16_t)((data[4]<<8) | (data[5]<<2));
	#else
	temp[0] = (int16_t) (data[0]<<8);
	temp[1] = (int16_t) (data[1]<<8);
	temp[2] = (int16_t) (data[2]<<8);
	#endif
	
	acc_X = temp[0];
	acc_Y = temp[1];
	acc_Z = temp[2];
	return 0;
}

#if 0
void read_xyz(void)
{
	// sign extend byte to 16 bits - need to cast to signed since function
	// returns uint8_t which is unsigned
	acc_X = (int8_t) i2c_read_byte(MMA_ADDR, REG_XHI);
	Delay(100);
	acc_Y = (int8_t) i2c_read_byte(MMA_ADDR, REG_YHI);
	Delay(100);
	acc_Z = (int8_t) i2c_read_byte(MMA_ADDR, REG_ZHI);

}
#endif

void convert_xyz_to_roll_pitch(void) {
	float ax = (acc_X/COUNTS_PER_G) ,
				ay = (acc_Y/COUNTS_PER_G) ,
				az = (acc_Z/COUNTS_PER_G);
	//float temp_atan, temp_asin, temp_180_over_pi;
	//temp_atan = (atan2f(ay,az));
	//temp_180_over_pi = M_180_OVER_PI;
	//temp_asin = asinf(ax);
	
	
	
	
	//arm_mult_f32 (&temp_180_over_pi, &temp_atan, &roll, 1);
	//arm_mult_f32 (&temp_180_over_pi, &temp_asin, &pitch, 1);
	
	//roll = (-roll)+180;
	
	//roll = 180+ (-atan2f(ay, az))*(M_180_OVER_PI);
	
	//pitch = atan2f(ax, sqrtf((ay*ay) + (az*az)))*(M_180_OVER_PI);
	//pitch = asinf(-(ax))*(M_180_OVER_PI);
	
	roll = atan2f(ay, az); // Use radians
	pitch = asinf(-ax);
	
	if(roll<0)
		roll = -roll;
	if(pitch <0)
		pitch = -pitch;
	
}


//mma data ready irq
// void PORTA_IRQHandler()
// {
// 	NVIC_ClearPendingIRQ(PORTA_IRQn);
// 	DATA_READY = 1;	
// }
