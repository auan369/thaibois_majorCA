// This program generates the standard wave function of one period, with values from 0 to 2 (amplitude = 1 and offset = 1) --> values from 0 to 2


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <pthread.h>
// include port_output.c
#include "port_output.c"
#include "user_input.c"

#define SINE_WAVE       0
#define SQUARE_WAVE     1
#define TRIANGLE_WAVE   2
#define SAWTOOTH_WAVE   3

int waveform; // type of wave
int steps;    // resultion of wave
float M_PI = 3.14159265358979323846;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

void* printOut( void* arg )
{
    while( 1 ) {
        pthread_mutex_lock( &mutex );
        tmp = count++;
        pthread_mutex_unlock( &mutex );
        printf( "Count is %d\n", tmp );

        /* snooze for 1 second */
        sleep( 1 );
    }

    return 0;
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
  }
  return 0;
}

void* function1( void* arg )
{
  while( 1 ) {
    pthread_mutex_lock( &mutex );
    // TODO: SEND TO output.c VIA MESSAGING
    printf("Index: %.2d;", i);
    port_output(data);  
    pthread_mutex_unlock( &mutex );
  }
  return 0;
}


int i;
float data;

//set default values
waveform = SINE_WAVE;
steps = 100;


int main(){
  


  // while(1){
  //   // TODO : RECEIVE MESSAGE FROM input.c FOR USER SETTINGS

  //   for (i=0; i<steps; i++){
  //     data = genWave(i, waveform);
  //     // printf("Index: %.2d; Value: %.5f\n", i, data);
      

  //     // TODO: SEND TO output.c VIA MESSAGING
  //     printf("Index: %.2d;", i);
  //     port_output(data);
  //   }
  // }
  // user_input(0, NULL);
  pthread_create( NULL, NULL, &function1, NULL );
  pthread_create( NULL, NULL, &function2, NULL );

  exit(0);
}