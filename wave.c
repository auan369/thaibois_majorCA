// This program generates the standard wave function of one period, with values from 0 to 2 (amplitude = 1 and offset = 1) --> values from 0 to 2


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
// #include <windows.h>
#include <pthread.h>
// include port_output.c
#include "port_output.c"
// #include "user_input.c"

#define SINE_WAVE       0
#define SQUARE_WAVE     1
#define TRIANGLE_WAVE   2
#define SAWTOOTH_WAVE   3

// int waveform; // type of wave
// int steps;    // resultion of wave
// float M_PI = 3.14159265358979323846;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t th[4];
int i;
float data;
//set default values
int waveform = SINE_WAVE;
int steps = 100;
float frequency = 1.0;

float sineWave(int index){ // return sine wave from 0 to 2
  float delta;
  delta = (2.0 * M_PI) / (float) steps;
  return sinf(index * delta) + 1; 
}

float squareWave(int index){
  if (index < (steps / 2)){
    return 2.0; // high
  }

  else{
    return 0.0; // low
  }
}

float triangleWave(int index){
  float delta;
  delta = 2 / (steps / 2);
  if (index < (steps / 2)){
    return index * delta; // increase linearly
  }
  else{
    return 2 - delta * (index - steps / 2); // decrease linearly
  }
}

float sawtoothWave(int index){
  float delta;
  delta = 2 / steps;
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
  // return 0;
}

void* audio_thread( void* arg )
{
  while( 1 ) {
    pthread_mutex_lock( &mutex );
    
    // printf("%.5f\n", data);
    //! Need to update with code for DAC
    printf("\a"); // Auditory cue
    pthread_mutex_unlock( &mutex );
    usleep( 1000000/frequency); // Wait for the beat rate//sleep for 1ms, allows next thread to run
  }

}

void *input_thread(void *arg) {
    char c;
    while (1) {
        c = getchar();
        pthread_mutex_lock(&mutex);
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
        usleep( 1000000/frequency); //sleep for 1ms, allows next thread to run
        // printf("Freq: %f\n", frequency);
        pthread_mutex_unlock(&mutex);
        
    }
    return NULL;
}


void *visual_thread(void *arg) {
    while (1) {
        // Visual cue
        pthread_mutex_lock(&mutex);
        printf("\r%.5f", data);
        // printf("\rA"); // Basic visual cue
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        usleep( 1000000/(frequency*2)); // Half the beat rate for synchronization
        pthread_mutex_lock(&mutex);
        printf("\r       "); // Clear the visual cue
        fflush(stdout);
        pthread_mutex_unlock(&mutex);
        usleep( 1000000/(frequency*2)); // Half the beat rate for synchronization
    }
    return NULL;
}




int main( void ){
  
  pthread_create( &th[0], NULL, &function1, NULL );
  pthread_create( &th[1], NULL, &visual_thread, NULL );
  pthread_create( &th[2], NULL, &audio_thread, NULL );
  pthread_create( &th[3], NULL, &input_thread, NULL );
  pthread_join(th[0], NULL); // wait for the thread 0 to exit first (never happens)
  pthread_join(th[1], NULL); // wait for the thread 1 to exit first (never happens)
  pthread_join(th[2], NULL); // wait for the thread 2 to exit first (never happens)
  pthread_join(th[3], NULL); // wait for the thread 3 to exit first (never happens)

  return 0;
}