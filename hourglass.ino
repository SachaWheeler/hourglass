#include <math.h>
#include <LedControl.h>
#include "binary.h";

const int x_out = A3; // analogue pins
const int y_out = A2;
const int z_out = A1; // use this for the randomness for slippage constant. flat has low slippage, vertical has high.

const int x_range_low = 230;
const int x_range_high = 430;

const int y_range_low = 230;
const int y_range_high = 430;
const int x_range_mid = (x_range_high + x_range_low) / 2;
const int y_range_mid = (y_range_high + y_range_low) / 2;


const int max_x = 0;
const int min_x = 300;
const int max_y = 0;
const int min_y = 300;

const int D_NORTH = 1;
const int D_NE = 2;
const int D_EAST = 3;
const int D_SE = 4;
const int D_SOUTH = 5; 
const int D_SW = 6;
const int D_WEST = 7;
const int D_NW = 8;
int quadrant;

const long interval = 1000;
long previousMillis = 0; 
unsigned long currentMillis;

const int random_scale = 5;
int random_score = 2;

/*
 Now we need a LedControl to work with.
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 */
const int number_devices = 2;
LedControl lc=LedControl(12, 11, 10, number_devices);

byte board[2][8] = {
  {
    B11111111,
    B11111111,
    B11111111,
    B11100111,
    B11100111,
    B11111111,
    B11111111,
    B11111111
  },{
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000
  }
};


void setup() {
  /* Wakeup The MAX72XXs */
  for(int i=0; i < number_devices; i++) {
    lc.shutdown(i, false); 
    lc.setIntensity(i, 8);
    lc.clearDisplay(i);
  } 

  Serial.begin(9600); 
  
  test_leds();
}

void test_leds(){
  Serial.print("start test\n");
  for (int h=0; h < number_devices; h++){
    for (int i=0; i < 8; i++){
      for (int j=0; j < 8; j++){
        lc.setLed(h, i, j, true);
        delay(10);
        lc.setLed(h, i, j, false);
      }
    }
  }
  Serial.print("end test\n");
}

int bit_read(int b, int row, int col){
  if(col > 7 || col < 0 || row > 7 || row < 0)
    return 1;
  return bitRead(board[b][row], 7-col);
}

void bit_swap(int from_b, int from_row, int from_col, int b, int row, int col){
  if (from_col >= 0 && from_col <= 7 && from_row >= 0 && from_row <= 7){
    bitClear(board[from_b][from_row], 7-from_col);
    if (from_b != b || from_row != row)  // draw it if we need to
      lc.setRow(from_b, from_row, board[from_b][from_row]);
  }
  if (col >= 0 && col <= 7 && row >= 0 && row <= 7){
    bitSet(board[b][row], 7-col);
    lc.setRow(b, row, board[b][row]);
  }
}


void process_boards(){
  for (int b=0; b < number_devices; b++){
    for (int row=0; row < 8; row++){
      for (int col=0; col < 8; col++){
        // read the bit
        if(bit_read(b, row, col) == 1){
          // a partile is present. look for a move in the direction of "quadrant"


          
          if (quadrant == D_NORTH){
            // check to see if this should pass the particle to the lower board
            if(b==0 && row==7 && col==7 && bit_read(1, 0, 0) == 0){
              if(currentMillis - previousMillis > interval) {
                  previousMillis = currentMillis;   

                  bit_swap(b, row, col, 1, 0, 0);
              }
            }else if(bit_read(b, row+1, col+1) == 0){              
              bit_swap(b, row, col, b, row+1, col+1);
            }else if(bit_read(b, row+1, col) == 0){
              bit_swap(b, row, col, b, row+1, col);
            }else if(bit_read(b, row, col+1) == 0){
              bit_swap(b, row, col, b, row, col+1);
            }
            
          }else if (quadrant == D_NE){
            if(bit_read(b, row+1, col) == 0){
              bit_swap(b, row, col, b, row+1, col);
            } // add random slide left or right, if available
            else if (random(1,random_scale) == random_score){
              if(bit_read(b, row+1, col-1) == 0){
                bit_swap(b, row, col, b, row+1, col-1);
              }else if (bit_read(b, row+1, col+1) == 0){
                bit_swap(b, row, col, b, row+1, col+1);
              }
            }
            
          }else if (quadrant == D_EAST){
            if(bit_read(b, row+1, col-1) == 0){
              bit_swap(b, row, col, b, row+1, col-1);
            }else if(bit_read(b, row, col-1) == 0){
              bit_swap(b, row, col, b, row, col-1);
            }else if(bit_read(b, row+1, col) == 0){
              bit_swap(b, row, col, b, row+1, col);
            }
            
          }else if (quadrant == D_SE){
            if(bit_read(b, row, col-1) == 0){
              bit_swap(b, row, col, b, row, col-1);
            } // add random slide left or right, if available
            else if (random(1,random_scale) == random_score){
              if(bit_read(b, row-1, col-1) == 0){
                bit_swap(b, row, col, b, row-1, col-1);
              }else if (bit_read(b, row+1, col-1) == 0){
                bit_swap(b, row, col, b, row+1, col-1);
              }
            }
            
          }else if (quadrant == D_SOUTH){
            // check to see if this should pass the particle to the upper board   
            if(b==1 && row==0 && col==0 && bit_read(0, 7, 7) == 0){
               if(currentMillis - previousMillis > interval) {
                  previousMillis = currentMillis;   
                  
                  bit_swap(b, row, col, 0, 7, 7);
               }
            }else if(bit_read(b, row-1, col-1) == 0){
              bit_swap(b, row, col, b, row-1, col-1);
            }else if(bit_read(b, row-1, col) == 0){
              bit_swap(b, row, col, b, row-1, col);
            }else if(bit_read(b, row, col-1) == 0){
              bit_swap(b, row, col, b, row, col-1);
            }
            
          }else if (quadrant == D_SW){
            if(bit_read(b, row-1, col) == 0){
              bit_swap(b, row, col, b, row-1, col);
            } // add random slide left or right, if available
            else if (random(1, random_scale) == random_score){
              if(bit_read(b, row-1, col+1) == 0){
                bit_swap(b, row, col, b, row-1, col+1);
              }else if (bit_read(b, row-1, col-1) == 0){
                bit_swap(b, row, col, b, row-1, col-1);
              }
            }
            
          }else if (quadrant == D_WEST){
            if(bit_read(b, row-1, col+1) == 0){
              bit_swap(b, row, col, b, row-1, col+1);
            }else if(bit_read(b, row, col+1) == 0){
              bit_swap(b, row, col, b, row, col+1);
            }else if(bit_read(b, row-1, col) == 0){
              bit_swap(b, row, col, b, row-1, col);
            }
            
          }else if (quadrant == D_NW){
            if(bit_read(b, row, col+1) == 0){
              bit_swap(b, row, col, b, row, col+1);
            } // add random slide left or right, if available
            else if (random(1,random_scale) == random_score){
              if(bit_read(b, row+1, col+1) == 0){
                bit_swap(b, row, col, b, row+1, col+1);
              }else if (bit_read(b, row-1, col+1) == 0){
                bit_swap(b, row, col, b, row-1, col+1);
              }
            }
          }


          
        }// end bitRead
      }// end col
    }// end row
  }// end b (board)
}


void loop() {
   // put your main code here, to run repeatedly:
  int x_adc_value, y_adc_value, z_adc_value, theta;

  currentMillis = millis();

  x_adc_value = analogRead(x_out) - x_range_mid;
  y_adc_value = analogRead(y_out) - y_range_mid;
  // z_adc_value = analogRead(z_out) - z_range_mid;
  
  theta = round( atan2 (y_adc_value, x_adc_value) * 180/3.14159265 );
  if(theta < 1) theta += 360;

  if(theta < 22.5 or theta > 337.5) quadrant = D_NORTH;
  else if(theta < 67.5)             quadrant = D_NE;
  else if(theta < 112.5)            quadrant = D_EAST;
  else if(theta < 157.5)            quadrant = D_SE;
  else if(theta < 202.5)            quadrant = D_SOUTH;
  else if(theta < 247.5)            quadrant = D_SW;
  else if(theta < 292.5)            quadrant = D_WEST;
  else                              quadrant = D_NW;

  process_boards();
  
  delay(20);
}

