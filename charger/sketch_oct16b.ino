/*
 * This code controls LEDs based on a state.
 * 
 */

#define STATE_NO_POD 0
#define STATE_DOCKED_NO_BOND 2
#define STATE_DOCKED_PAIRING 3
#define STATE_DOCKED_BONDED_CHARGING 4
#define STATE_DOCKED_BONDED_CHARGED 5
#define STATE_DOCKED_OTA_IN_PROGRESS 6
#define STATE_ERROR 7
#define LED_MAX 255
#define LED_MIN 0
#define BLINK_COUNTER_MAX 30

int rled_brightness = 0;    // how bright the Red LED is
int gled_brightness = 0;    // how bright the Green LED is
int next_pulse_value = 0;   // Counter for pulsing-throbbing LEDs, PWM outputs only
int blink_counter = 0;      // Counter for blinking LEDs at 1 Hz
int fadeAmount = 5;    // step size (of 255) to change the LED by in each loop
int chargerState = STATE_NO_POD;  // default state of charger on startup
bool blinkState = 0;    // state of blinking LEDs

void setup() {
  // Declare LED pins to be an output
  pinMode(GREEN_LED, OUTPUT); // pin 1.1
  pinMode(RED_LED, OUTPUT);   // pin 1.0

  // initialize serial port:
  Serial.begin(9600);
}

void loop() {

  // if there's any serial available, read it and set dock state
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    int firstInput = Serial.parseInt();
  
    // look for the newline. That's the end of your
    // sentence:
    if (Serial.read() == '\n') {
    // constrain the values to valid values 
    firstInput = constrain(firstInput, 100, 104);

    // print the number:
    Serial.write("Switching state to: ");
    Serial.print(firstInput, HEX);
    Serial.write("\n");
    chargerState = firstInput;
    }
  }

  // Determine the function of each LED based on the state of the dock
  /* 
   *  STATE_NO_POD: Steady blue
   *  STATE_DOCKED_NO_BOND: Pulsing Blue 1.25Hz
      STATE_DOCKED_PAIRING Blue off Red off at 1.25Hz
      STATE_DOCKED_BONDED_CHARGING : Pulse Red at 1.25Hz
      STATE_DOCKED_BONDED_CHARGED : Steady Green
      STATE_DOCKED_OTA_IN_PROGRESS : Pulse Red at TBD
      STATE_ERROR : Pulse blue at 5Hz
   */
  switch(chargerState) {
    case STATE_NO_POD: // Red is blinking, green is pulsing
      digitalWrite(RED_LED, blinkState);
      analogWrite(GREEN_LED, next_pulse_value);
      break;
    case STATE_DOCKED_NO_BOND:
      digitalWrite(RED_LED, blinkState);
      digitalWrite(GREEN_LED, blinkState);
      break;
    default:
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
      break;
  }

  // The pulsing brightness value changes each time in the loop
  next_pulse_value = get_pulse_brightness(next_pulse_value, &fadeAmount);

  // Blink counter
  blink_counter++;
  if(blink_counter > BLINK_COUNTER_MAX){
    blink_counter = 0;
    toggle_digital_state(&blinkState);
  }
 
  // wait for 30 milliseconds for each loop  
  delay(30);                       
}


// Function returns the new brightness for each time through the loop
int get_pulse_brightness(int prev_brightness, int *delta){
  int new_brightness;
  new_brightness = constrain(prev_brightness + *delta,0,255);

  // reverse the direction of the fading at the ends of the fade: 
  if (new_brightness == 0 || new_brightness == 255) {
    *delta = -*delta ; 
  }
  
  return new_brightness;
}

// Function that inverts the current state of the variable
void toggle_digital_state(bool *current_state) {
  *current_state = !*current_state;
}
