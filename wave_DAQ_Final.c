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
# define SINESAWTOOTH_WAVE 4


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
int waveform = SINE_WAVE;
int steps = 50;
int frequency = 1;
char filename[20];
int file_exist = 0;
char out_filename[20] = "new_settings.txt";
int outfile_exist = 0;



//-------------------------------------------------------------parse arg----------------------------------------------------------------
int parse_arg(int argc, char const *argv[], int *wave, int *freq, char file[], char out_file[]){

	for (i=1;i<argc; i++){
		if (strcmp(argv[i], "-w")==0){ //waveform flag
			i++;
			if (strcmp(argv[i], "sine") == 0) *wave=SINE_WAVE;
			else if (strcmp(argv[i], "square") == 0) *wave=SQUARE_WAVE;
			else if (strcmp(argv[i], "triangle") == 0) *wave=TRIANGLE_WAVE;
			else if (strcmp(argv[i], "sawtooth") == 0) *wave=SAWTOOTH_WAVE;
			else if (strcmp(argv[i], "sinesawtooth") == 0) *wave=SINESAWTOOTH_WAVE;

			else {
				printf("invalid input. Loading Default Sine waveform\n");
				*wave=SINE_WAVE;
				} 
		}
		
		else if (strcmp(argv[i], "-f")==0){ //frequency flag
			i++;
			*freq = atoi(argv[i]);
			if (*freq <=0){
				printf("Input frequency is out of range as it should be more than 0Hz\n");
				printf("Setting default frequency to 1Hz\n");
				*freq = 1;
			}
		
		}
		
		else if (strcmp(argv[i], "-file")==0){ // input file flag
			i++; 
			strcpy(file, argv[i]);
			file_exist = 1;
			//printf(file);
		}
		
		else if (strcmp(argv[i], "-outfile")==0){//output file flag
			i++;
			strcpy(out_file, argv[i]);
			outfile_exist = 1;

		}

		else{
			printf("Invalid flag\nExiting Program\n");
			return 1; // return error
		}
		
	}
	
return 0; // if everything is parse correctlly
	
}

//--------------------------------------------------------------read and write file-------------------------------------------------------------

void readFile(char filename[]){

	FILE *file = fopen(filename, "r"); // can add missing file check here
	

	if(file != NULL){
		printf("\nRead file %s\n", filename);
		fscanf(file, "%d", &waveform);
		printf("%d\n",waveform);
		usleep(1000000);
		
		fscanf(file, "%d", &frequency);
		printf("%d\n",frequency);
		usleep(1000000);
		fclose(file); }
		
	else{ 			// error checking for invalid filename
		printf("File %s does not exists\n", filename);
		printf("Loading default settings: SINE_WAVE, 1Hz\n");
	}
}

void writeFile(char filename[]){

	FILE *file = fopen(filename, "w");
	//printf("\nWritefile\n");

	if(file != NULL){
		fprintf(file, "%d\n", waveform);
		// printf("%d\n",waveform);
		
		fprintf(file, "%d\n", frequency);
		// printf("%d\n",frequency);


			
		fclose(file); }
}



void print_wave_settings(){
	   switch  (waveform){
    	case SINE_WAVE :
      		printf("Generating sine wave of frequency %d\n", frequency);
      		break;

    	case SQUARE_WAVE:
      		printf("Generating square wave of frequency %d\n", frequency);
      		break;

    	case TRIANGLE_WAVE :
      		printf("Generating triangle wave of frequency %d\n", frequency);
      		break;
      
    	case SAWTOOTH_WAVE:
      		printf("Generating sawtooth wave of frequency %d\n", frequency);
      		break;
      
     	case SINESAWTOOTH_WAVE:
     		printf("Generating sine + sawtooth wave of frequency %d\n", frequency);
     		break;

    	default:
      		printf("\n");
  }
}

// ------------------------- Define wave functions ------------------------------------------------
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


float sineSawtoothWave(int index){
	return (sineWave(index) + sawtoothWave(index)) /2;
	
}

// ----------------------------------------- Generate the data point for the wave for a given index -----------------------------

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
      
     case SINESAWTOOTH_WAVE:
     	return sineSawtoothWave(index);
     	break;

    default:
      printf("No such waveform\n");
  }

}
//------------------------------------------- Define Threads ------------------------------------------


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
    printf("\a"); // Auditory cue
;
    //pthread_mutex_unlock( &mutex );
    usleep( 1000000/frequency); // Wait for the beat rate//sleep for 1ms, allows next thread to run
  }

}
//
------------------------------------------------------------input thread ------------------------------------------------------------
// thread to take in input from terminal
void *input_thread(void *arg) {
    char input[10];
    char c;
    int num;
    
    while (1) {
  	  
  	  //printf("input thread\n");
  	   if (scanf("%s", input)==1){
       //printf("input: %s\n", input);
       	if (sscanf(input, "%d", &num) ==1) {
       		pthread_mutex_lock( &mutex );
       		printf("\nInput Frequency: %d\n", num);
       		if (num > 0){
       			frequency=num;
       		}
       		else{
       			printf("Invalid Input Frequency. Please try again\n");
       		}
       		writeFile(out_filename);
       		print_wave_settings();
       		pthread_mutex_unlock( &mutex );
       	}
       	else{
       		//printf("not a number");
       		if (sscanf(input, "%c", &c) ==1) {
       			pthread_mutex_lock( &mutex );
       			printf("Input Char is: %c\n", c);
       			 switch (c) {
            		case '+':
              	  		frequency += 1;
              	  		//writeFile();
                		break;
            		case '-':
                		if (frequency > 1) {
                    		frequency-=1;
                    		//writeFile();
                		}
                		break;
                	case 'S':
                	case 's':
                		waveform = SINE_WAVE;
                		//writeFile();
                		break;
                	case 'W':
                	case 'w':
                		waveform = SAWTOOTH_WAVE;
                		//writeFile();
                		break;
                	case 'Q':
                	case 'q':
                		waveform = SQUARE_WAVE;
                		//writeFile();
                		break;
              	 	case 'T':
					case 't':
                		waveform = TRIANGLE_WAVE;
                		//writeFile();
                		break;
                	case 'E':
					case 'e':
                		waveform = SINESAWTOOTH_WAVE;
                		//writeFile();
                		break;
            		default:
            			printf("Invalid char input. Please put only 'S or s', 'W or w', 'Q or q', 'T or t', 'E or e' as inputs\n");
                		break;
        		}
        		writeFile(out_filename);
        		print_wave_settings();
        		
        		

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







//!--------------------------------------------------Main Function--------------------------------------------------!
int main(int argc, char const *argv[]){


struct pci_dev_info info;
void *hdl;

uintptr_t iobase[6];
uintptr_t dio_in;
uint16_t adc_in;
	
unsigned int i,j,count;
unsigned short chan;

unsigned int data1[5][50];



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

printf("\n");


//-------------------------------------------------------------parse arg------------------------------------------------------
if (parse_arg(argc, argv, &waveform, &frequency, filename, out_filename)) exit(1);
 // exit program if parse arg fails
if (!outfile_exist) printf("No output file given. Saving settings to default output file new_settings.txt\n" );





// generate the data points for the 4 waveforms
for (j=0;j<5;j++){
	for(i=0;i<50;i++) {

  	dummy = genWave(i, j)* 0x8000;
 	data1[j][i]= (unsigned) dummy;			// add offset +  scale
 	//printf("%f\n", dummy);
 }

}
  
  
  if (file_exist) readFile(filename);
 // it will priorities file settings over command line settings
  
writeFile(out_filename);

  print_wave_settings();
 
  
  //pthread_create( &th[0], NULL, &function1, NULL );
  pthread_create( &th[1], NULL, &visual_thread, NULL );
  pthread_create( &th[2], NULL, &audio_thread, NULL );
  pthread_create( &th[3], NULL, &input_thread, NULL );
  
while(1) {
//printf("Main thread\n");

for(i=0;i<50;i++) {
	out16(DA_CTLREG,0x0a23);			// DA Enable, #0, #1, SW 5V unipolar		2/6
  	out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
  	out16(DA_Data,(short) data1[waveform][i]);																																		
  	out16(DA_CTLREG,0x0a43);			// DA Enable, #1, #1, SW 5V unipolar		2/6
  	out16(DA_FIFOCLR, 0);					// Clear DA FIFO  buffer
	out16(DA_Data,(short) data1[waveform][i]);			
	
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
#0:5786:default:-3:-3:0
#5786:6420:default:-3:-3:4
#6420:6686:TextFont9:-3:-3:4
#6686:6828:default:-3:-3:4
#6828:10497:default:-3:-3:0
#**  PhEDIT attribute block ends (-0000224)**/
