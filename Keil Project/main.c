                        //Include headers Section
#include "tm4c123gh6pm.h"
#include "math.h"
#include "string.h"
//---------------------------------------------------------------------------
                        //Global Variables to be used
double dist_travelled = 0;
double old_dist = 0;
char dist_buffer[16];												
												
								
//---------------------------------------------------------------------------
                        //Function declaration
//---------------------------------------------------------------------------
void portF(void);
void portE(void);
void portD(void);
void portC(void);
void portB(void);
void portA(void);
//---------------------------------------------------------------------------
void Systick_init(void);
void delay_ms(int i);
//---------------------------------------------------------------------------
void update_7_segment(void);
void send_dist_to_segment(void);
void pinA3_High_output(void);
void pinA3_Low_output(void);
void light_led_after_100m(void);
void red_led_output(void);

//--------------------------------------------------------------------------
                        // Main Function
int main(){
	
}




//--------------------------------------------------------------------------
                        //Function Definition

//PortF Intialize to use with Tiva Board LEDs
void portF(void){
        SYSCTL_RCGCGPIO_R |= 0x20;                      // enable clock to PORTF
        while((SYSCTL_PRGPIO_R & 0x20) == 0){}          // waiting for port F stabilization
        GPIO_PORTF_LOCK_R = 0x4C4F434B;                 // unlock port F
        GPIO_PORTF_CR_R |= 0x0E;                        // unlock port F
        GPIO_PORTF_DIR_R |= 0x0E;                       // output pins LEDs
        GPIO_PORTF_DEN_R |= 0x0E;                       // Digital Pins
        GPIO_PORTF_AMSEL_R &= ~0x0E;                    // Analog select ; clear 3 pins of LEDs of analog property
        GPIO_PORTF_AFSEL_R &= ~0x0E;                    // Alternative function or General Purpose Selection; here we selected GPIO
        GPIO_PORTF_PCTL_R &= ~0x0000FFF0;               // Definition of the alternative function of the pin; here we clear the leds pins
        GPIO_PORTF_DATA_R &= ~0x0E;                     // clear data from LEDs.
}

//PortE Intialize
void portE(void){
        SYSCTL_RCGCGPIO_R |= 0x10;
        while((SYSCTL_PRGPIO_R & 0x10) == 0){}
        GPIO_PORTE_DEN_R = 0x01;
        GPIO_PORTE_DIR_R = 0x00;
        GPIO_PORTE_AFSEL_R = 0x00;
        GPIO_PORTE_AMSEL_R = 0x00;
        GPIO_PORTE_PCTL_R = 0x00000000;
        GPIO_PORTE_DATA_R = 0x00;
}

//PortD Intialize
void portD(void){
        SYSCTL_RCGCGPIO_R |= 0x08;
        while((SYSCTL_PRGPIO_R & 0x08) == 0){}
        GPIO_PORTD_LOCK_R = 0x4C4F434B;
        GPIO_PORTD_DEN_R |= 0x03;    // Digital Pin D0,D1
        GPIO_PORTD_DIR_R |= 0x03;    // Digital outputD0,D1
        GPIO_PORTD_AFSEL_R = 0x00000000;
        GPIO_PORTD_AMSEL_R = 0x00000000;
        GPIO_PORTD_PCTL_R = 0x00000000;
        GPIO_PORTD_DATA_R = 0x00000000;
}

//PortC Intialize
void portC(void){
        SYSCTL_RCGCGPIO_R |= 0x04;
        while((SYSCTL_PRGPIO_R & 0x04) == 0){}
        GPIO_PORTC_DEN_R = 0x00;
        GPIO_PORTC_DIR_R = 0x00;
        GPIO_PORTC_AFSEL_R = 0x00;
        GPIO_PORTC_AMSEL_R = 0x00;
        GPIO_PORTC_PCTL_R = 0x00000000;
        GPIO_PORTC_DATA_R = 0x00;
}

//PortB Intialize
void portB(void){
        SYSCTL_RCGCGPIO_R |= 0x02;
        while((SYSCTL_PRGPIO_R & 0x02) == 0){}
        GPIO_PORTB_DEN_R = 0xFF; //for LCD
        GPIO_PORTB_DIR_R = 0xFF;
        GPIO_PORTB_AFSEL_R = 0x00;
        GPIO_PORTB_AMSEL_R = 0x00;
        GPIO_PORTB_PCTL_R = 0x00000000;
        GPIO_PORTB_DATA_R = 0x00;
}

//PortA Intialize
void portA(void){
        SYSCTL_RCGCGPIO_R |= 0x01;
        while((SYSCTL_PRGPIO_R & 0x01) == 0){}
        GPIO_PORTA_DEN_R = 0xEC; //A5 , A6 , A7 , A3,A2
        GPIO_PORTA_DIR_R = 0xEC;
        GPIO_PORTA_AFSEL_R = 0x00;
        GPIO_PORTA_AMSEL_R = 0x00;
        GPIO_PORTA_PCTL_R = 0x00000000;
        GPIO_PORTA_DATA_R = 0x00;
}

//Update 7 Segment Readings
void update_7_segment(void){   
	int count = (int)dist_travelled - (int)old_dist;
    	int i = 0;
    	if(dist_travelled > old_dist){
        	for(i=0; i < count; i++){
            		send_dist_to_segment();
        	}
        	old_dist = dist_travelled;
    	}
}
//System Clock
void Systick_init(void){
        NVIC_ST_RELOAD_R = (int)(15999999/1000000);  // for us delay
				//NVIC_ST_RELOAD_R = 16;  // for us delay
        NVIC_ST_CTRL_R = 5;
        NVIC_ST_CURRENT_R = 0;
}
//Delay function to be used
void delay_ms(int i){
    int counter = 0;
    while(counter != i*1000){
        if(NVIC_ST_CTRL_R & 0x10000){
            counter++;
        }
    }
}
//Send distance to 7 segments
void send_dist_to_segment(void){
    GPIO_PORTA_DATA_R |= 0x04; // Pin A2
    delay_ms(10);
    GPIO_PORTA_DATA_R &=~0x04;
}

//7 Segments All Clear - Set Reset - D0 = High to clear 7 segments
void pinA3_High_output(void){
    GPIO_PORTA_DATA_R |= 0x0C;
}

//Pin D0 = Low
void pinA3_Low_output(void){
    GPIO_PORTA_DATA_R &= ~0x0C;
}

//Turn on LED after 100m
void light_led_after_100m(void){
    if(dist_travelled >= 100){
        red_led_output(); //led red o/p function
    }
}

//LEDs Reset Function and Turn on red LED
void red_led_output(void){
        GPIO_PORTF_DATA_R &= ~(0x0E);
        GPIO_PORTF_DATA_R |= 0x02;
}