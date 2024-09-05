#include <avr/sleep.h> // library for sleep function
#include <dht11.h>    // temprature sensor 

#define DHT11PIN  A0
#define buttonPin 2

/*
  Showing number 0-9 on a Common Anode 7-segment LED display
  Displays the numbers 0-9 on the display, with one second inbetween.
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
 */
// Pin 3-8 + 13 is connected  to the 7 segments of the display.
// Pin 9 - 12 are the corresponding digit activators 
int pinA = 13;
int pinB = 3;
int pinC = 4;
int pinD = 5;
int pinE = 6;
int pinF = 7;
int pinG = 8;
int D1 = 9;
int D2 = 10;
int D3 = 11;
int D4 = 12;

int digitIndex = 0;  // this flag determins whicj digit of 4 will be enabled avery time 

// this flag indicates which mode the display uses :
// 1st mode(first push of button) : display the maximum Temp and Hum difference ever measured
// 2nd mode(second push of button) : display the current Temp and Hum measured
int displayIndex = 0; 

dht11 DHT11; //DHT module (For Temp Humidity)


int  currentTemp = 0; //last measurement of Temprature
int  currentHum  = 0; //last measurement of Humidity


int maxTempOfDay = 0;  // maximum Temprature measured 
int minTempOfDay = 0;  // minimum Temprature measured 
int maxHumOfDay = 0;  // maximum Humidity measured 
int minHumOfDay = 0;  // minimum Humidity measured 
int maxTempDiff = 0;  // maximum Temprature difference 
int maxHumDiff = 0;   // maximum Humidity difference 

int dayTime = 0; // 86400/4 = 21600(counter need to count to 21600 to end a full day)





//****** the setup function runs once when you press reset or power the board ****** 
//*                                                                                * 
//*                    Enable interrupts and initialize In/Out                     *
//*                                                                                *
//**********************************************************************************
void setup() {

  cli();//stop interrupts


  //-----------set timer1 interrupt at 0.25Hz--------------
  TCCR1A = 0;// set entire TCCR1A register to 0 (increase every cycle)
  TCCR1B = 0;// same for TCCR1B prescaler ( manages how fast timer is counting)
  TCNT1  = 0;//initialize counter value to 0 (counts every time TCCR1A = TCCR1B)
  // set compare match register for 1hz increments
  OCR1A = 62499;// interrupt every 4s = (16*10^6) / (0,25*1024) - 1 (must be <65536)
  // turn on CTC(Clear Timer Compare Match) mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler(one count every 1024 clock periods)
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();//allow interrupts
//-----------end set timer1 interrupt at 0.25Hz--------------

  Serial.begin(9600); // open the serial port at 9600 bps:
  pinMode(buttonPin, INPUT); // initialize interupt input pin for pushbutton to pin A0
  attachInterrupt(digitalPinToInterrupt(buttonPin), Display, RISING); // push button interupt enable

         
  // initialize  the digital pins as outputs for display.
  pinMode(pinA, OUTPUT);     
  pinMode(pinB,  OUTPUT);     
  pinMode(pinC, OUTPUT);     
  pinMode(pinD, OUTPUT);     
  pinMode(pinE, OUTPUT);     
  pinMode(pinF, OUTPUT);     
  pinMode(pinG,  OUTPUT);   
  pinMode(D1, OUTPUT);  
  pinMode(D2, OUTPUT);  
  pinMode(D3,  OUTPUT);  
  pinMode(D4, OUTPUT); 
}





//****** the loop function runs over and over again forever ****** 
//*                                                              * 
//*                     Do nothing but sleep                     *
//*                                                              *
//****************************************************************
void loop() {

  // There are five different sleep modes in order of power saving:
	// SLEEP_MODE_IDLE - the lowest power saving mode
	// SLEEP_MODE_ADC
	// SLEEP_MODE_PWR_SAVE
	// SLEEP_MODE_STANDBY
	// SLEEP_MODE_PWR_DOWN - the highest power saving mode
  set_sleep_mode(SLEEP_MODE_IDLE); // sleep but allow interrupts
	sleep_enable();

	// Now enter sleep mode.
	sleep_mode();
}



//****************************************************************
//*                                                              * 
//*              Enable timer for display digits                 *
//*                                                              *
//****************************************************************
void Display() {
  if(displayIndex == 0){
    //-----------set timer2 interrupt -------------
    TCCR2A = 0;// set entire TCCR2A register to 0 (increase every cycle)
    TCCR2B = 0;// same for TCCR2B prescaler ( manages how fast timer is counting)
    TCNT2  = 0;//initialize counter value to 0 (counts every time TCCR2A = TCCR2B)
    // set compare match register for 1hz increments
    OCR2A = 250;// interrupt every 0,06s , 16,5Hz = (16*10^6) / (250*1024) - 1 (must be <255)
    // turn on CTC(Clear Timer Compare Match) mode
    TCCR2A |= (1 << WGM21);
    // Set CS20 and CS22 bits for 1024 prescaler(one count every 1024 clock periods)
    TCCR2B |= (1 << CS22) | (1 << CS20);  
    // enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);
    //-----------end set timer2 interrupt -------------
    displayIndex = 1;
    Serial.print("Display Max Temp and Hum Difference");
  }
  else if(displayIndex == 1){
    displayIndex = 2;
    Serial.print("Display current Temp and Hum Difference");
  }
  else{
    Serial.print("Disable Display");
    displayIndex = 0;
    // Disable all Timer2 interrupts (set its clock to 0)
    TCCR2B &= ~((1 << CS22) | (1 << CS21) | (1 << CS20));
    //DIsable all digits
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH); 
  }

}


//****************************************************************
//*                                                              * 
//*          Visualize the correct digit in the display          *
//*                                                              *
//****************************************************************

void indexVisualize (int number, int index){

  // activate the correct index (one of the 4 digits of the dispaly)(active low)
  if(index == 0){ // activate 1st digit
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH); 
  }
  else if(index == 1){ // activate 2nd digit
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, HIGH); 
  }
  else if(index == 2){ // activate 3rd digit
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH); 
  }
  else if(index == 3){ // activate 4th digit
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, LOW); 
  }

  // create the decimal representation of the digit
  /*  
   A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
    */
  if(number == 0){
    //display number  0
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, HIGH);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, LOW); 
}
else if(number == 1){
    //display number  1
    digitalWrite(pinA,  LOW);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, LOW);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  LOW);   
    digitalWrite(pinG, LOW); 
}
else if(number == 2){
    //display number  2
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, LOW);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, HIGH);   
    digitalWrite(pinF,  LOW);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 3){
    //display number  3
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  LOW);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 4){
    //display number  4
    digitalWrite(pinA,  LOW);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, LOW);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 5){
    //display number  5
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, LOW);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 6){
    //display number  6
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, LOW);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, HIGH);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 7){
    //7
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, LOW);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  LOW);   
    digitalWrite(pinG, LOW); 
}
else if(number == 8){
    //display number 8
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, HIGH);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, HIGH); 
}
else if(number == 9){
    //display number  9
    digitalWrite(pinA,  HIGH);   
    digitalWrite(pinB, HIGH);   
    digitalWrite(pinC, HIGH);   
    digitalWrite(pinD, HIGH);   
    digitalWrite(pinE, LOW);   
    digitalWrite(pinF,  HIGH);   
    digitalWrite(pinG, HIGH); 
}
}

//****************************************************************
//*                                                              * 
//*                         UPDATE DISPLAY                       *
//*                                                              *
//****************************************************************
// At the first button push (displayIndex == 1) display the max Temp and Hum difference

// At the second button push (displayIndex == 2) display the current Temp and Hum difference 
ISR(TIMER2_COMPA_vect){
  if(displayIndex == 1){  // first button push 
     if(digitIndex == 0)
       indexVisualize (maxTempDiff/10, digitIndex); //display the first digit of max Temp Difference
     else if(digitIndex == 1)
       indexVisualize (maxTempDiff % 10, digitIndex); //display the second digit of max Temp Difference
     else if(digitIndex == 2)
       indexVisualize (maxHumDiff/10, digitIndex); // display the first digit of max Hum Difference
     else if(digitIndex == 3)
       indexVisualize (maxHumDiff % 10, digitIndex); // display the second index of max Hum Difference 
  }
   else if(displayIndex == 2){
     if(digitIndex == 0)
       indexVisualize ((currentTemp/10), digitIndex); //display the first digit of current Temp 
     else if(digitIndex == 1)
       indexVisualize ((currentTemp % 10), digitIndex); //display the second digit of current Temp 
     else if(digitIndex == 2)
       indexVisualize ((currentHum/10), digitIndex); //display the first digit of current Hum 
     else if(digitIndex == 3)
       indexVisualize ((currentHum % 10), digitIndex); //display the second digit of current Hum 
  }

  if(digitIndex==3)  // change display digit index that will be visualize next time 
    digitIndex = 0 ;
  else
    digitIndex = digitIndex + 1 ; 

}

//****************************************************************
//*                                                              * 
//*       ISR of TIMER measure Temprature and Humidity           *
//*                                                              *
//****************************************************************

ISR(TIMER1_COMPA_vect){//timer1 interrupt triggered every 4 sec 
   DHT11.read(DHT11PIN);  // read temprature and humidity from pin DHT11PIN = 13
   
  currentTemp = (int)DHT11.temperature;
  currentHum  = (int)DHT11.humidity;

// make the measurements
  Serial.println("Temp:");
  Serial.println((int)currentTemp);
  Serial.println("Hum:");
  Serial.println((int)currentHum);


   if(dayTime == 21600){ // every day reset for finding the new max and min values
      dayTime = 0 ;
      maxTempOfDay = 0;  
      minTempOfDay = 0;
      minTempOfDay = 0;
      minTempOfDay = 0;
   }
   dayTime = dayTime + 1;
   
   
   // check if max or min temprature found (and measurement didnt failed)
   if(currentTemp > maxTempOfDay & currentTemp < 50 & currentTemp > 0)
      maxTempOfDay = currentTemp;
   else if((currentTemp < minTempOfDay | minTempOfDay == 0) & currentTemp < 50 & currentTemp > 0)
      minTempOfDay = currentTemp;
  // check if max or min humidity found (and measurement didnt failed)
   if(currentHum > maxHumOfDay & currentHum < 90 & currentHum > 20)
      maxHumOfDay = currentHum;
   else if((currentHum < minHumOfDay | minHumOfDay == 0) & currentHum < 90 & currentHum > 20)
      minHumOfDay = currentHum;
   
   // check if there is a maximum difference in temprature ever
   if(maxTempOfDay-minTempOfDay > maxTempDiff & maxTempOfDay != 0 & minTempOfDay != 0)
     maxTempDiff = maxTempOfDay-minTempOfDay;
   // check if there is a maximum difference in humidity ever
   if(maxHumOfDay-minHumOfDay > maxHumDiff & maxHumOfDay != 0 & minHumOfDay != 0)
     maxHumDiff = maxHumOfDay-minHumOfDay;

}