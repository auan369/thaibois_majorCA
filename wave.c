
//*************************************************************
// Performs basic I/O for the Omega PCI-DAS1602 
// Adapted from sine.c 
//*************************************************************
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
// #include <hw/pci.h>
// #include <hw/inout.h>
// #include <sys/neutrino.h>
// #include <sys/mman.h>
#include <math.h>
#include<windows.h>
																
#define	INTERRUPT	iobase[1] + 0		// Badr1 + 0 : also ADC register
#define	MUXCHAN		iobase[1] + 2		// Badr1 + 2
#define	TRIGGER		iobase[1] + 4		// Badr1 + 4
#define	AUTOCAL		iobase[1] + 6		// Badr1 + 6
#define DA_CTLREG	iobase[1] + 8		// Badr1 + 8

#define	 AD_DATA	iobase[2] + 0		// Badr2 + 0
#define	 AD_FIFOCLR	iobase[2] + 2		// Badr2 + 2

#define	TIMER0		iobase[3] + 0		// Badr3 + 0
#define	TIMER1		iobase[3] + 1		// Badr3 + 1
#define	TIMER2		iobase[3] + 2		// Badr3 + 2
#define	COUNTCTL	iobase[3] + 3		// Badr3 + 3
#define	DIO_PORTA	iobase[3] + 4		// Badr3 + 4
#define	DIO_PORTB	iobase[3] + 5		// Badr3 + 5
#define	DIO_PORTC	iobase[3] + 6		// Badr3 + 6
#define	DIO_CTLREG	iobase[3] + 7		// Badr3 + 7
#define	PACER1		iobase[3] + 8		// Badr3 + 8
#define	PACER2		iobase[3] + 9		// Badr3 + 9
#define	PACER3		iobase[3] + a		// Badr3 + a
#define	PACERCTL	iobase[3] + b		// Badr3 + b

#define DA_Data		iobase[4] + 0		// Badr4 + 0
#define	DA_FIFOCLR	iobase[4] + 2		// Badr4 + 2
	
int badr[5];			// PCI 2.2 assigns 6 IO base addresses

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int data[100];
float delta,dummy;

void makeSine(){
  delta=(2.0*3.142)/100.0;					// increment
  for(int i=0;i<100;i++) {
    dummy= ((sinf((float)(i*delta))) + 1.0) * 0x7fff;
    data[i]= (unsigned) dummy;			// add offset +  scale
  }
}

void makeSquare(){
  // delta=(2.0*3.142)/100.0;					// increment
  for(int i=0;i<100;i++) {
    if (i<50){
      data[i]=0;
    } else {
      data[i]=0xffff;
    }   
  }
}

void makeTriangle(){
  delta=0xffff/50.0;					// increment
  for(int i=0;i<100;i++) {
    if (i<50){
      data[i]=i*delta;
    } else {
      data[i]=0xffff-(i-50)*delta;
    }
  }
}

void makeSawtooth(){
  delta=0xffff/99.0;					// increment
  for(int i=0;i<100;i++) {
    data[i]=i*delta;
  }
}

int main() {
// struct pci_dev_info info;
// void *hdl;

// uintptr_t iobase[6];
// uintptr_t dio_in;
// uint16_t adc_in;
	
// unsigned int i,count;
// unsigned short chan;



// printf("\fDemonstration Routine for PCI-DAS 1602\n\n");

// memset(&info,0,sizeof(info));
// if(pci_attach(0)<0) {
//   perror("pci_attach");
//   exit(EXIT_FAILURE);
//   }

// 					// Vendor and Device ID 
// info.VendorId=0x1307;
// info.DeviceId=0x01;

// if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {
//   perror("pci_attach_device");
//   exit(EXIT_FAILURE);
//   }
//   					// Determine assigned BADRn IO addresses for PCI-DAS1602			

// printf("\nDAS 1602 Base addresses:\n\n");
// for(i=0;i<5;i++) {
//   badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
//   printf("Badr[%d] : %x\n", i, badr[i]);
//   }
 
// printf("\nReconfirm Iobase:\n");  	// map I/O base address to user space						
// for(i=0;i<5;i++) {			// expect CpuBaseAddress to be the same as iobase for PC
//   iobase[i]=mmap_device_io(0x0f,badr[i]);	
//   printf("Index %d : Address : %x ", i,badr[i]);
//   printf("IOBASE  : %x \n",iobase[i]);
//   }													
// 					// Modify thread control privity
// if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
//   perror("Thread Control");
//   exit(1);
//   }																											

//******************************************************************************
// D/A Port Functions
//******************************************************************************
	
printf("\n\nWrite Sine Demo\n");																						



// makeSine();
// makeSquare();
// makeTriangle();
makeSawtooth();
while(1) {
for(int i=0;i<100;i++) {
	// out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		
  //  	out16(DA_FIFOCLR, 0);				// Clear DA FIFO  buffer
  //  	out16(DA_Data,(short) data[i]);																																		
  //  	out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		
  // 	out16(DA_FIFOCLR, 0);				// Clear DA FIFO  buffer
	// out16(DA_Data,(short) data[i]);
  printf("Index: %d; Data: %d\n", i, data[i]);			
  Sleep(10);


  }
}
  							// Unreachable code
  							// Reset DAC to 5v
// out16(DA_CTLREG,(short)0x0a23);	
// out16(DA_FIFOCLR,(short) 0);			
// out16(DA_Data, 0x8fff);					// Mid range - Unipolar																											
  
// out16(DA_CTLREG,(short)0x0a43);	
// out16(DA_FIFOCLR,(short) 0);			
// out16(DA_Data, 0x8fff);				
																																						
// printf("\n\nExit Demo Program\n");
// pci_detach_device(hdl);
// return(0);
}