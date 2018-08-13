#include <math.h>
#include "LedControl.h"
#include "binary.h";

const int x_out = A3; // analogue pins
const int y_out = A2;
const int z_out = A1;

const int x_range_low = 230;
const int x_range_high = 430;

const int y_range_low = 230;
const int y_range_high = 430;

int max_x = 0;
int min_x = 300;
int max_y = 0;
int min_y = 300;

int D_NORTH = 1;
int D_NE = 2;
int D_EAST = 3;
int D_SE = 4;
int D_SOUTH = 5; 
int D_SW = 6;
int D_WEST = 7;
int D_NW = 8;

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 LedControl lc=LedControl(12,11,10,1);
 */
LedControl lc=LedControl(12,11,10,2);

/*
byte frames[][8] = {
   {
  B00000000,
  B00110000,
  B00110000,
  B00110000,
  B00111000,
  B00111000,
  B00011000,
  B00000000
}
*/


void setup() {
  /* The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call */
   
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,10);
  /* and clear the display */
  lc.clearDisplay(0);
  
  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  int x_adc_value, y_adc_value, z_adc_value, theta, x, y;
  int quadrant;
  int x_range_mid = (x_range_high + x_range_low) / 2;
  int y_range_mid = (y_range_high + y_range_low) / 2;

  x = analogRead(x_out);
  x_adc_value = x - x_range_mid;

  y = analogRead(y_out);
  y_adc_value = y - y_range_mid;
  /*
  if(x > max_x) max_x = x;
  if(x < min_x) min_x = x;
  if(y > max_y) max_y = y;
  if(y < min_y) min_y = y;
   */
  
  theta = round( atan2 (y_adc_value, x_adc_value) * 180/3.14159265 );
  if(theta < 1) theta += 360;

  if(theta < 22.5 or theta > 337.5)
    quadrant = D_NORTH;
  else if(theta < 67.5)
    quadrant = D_NE;
  else if(theta < 112.5)
    quadrant = D_EAST;
  else if(theta < 157.5)
    quadrant = D_SE;
  else if(theta < 202.5)
    quadrant = D_SOUTH;
  else if(theta < 247.5)
    quadrant = D_SW;
  else if(theta < 292.5)
    quadrant = D_WEST;
  else
    quadrant = D_NW;


    
  /*
  Serial.print("x = ");
  Serial.print(x_adc_value);
  Serial.print("\t\t");
  // Serial.print(max_x);
  // Serial.print("\t\t");
  Serial.print("y = ");
  Serial.print(y_adc_value);
  Serial.print("\t\t");
  // Serial.print(max_y);
  // Serial.print("\t\t");
  */
  Serial.print("theta = ");
  Serial.print(theta);
  Serial.print("\t\t");
  Serial.print("quadrant = ");
  Serial.print(quadrant);
  Serial.print("\n");
 
  delay(500);
}

/*
 * 
  flat    : x = 331    y = 324   z = 411
  reversed: x = 331    y = 327   z = 278
  
  vertical: x = 396    y = 326   z = 356
  90      : x = 335    y = 390   z = 364
  inverted: x = 265    y = 327   z = 355
  270     : x = 330    y = 262   z = 353

  x_range: high: 396 mid: 330 low: 265
  y_range: high: 390 mid: 326 low: 262

  prototype:
    populate 60 particles in 64 squares
    find which direction is *down*
    shunt particles in that direction

    def what direction is down(x, y):
      // theta = tan–1(y/x)
      // https://www.dummies.com/education/science/physics/how-to-find-a-vectors-magnitude-and-direction/
      

  setup:
    create 2 boards of 64 sqaures
    fill first board with 60 particles randomly

  loop:

    find which direction is down
    shunt each particle into an open space *down* (up, doww, left, right & diagonal)

    if down is laterally diagonal and gateway space is occupied:
      move gateway particle into southern square
      
*/

