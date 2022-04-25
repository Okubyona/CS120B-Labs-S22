/*        Your Name & E-mail:  Andrew Bazua abazu001@ucr.edu

*          Discussion Section: 021

 *         Assignment: Lab #4  Exercise #2

 *         Exercise Description: Reimplement Exercise 3 Double Light Show into a SynchSM

Description: Create a single Light Show that is run by a SynchSM. The period of
             the SynchSM is 1 second. This light show is relatively simple and
             simply has the four sequences {1000, 0100, 0010, 0001}. The light
             show changes patterns every period. The light show goes forward in
             the pattern, resetting at the beginning as needed.

             However covering the photoresistor should cause the light show to
             slow down and only change patterns every 2 periods. The period
             should not change.

             Hint: Keep track of the number of periods using a variable.

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://youtu.be/B9Ty_vb_9bo

 */

enum States{INIT, L1, L2} SM1_state = INIT;
unsigned long lastRan;
const unsigned long period = 1000;
int X_button = 8;

// Used to track which light element will be shown
int iter = 0;
// Will increment to slow down the SM
int slowTime = 0;

unsigned short lightLevel = 313;
// Photoresistor value that changes lightShows if value is less than lightSwitch
unsigned short lightOut = 130;

// The lightshows
char lightShow1[4] = {0x01, 0x08, 0x02, 0x04};

char output;

// Array of Output Pin variables, set to the pins being used
const int b_size = 4;
const int b[b_size] = {2, 3, 4, 5};
// Output Buffer
int b_buf = 0x00;
// Input Variables

// Serial Monitor Buffer
int s_buf = 0x00;

/* 's' is an array of integers of size 8. Note that arrays start at 0
 We will use this to be able to see the individual bit values of the s_buf

 */
const int s_size = 8;
int s[s_size];

// We can also define our own helper functions. It is a good idea to use helper functions whenever they make sense.
// Normally we also define the return type (void for none)

// Read from the serial monitor into s_buf and the s[] array for individual bits
void readData()
{
    if(Serial.available())
        s_buf = Serial.parseInt();

    for(int i = (s_size - 1); i>=0; i--)  {
        s[i] = (s_buf >> i) & 0x01; // What's going on here?
        // ">>" bit shifting
        // "&" bit masking
    }

}
// Reset the Output Buffer.
void resetBuffer() {
    for(int i = 0; i < b_size; i++) {
        // Note this is an arduino function call to the pins
        digitalWrite(b[i], LOW);
    }
}
// Writes to the buffer. Note this function ORs the current value with the new value
// Note that size is an optional argument with default size the same as the buffer
void writeBuffer(unsigned char b_temp, int size = b_size)
{
    for (int i = (size - 1); i >= 0; i--) {
        if ((b_temp >> i) & 0x01) {
        digitalWrite(b[i], HIGH);
        }
    }


}


void setup(){
  //some set up (Default Arduino Function)
    pinMode(X_button, INPUT);

    // LEDs
    for(int i = 0; i < b_size; i++)
    {
        pinMode(b[i], OUTPUT);
    }
    Serial.begin(9600);
   lastRan = 0;
}
// This function takes in the state as an argument & returns the current state
int SM1_Tick(int state){
    //Read thing
    switch(state){ // State transitions
        case INIT:
            //State Transition
            state = L1;

            lightLevel = analogRead(A1);
        break;

        case L1:
            if (lightLevel > lightOut) {
                state = L1;
            }
            else {
                state = L2;
            }
        break;

      case L2:
          if (lightLevel <= lightOut) {
              state = L2;
          }
          else {
              state = L1;
          }
        break;
    }
    switch(state){ // State Action
        case INIT:
            //State Action
            iter = 0;
            slowTime = 2;
            lightLevel = analogRead(A1);
        break;

        case L1:
            lightLevel = analogRead(A1);
            iter %= 4;
            output = lightShow1[iter];
            // Set to 1 to avoid stalls
            slowTime = 1;
            ++iter;
        break;

        case L2:
            lightLevel = analogRead(A1);
            if (slowTime == 2) {
                iter %= 4;
                output = lightShow1[iter];
                ++iter;
                slowTime = 0;
            }
            ++slowTime;
        break;
        }

    resetBuffer();
    writeBuffer(output);
    Serial.print(state);
    Serial.print(" ");
    Serial.print(slowTime);
    Serial.print(" ");
    Serial.print(lightLevel);
    Serial.println();
    return state;
}

void loop(){
    ///Default arduino function
    // We have to cast the return value of the tick function to type States
  if ((millis() - lastRan) > period) {
    SM1_state = (States)SM1_Tick(SM1_state);
    lastRan = millis();
  }
}
