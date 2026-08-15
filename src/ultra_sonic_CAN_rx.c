/*
mesure distance in M1 then send  to M2 using CAN and display it in LCD through I2C
reciver code reciveves the ditance in hex values and displays it on LCD 
*/

#include<LPC17xx.h>
#include <stdio.h>

#define LCD_ADDR 0x27 

void CAN_Init(void);
void CAN_ACC(void);
void looktable(void); 
void delay(unsigned long int z);
void can_rx(void);
unsigned char rx=0;

void lcd_write(unsigned char ch, unsigned char rs);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(unsigned char data);
void i2c_init(void);

int main()
{

	
	SystemInit ();	
	i2c_init();
	delay(0x100);
	LPC_GPIO1->FIOMASK3 =0XDF;  //p1.29 LED OUTPUT  //for cross-check
	LPC_GPIO1->FIODIR3 =0X20;   //LED OUTPUT  //for cross-check
	CAN_Init();
	CAN_ACC();
	looktable(); 	
	
	while(1){
		
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

void CAN_ACC(void)
{
	LPC_CANAF->AFMR=0x00000001;
	LPC_CANAF->SFF_sa=0x00000000;
	LPC_CANAF->SFF_GRP_sa=0x00000000;
	LPC_CANAF->EFF_sa=0x00000000;
	LPC_CANAF->EFF_GRP_sa=0x0000000C;
	LPC_CANAF->ENDofTable=0x0000000C;
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

void delay(unsigned long int z)
{
	unsigned long int x;
	for(x=0;x<z;x++);
}

  

void 	CAN_IRQHandler(void)
{
  can_rx();
}	

void can_rx(void) // Changes need 
{                                                    //ca,cb,cc,cd are all temperory variables
  unsigned long int CA = 0,CB = 0;  //ca = standard freme i.e., 11 bit or 29 bit
	
	CA = LPC_CAN1->RFS;  //receive frame status
	CA = CA & 0x20000000;  //Bit 29 indicates whether the frame is:11 bit or 29 bit, now it is 1 so 29 bit ide
	if (CA == 0x00000000)
	{
		CB = LPC_CAN1->RID;  //cb = ide
		CB = CB & 0x000007FF; //Bits 0-9 of RFS contain frame information including DLC (Data Length Code)
		if(CB == 0x00000022)
		{
        char buf[16];
				unsigned char cmd[]={0x33,0x32,0x28,0x0E,0x01,0X06,0x80};
				unsigned char i;
				unsigned char distance;
			  distance = LPC_CAN1->RDA & 0xFF;
			for(i=0;i<7;i++)
      {
        lcd_write(cmd[i],0);  //rs=0
        delay(0x5000);
      }
			
		sprintf(buf,"%u",distance);
			
    for(i=0;buf[i]!='\0';i++)
    {
        lcd_write(buf[i],1);  //rs=1
        delay(0x8000);
    }
			}
		LPC_CAN1->CMR = 0x04;   // Release receive buffer
		}
}


void i2c_init(void)
{
    LPC_SC->PCONP |= 0x00000080;//7
    LPC_PINCON->PINSEL1 = 0x01400000;//p0.27 28 - sda,scl

    LPC_I2C0->I2SCLH = 0x0000005A;// SCL High time (90) for 100khz standard mode
    LPC_I2C0->I2SCLL = 0x0000005A;// SCL Low time  (90) for 100khz standard mode
    LPC_I2C0->I2CONSET = 0x00000040;// I2EN = 1 (Enable I2C) 6
}


void i2c_start(void)
{
    LPC_I2C0->I2CONSET = 0x00000020;        // Set START bit
	  while(LPC_I2C0->I2STAT != 0x08);        // Wait for START transmitted
    LPC_I2C0->I2CONCLR = 0x00000028;        // Clear SI
}


void i2c_stop(void)
{
    LPC_I2C0->I2CONSET = 0x00000010;        // STOP bit
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI
    delay(0x2000);
}


void i2c_write(unsigned char data)
{
    LPC_I2C0->I2DAT = data;                 // Load data into I2DAT
    LPC_I2C0->I2CONCLR = 0x00000008;        // Clear SI
    while((LPC_I2C0->I2CONSET & 0x00000008) == 0); // Wait till SI set
}


void lcd_write(unsigned char ch, unsigned char rs)
{
    unsigned char high_nib, low_nib;
    unsigned char data;

    high_nib = ch & 0xF0;              // Upper nibble
    low_nib = (ch << 4) & 0xF0;        // Lower nibble

    // Send upper nibble first
    data = high_nib | (rs ? 0x01 : 0x00) | 0x08  ; 
    i2c_start();
    i2c_write(LCD_ADDR << 1);          // Slave address + write
    i2c_write(data | 0x04);            // EN = 1
	  delay(0x1000);
    i2c_write(data & ~0x04);           // EN = 0
		delay(0x10000);
    i2c_stop();

    // Send lower nibble
    data = low_nib | (rs ? 0x01 : 0x00) | 0x08;   
    i2c_start();
    i2c_write(LCD_ADDR << 1);          // Slave address + write
    i2c_write(data | 0x04);            // EN = 1
		delay(0x1000);
    i2c_write(data & ~0x04);           // EN = 0
		delay(0x10000);
    i2c_stop();

    delay(0x3000);
}