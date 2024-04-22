// This program generates the standard wave function of one period, with values from 0 to 2 (amplitude = 1 and offset = 1) --> values from 0 to 2


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <hw/pci.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <math.h>

#define	INTERRUPT		  iobase[1] + 0				// Badr1 + 0 : also ADC register
#define	MUXCHAN			  iobase[1] + 2				// Badr1 + 2
#define	TRIGGER			  iobase[1] + 4				// Badr1 + 4
#define	AUTOCAL			  iobase[1] + 6				// Badr1 + 6
#define DA_CTLREG		  iobase[1] + 8				// Badr1 + 8

#define	 AD_DATA		  iobase[2] + 0				// Badr2 + 0
#define	 AD_FIFOCLR		iobase[2] + 2				// Badr2 + 2

#define	TIMER0				iobase[3] + 0				// Badr3 + 0
#define	TIMER1				iobase[3] + 1				// Badr3 + 1
#define	TIMER2				iobase[3] + 2				// Badr3 + 2
#define	COUNTCTL			iobase[3] + 3				// Badr3 + 3
#define	DIO_PORTA		  iobase[3] + 4				// Badr3 + 4
#define	DIO_PORTB		  iobase[3] + 5				// Badr3 + 5
#define	DIO_PORTC		  iobase[3] + 6				// Badr3 + 6
#define	DIO_CTLREG		iobase[3] + 7				// Badr3 + 7
#define	PACER1				iobase[3] + 8				// Badr3 + 8
#define	PACER2				iobase[3] + 9				// Badr3 + 9
#define	PACER3				iobase[3] + a				// Badr3 + a
#define	PACERCTL			iobase[3] + b				// Badr3 + b

#define 	DA_Data			iobase[4] + 0				// Badr4 + 0
#define	DA_FIFOCLR		iobase[4] + 2				// Badr4 + 2
	
int badr[5];															// PCI 2.2 assigns 6 IO base addresses




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	

#define SINE_WAVE       0
#define SQUARE_WAVE     1
#define TRIANGLE_WAVE   2
#define SAWTOOTH_WAVE   3


// Structure to hold settings
//struct Settings {
//    float frequency = 1.0;
//};

// Default settings
//struct Settings default_settings = {1.0};
//struct Settings settings;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t th[4];
int i;
float data;
//set default values
int waveform = TRIANGLE_WAVE;
int steps = 50;
float frequency = 1.0;

float sineWave(int index){ // return sine wave from 0 to 2
  float delta;
  delta = (2.0 * M_PI) / (float) steps;
  return sinf(index * delta) + 1; 
}

float squareWave(int index){
  if (index < (steps / 2)){
    return 1.999; // high
  }

  else{
    return 0.0; // low
  }
}

float triangleWave(int index){
  float delta;
  delta = 1.999 / (steps / 2);
  if (index < (steps / 2)){
    return index * delta; // increase linearly
  }
  else{
    return 1.999 - delta * (index - steps / 2); // decrease linearly
  }
}

float sawtoothWave(int index){
  float delta;
  delta = 2.0 / steps;
  return index * delta;
}


float genWave(int index, int waveform){
  switch  (waveform){
    case SINE_WAVE :
      return sineWave(index);
      break;

    case SQUARE_WAVE:
      return squareWave(index);
      break;

    case TRIANGLE_WAVE :
      return triangleWave(index);
      break;
      
    case SAWTOOTH_WAVE:
      return sawtoothWave(index);
      break;

    default:
      printf("No such waveform\n");
  }

}


void* function1( void* arg )
{
  while( 1 ) {
    pthread_mutex_lock( &mutex );
    // TODO : RECEIVE MESSAGE FROM input.c FOR USER SETTINGS
    if (i<steps){
      data = genWave(i++, waveform);
    }
    else{
      i = 0;
      data = genWave(i++, waveform);
    }
    pthread_mutex_unlock( &mutex );
    usleep(1000000/frequency); //sleep for 1ms, allows next thread to run
  }

}

void* audio_thread( void* arg )
{
  while( 1 ) {
  	//printf("Audio thread\n");
    //pthread_mutex_lock( &mutex );
    printf("\a"); // Auditory cue;
    //pthread_mutex_unlock( &mutex );
    usleep( 1000000/frequency); // Wait for the beat rate//sleep for 1ms, allows next thread to run
  }

}

void *input_thread(void *arg) {
    char input[10];
    char c;
    int num;
    
    while (1) {
  	  
  	  //printf("input thread\n");
  	  
  	   if (scanf("%s", input)==1){
       printf("input: %s", input);
       	if (sscanf(input, "%d", &num) ==1) {
       		pthread_mutex_lock( &mutex );
       		printf("the value here is: %d\n", num);
       		frequency=num;
       		pthread_mutex_unlock( &mutex );
       	}
       	else{
       		//printf("not a number");
       		if (sscanf(input, "%c", &c) ==1) {
       			pthread_mutex_lock( &mutex );
       			printf("the char here is: %c\n", c);
       			 switch (c) {
            		case '+':
              	  		frequency += 1;
                		break;
            		case '-':
                		if (frequency > 1) {
                    		frequency-=1;
                		}
                		break;
            
            		default:
                		break;
        		}
        		pthread_mutex_unlock( &mutex );
          	
       		}
       	}

       }

      usleep( 1000000/frequency); //sleep for 1ms, allows next thread to run
        
    }
    return NULL;
}

void *visual_thread(void *arg) {
    while (1) {
        // Visual cue
        pthread_mutex_lock(&mutex);
        printf("\rBLINK");	
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        usleep( 1000000/(frequency*2)); // Half the beat rate for synchronization
        pthread_mutex_lock(&mutex);
        printf("\r     "); // Clear the visual cue
        fflush(stdout);																	
        pthread_mutex_unlock(&mutex);
        usleep( 1000000/(frequency*2)); // Half the beat rate for synchronization
    }
    return NULL;
}


//void read_settings(struct Settings *settings) {
    // Read settings from file
//    FILE *file = fopen("settings.txt", "r");
//    if (file != NULL) {
//        fscanf(file, "%d", &(settings->frequency));
//        fclose(file);
//    } else {
        // Use default settings if file doesn't exist
//        *settings = default_settings;
//    }
//}

//void write_settings(struct Settings *settings) {
    // Write settings to file
//    FILE *file = fopen("settings.txt", "w");
//    if (file != NULL) {
//        fprintf(file, "%d", settings->frequency);
//        fclose(file);
//    }
//}

//!----------Main Function----------!
int main( void ){

//read_settings(&settings);
//frequency = settings.frequency;

struct pci_dev_info info;
void *hdl;

uintptr_t iobase[6];
uintptr_t dio_in;
uint16_t adc_in;
	
unsigned int i,count;
unsigned short chan;

unsigned int data1[50];
float delta,dummy;

printf("\fDemonstration Routine for PCI-DAS 1602\n\n");

memset(&info,0,sizeof(info));
if(pci_attach(0)<0) {
  perror("pci_attach");
  exit(EXIT_FAILURE);
  }

info.VendorId=0x1307;
info.DeviceId=0x01;																		/* Vendor and Device ID */


if ((hdl=pci_attach_device(0, PCI_SHARE|PCI_INIT_ALL, 0, &info))==0) {
  perror("pci_attach_device");
  exit(EXIT_FAILURE);
  }
  																		// Determine assigned BADRn IO addresses for PCI-DAS1602			

printf("\nDAS 1602 Base addresses:\n\n");
for(i=0;i<5;i++) {
  badr[i]=PCI_IO_ADDR(info.CpuBaseAddress[i]);
  printf("Badr[%d] : %x\n", i, badr[i]);
  }
 
printf("\nReconfirm Iobase:\n");  						// map I/O base address to user space						
for(i=0;i<5;i++) {												// expect CpuBaseAddress to be the same as iobase for PC
  iobase[i]=mmap_device_io(0x0f,badr[i]);	
  printf("Index %d : Address : %x ", i,badr[i]);
  printf("IOBASE  : %x \n",iobase[i]);
  }													
																		// Modify thread control privity
if(ThreadCtl(_NTO_TCTL_IO,0)==-1) {
  perror("Thread Control");
  exit(1);
  }				

//delta=(2.0*3.142)/50.0;					// increment
for(i=0;i<50;i++) {
  //dummy= ((sinf((float)(i*delta))) + 1.0) * 0x8000 ;
  //data1[i]= (unsigned) dummy;			// add offset +  scale

  dummy = genWave(i, waveform)* 0x8000;
  data1[i]= (unsigned) dummy;			// add offset +  scale
  printf("%f\n", dummy);
 }

  
  //pthread_create( &th[0], NULL, &function1, NULL );
  pthread_create( &th[1], NULL, &visual_thread, NULL );
  pthread_create( &th[2], NULL, &audio_thread, NULL );
  pthread_create( &th[3], NULL, &input_thread, NULL );
  
while(1) {
//printf("Main thread\n");

for(i=0;i<50;i++) {
	out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		2/6
  	out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
  	out16(DA_Data,(short) data1[i]);																																		
  	out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		2/6
  	out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
	out16(DA_Data,(short) data1[i]);			
	
	usleep( 1000000/(frequency*50)); // Wait for time of T/steps, allows next thread to run
  }
}
	

  //pthread_join(th[0], NULL); // wait for the thread 0 to exit first (never happens)
  //pthread_join(th[1], NULL); // wait for the thread 1 to exit first (never happens)
  //pthread_join(th[2], NULL); // wait for the thread 2 to exit first (never happens)
  //pthread_join(th[3], NULL); // wait for the thread 3 to exit first (never happens)

  return 0;
}
#/** PhEDIT attribute block
#-11:16777215
#0:4520:default:-3:-3:0
#4520:4888:default:-3:-3:4
#4888:9086:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000167)**/
