/*
mesure distance in M1 then send  to M2 using CAN and display it in LCD through I2C
transmitter code transmits the ditance in hex values 
*/

#include <LPC17xx.h>
#define PRESCALE (25-1)

unsigned int measure_distance(void);

//Timmer 
void initTimer0(void);
void startTimer0(void);
unsigned int stopTimer0(void);
void delayUS(unsigned int microseconds);

//CAN 
void CAN_Init(void);
void CAN_ACC(void);
void looktable(void); 
void can_tx(unsigned long int id,unsigned char msg);

int main()
{
	unsigned char distance;

  SystemInit();
	LPC_PINCON->PINMODE1 |= (3<<10); //Pull downn if need change it 
	
  LPC_GPIO2->FIOMASK1 = 0xE7;      // 1110 0111 P2 11 /12 : TRIG / ECHO
  LPC_GPIO2->FIODIR1 = 0X08;      //  0000 1000 TRIG output // ECHO input

  LPC_GPIO1->FIOMASK3 = 0xDF;      // Allow bit5 LED P.1 29
  LPC_GPIO1->FIODIR3 |= 0X20;    
  LPC_GPIO1->FIOCLR3 = 0X20;     //  OFF initially

	initTimer0();
	CAN_Init();
	CAN_ACC();
	looktable();

  while(1)
		{
			unsigned char prev_d;
			distance = measure_distance();
			
//			if (prev_d != distance ){
//				prev_d = distance;
//				can_tx(0x00000022,distance);
//				delayUS(60000);
//			}
			
			can_tx(0x00000022,distance);
			delayUS(200000);
//			int i ;
//			char val = 0x0;
//			for (i=0; i<0x32; i++){
//				can_tx(0x00000022, val);
//				val ++;
//				delayUS(200000);
//			}
    }
}

void CAN_Init(void)
{
	LPC_SC->PCONP|=0x00002000;
	LPC_SC->PCLKSEL0|=0X00000000;
	LPC_PINCON->PINSEL0|=0X00000005;
	LPC_CAN1->MOD=0x00000001;
	LPC_CAN1->CMR=0X00000000;
	LPC_CAN1->GSR=0x00000000;
	LPC_CAN1->IER=0x00000001;
	LPC_CAN1->BTR=0X001C0007;
	LPC_CAN1->MOD=0x00000000;	
	NVIC_EnableIRQ(CAN_IRQn);
}

void CAN_ACC(void) // need changes here if we add id s
{
	LPC_CANAF->AFMR=0x00000001;
	LPC_CANAF->SFF_sa=0x00000000;
	LPC_CANAF->EFF_sa=0x00000000;
	LPC_CANAF->SFF_GRP_sa=0x00000000;
	LPC_CANAF->EFF_GRP_sa=0x00000000C;
	LPC_CANAF->ENDofTable=0x00000000C;
	LPC_CANAF->AFMR=0x00000000;	
}

void looktable(void) 
{
	LPC_CANAF->AFMR=0x00000001;
	LPC_CANAF_RAM->mask[0]=0x00000020;
	LPC_CANAF_RAM->mask[1]=0x00000021;
	LPC_CANAF_RAM->mask[2]=0x00000022; // Add up if need ID's 
	LPC_CANAF->AFMR=0x00000000;
}
	

void can_tx(unsigned long int id,unsigned char msg) //Based on Node 
{
	while((LPC_CAN1->SR&0X00000004)!=0X00000004);
	LPC_CAN1->TFI1=0X80020000; //FF, RTR, DLC 19-16, Priority 7-0 1000 0000 0000 0010 
	LPC_CAN1->TID1=id;
	LPC_CAN1->TDA1=msg;
	LPC_CAN1->CMR=0X21;
}

unsigned int measure_distance(void)
{
   unsigned int echoTime =0;
	
   // TRIGGER 10us PULSE 
   LPC_GPIO2->FIOCLR1 = 0X08; // 0000 1000  TRIG o/p
   delayUS(5);
   LPC_GPIO2->FIOSET1 = 0X08; 
   delayUS(10); // 10US
   LPC_GPIO2->FIOCLR1 = 0X08; 

   //  WAIT FOR ECHO HIGH 
   while(!(LPC_GPIO2->FIOPIN1 & 0X10)); // 0001 0000
	
	 startTimer0(); //start
	 while(LPC_GPIO2 -> FIOPIN1 & 0x10);
	 echoTime = stopTimer0();
		 
   return echoTime/0X3A;  //58
}

void initTimer0(void) //PCLK must be = 25Mhz! 
  { 
		LPC_SC->PCONP |= (1<<1); //Power up TIM0. By default TIM0 and TIM1 are enabled.
	  LPC_SC->PCLKSEL0 &= ~(0x3<<2); //Set PCLK for timer = CCLK/4 = 100/4 (default)
  	LPC_TIM0->CTCR = 0x0; 
  	LPC_TIM0->PR = PRESCALE; //Increment TC at every 24999+1 clock cycles 
  	LPC_TIM0->TCR = 0x02; //Reset Timer 
  }
void startTimer0(void) 
  { 
  	LPC_TIM0->TCR = 0x02; //Reset Timer 
  	LPC_TIM0->TCR = 0x01; //Enable timer 
  } 
  
unsigned int stopTimer0(void)
{ 
  LPC_TIM0->TCR = 0x00; //Disable timer 
  return LPC_TIM0->TC; 
}

void delayUS(unsigned int microseconds) //Using Timer0 
  { 
  	LPC_TIM0->TCR = 0x02; //Reset Timer 
  	LPC_TIM0->TCR = 0x01; //Enable timer 
  	while(LPC_TIM0->TC < microseconds); //wait until timer counter reaches the desired delay 
  	LPC_TIM0->TCR = 0x00; //Disable timer 
  }
	