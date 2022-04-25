/*        Your Name & E-mail:  Andrew Bazua abazu001@ucr.edu

*          Discussion Section: 021

 *         Assignment: Lab #4  Exercise #1

 *         Exercise Description: Reimplement Exercise 3 Double Light Show into a SynchSM

Description: Instead of the buttons changing which pattern is active in the current
             light show, the Light Show should move through the different patterns
             every period. The period of the SM should be 1 second. The photoresistor
             should behave as previously implemented. The photoresistor should not
             interrupt the cyling of patterns in a light show, only switch which
             light show is active.

 *

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: https://youtu.be/g58is7dJWdA

 */

enum States{INIT, L1, L2} SM1_state = INIT;
unsigned long lastRan;
const unsigned long period = 1000;
int X_button = 8;

// Used to track which light element will be shown
int iter = 0;

unsigned short lightLevel = 313;
// Photoresistor value that changes lightShows if value is less than lightSwitch
unsigned short lightOut = 130;
bool lightSwitch = true;

// The lightshows
char lightShow1[4] = {0x01, 0x08, 0x02, 0x04};
char lightShow2[4] = {0x05, 0x0A, 0x03, 0x0C};

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
                lightSwitch = true;
            }
            else if ((lightLevel <= lightOut) && lightSwitch) {
                state = L2;
                lightSwitch = false;
            }
            else {
                state = L1;
            }
        break;

      case L2:
          if (lightLevel > lightOut) {
              state = L2;
              lightSwitch = true;
          }
          else if ((lightLevel <= lightOut) && lightSwitch) {
              state = L1;
              lightSwitch = false;
          }
          else {
              state = L2;
          }
        break;
    }
    switch(state){ // State Action
        case INIT:
            //State Action
            iter = 0;
            lightSwitch = true;
            lightLevel = analogRead(A1);
        break;

        case L1:
            lightLevel = analogRead(A1);
            iter %= 4;
            output = lightShow1[iter];
            ++iter;
        break;

        case L2:
            lightLevel = analogRead(A1);
            iter %= 4;
            output = lightShow2[iter];
            ++iter;
        break;
        }

    resetBuffer();
    writeBuffer(output);
    Serial.print(state);
    Serial.print(" ");
    Serial.print(lightSwitch);
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
