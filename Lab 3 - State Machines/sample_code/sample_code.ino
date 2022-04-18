enum States{INIT, S0, S1} SM1_state = INIT;
int X_button = 8;

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
}
// This function takes in the state as an argument & returns the current state
int SM1_Tick(int state){
    //Read thing
    switch(state){ // State transitions
      case INIT:
         //State Transition
            state = S0;
        break;
      case S0:
         if (digitalRead(X_button) == HIGH) {
            state = S1;
         }
         else {
            state = S0;
         }
        break;
      case S1:
        if (digitalRead(X_button) == LOW) {
            state = S0;
        }
        else {
            state = S1;
        }
         //State Transition
        break;
    }
    switch(state){ // State Action
      case INIT:
         //State Action
        break;
      case S0:
        resetBuffer();
         //State Action
        break;
      case S1:
         //State Action
        writeBuffer(0x0F);
        break;
    }

    return state;
}

void loop(){ 
    ///Default arduino function
    // We have to cast the return value of the tick function to type States
    SM1_state = (States)SM1_Tick(SM1_state);
}
