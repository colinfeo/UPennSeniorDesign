#include <Encoder.h>

// onboard LED
const int led = 13;

boolean left;
boolean right;

// mouse clicking
const int L_click = 10;
const int R_click = 11;

// quadrature encoder inputs
const int quad_L2A = 1;
const int quad_L2B = 2;
const int quad_L3B = 7;
const int quad_L3A = 8;
const int quad_L1B = 16;
const int quad_L1A = 17;
const int quad_L0A = 21;
const int quad_L0B = 22;

// motor output PWM signals
const int motor_NW = 3;
const int motor_SW = 4;
const int motor_NE = 6;
const int motor_SE = 5;

// limit switch inputs for calibration
const int lim_A = 14;
const int lim_B = 15;

// calibration button input
const int cal_button = 12;

//force sensitive resistor
const int fsr = 23;

// lengths of wires at bottom left corner
const int cal_L0 = 4501;
const int cal_L1 = 5239;
const int cal_L2 = 2987;
const int cal_L3 = 1120;

//holds encoder counts
int posit1;
int posit2;
int posit3;
int posit4;

// minimum tension on motors
int minPWM = 60;

//4 encoders
Encoder myEncL0(quad_L0A, quad_L0B);  // uses encoder library
Encoder myEncL1(quad_L1A, quad_L1B);
Encoder myEncL2(quad_L2A, quad_L2B);
Encoder myEncL3(quad_L3A, quad_L3B);

byte incomingByte;
int buf[4];
char incomingBytes[16];

int byteCount;

// calibration flag
int cal_flag = 0;

unsigned long sendTime;
// the setup() method runs once, when the sketch starts
void setup() {
  sendTime = millis() + 10.0;
//  Serial.begin(38400);
 Serial.begin(115200);
  // turn orange onboard LED on
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  // input limit switches
  pinMode(lim_A, INPUT);
  pinMode(lim_B, INPUT);
  
  // clicking
  pinMode(L_click, INPUT);
  pinMode(R_click, INPUT);  
  
  // calibration button
  pinMode(cal_button, INPUT);
  
  byteCount = 0;
}

// the loop() method runs over and over again,
// as long as the board has power
void loop() {
  // stay in this loop until the jawn has been calibrated
  while(!cal_flag) {
    analogWrite(5,minPWM);
    analogWrite(6,minPWM);
    analogWrite(3,minPWM);
    analogWrite(4,minPWM);
//    analogWrite(5,250);
//    analogWrite(6,250);
//    analogWrite(3,250);
//    analogWrite(4,250);
    
    // if calibration button is pushed, pull to bottom left until both limit switches are touched
    if (digitalRead(cal_button)) {
      //Serial.print("BUTTON");
      // pull to bottom left until both limits switch are high 
      while(!digitalRead(lim_A) || !digitalRead(lim_B)){
        analogWrite(5,minPWM);
        analogWrite(6,minPWM);
        analogWrite(3,minPWM);
        analogWrite(4,200); // pull to bottom left corner
        digitalWrite(led, LOW);
      }
      digitalWrite(led, HIGH);
      // calibrate encoders to bottom left position
      myEncL0.write(cal_L0);
      myEncL1.write(cal_L1);
      myEncL2.write(cal_L2);
      myEncL3.write(cal_L3);
      delay(500); // wait half a second
      cal_flag = 1; // set calibration flag so it exits this loop
    }
  }
  
  // in case we ever need to calibrate again, we poll for calibration button presses here
 if (digitalRead(cal_button)) {
      // pull to bottom left until both limits switch are high 
      while(!digitalRead(lim_A) || !digitalRead(lim_B)){
        analogWrite(5,minPWM);
        analogWrite(6,minPWM);
        analogWrite(3,minPWM);
        analogWrite(4,200); // pull to bottom left corner
      }
      // calibrate encoders to bottom left position
      myEncL0.write(cal_L0);
      myEncL1.write(cal_L1);
      myEncL2.write(cal_L2);
      myEncL3.write(cal_L3);
 }
  if (millis() > sendTime)
  {
    sendTime = millis() + 10; 
    
    posit1 = myEncL0.read();
    posit2 = myEncL1.read();
    posit3 = myEncL2.read();
    posit4 = myEncL3.read();
 
    left = digitalRead(L_click);
    right = digitalRead(R_click);
   
     
    buf[0] = posit1;
    buf[1] = posit2;
    buf[2] = posit3;
    buf[3] = posit4;
   
//    buf[5] = 5;
//    buf[6] = 6;
//    buf[7] = 7;
//    
    
    // check for button clicks
    if (left && right) {
      buf[2] = -buf[2];
//      buf[4] = 3;
    }
    if (!left && !right) {
//      buf[4] = 0;
//    do nothing
    }    
    if (!left && right) {
//      buf[4] = 2;
      buf[1] = -buf[1];
    }    
    if (left && !right) {
//      buf[4] = 1;
      buf[0] = -buf[0];
    }
      
   
    Serial.write((char*)buf,16);
    Serial.flush();
    Serial.send_now();

  }
  
  if (Serial.available()) {
   while (Serial.available() && byteCount < 16) {
      incomingBytes[byteCount] = NULL;
      incomingBytes[byteCount] = Serial.read();  // will not be -1
      byteCount++;
    }
   byteCount = 0;
  }
  int* incomingInts = (int*)incomingBytes; 
  

  
  // last minute safety check to make sure we don't send too high or too low PWM signals to the motors
  if (incomingInts[0] < minPWM) incomingInts[0] = minPWM;
  if (incomingInts[1] < minPWM) incomingInts[1] = minPWM;
  if (incomingInts[2] < minPWM) incomingInts[2] = minPWM;
  if (incomingInts[3] < minPWM) incomingInts[3] = minPWM;
  
  if (incomingInts[0] > 255) incomingInts[0] = 255;
  if (incomingInts[1] > 255) incomingInts[1] = 255;
  if (incomingInts[2] > 255) incomingInts[2] = 255;
  if (incomingInts[3] > 255) incomingInts[3] = 255;  
  analogWrite(5,incomingInts[0]);
  analogWrite(6,incomingInts[1]);
  analogWrite(3,incomingInts[2]);
  analogWrite(4,incomingInts[3]);  
  
}
