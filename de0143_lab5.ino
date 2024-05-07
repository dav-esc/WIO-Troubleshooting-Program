/************************************************************
This program runs a troubleshooting menu on the serial console 
for the WIO Terminal board by SEEED.
The sensors (acceleometer, AHT20 temp and humidity sensor, and light)
and LCD screen that are tested in this program uses 
libraries from the WIO Terminal wiki:
https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/

By: David Escobedo
*************************************************************/
#include"TFT_eSPI.h"
#include <Wire.h>
#include "AHT20.h"
#include"LIS3DHTR.h"

TFT_eSPI tft;
LIS3DHTR<TwoWire> lis;
AHT20 AHT;

/*SET GLOBAL VARIABLES*/
String s;   //string to get input
char keyboard[10]; //This will hold the input #
unsigned long prevMillis = 0; //will hold old millis value
unsigned long interval = 5000; //any interval value in ms
unsigned long initialTimer = 2500; //used to display the menu on startup (2s)
unsigned long currentMillis;//get current program time (ms)
unsigned long LEDinterval = 500; //time (ms) before blinking the LED again
int LEDstate = LOW; //LED set to low initially
int  LEDcount = 0; //count # of LED blinks
int LEDblinks = 0; //user sets number of blinks
int in; //switch case option
int count = 0; //condition used to print the menu
bool displayOnce = true; //The condition to display the menu once
bool prntMenu = false;  //condition variable to display the menu

/*Functions to get sensor values, menu and use the buzzer*/
//accelerometer value
void acc(){
  float x, y, z;
  //get values
  x = lis.getAccelerationX();
  y = lis.getAccelerationY();
  z = lis.getAccelerationZ();
  //values in radians
  x=acos(x/sqrt(x*x+y*y+z*z));
  y=acos(y/sqrt(x*x+y*y+z*z));
  z=acos(z/sqrt(x*x+y*y+z*z));
  //values in degrees
  float x_ang=(x*180)/PI;
  float y_ang=(y*180)/PI;
  float z_ang=(z*180)/PI;
  Serial.print("Accelerometer values (x, y, z): ");
  
  Serial.print(x_ang);
  Serial.print(", ");
  Serial.print(y_ang);
  Serial.print(", ");
  Serial.print(z_ang);
  Serial.println();
}

//get temperature readings
void temp(){  
  float humi, tempe;  
  AHT.getSensor(&humi, &tempe);
  Serial.print("Temperature: ");
  Serial.print(tempe);
  Serial.print(" C");
  Serial.println();
}

//get humidity readings
void humid(){
  float humi, tempe;
  AHT.getSensor(&humi, &tempe);
  humi = humi*100;
  Serial.print(" Humidity: ");
  Serial.print(humi);
  Serial.print("%");
  Serial.println();
}

//get light value
void light(){
  //get Light sensor readings
  int light = analogRead(WIO_LIGHT);
  Serial.print("Light Value: ");
  Serial.println(light);
}

//generate buzzer
void buzzer(){
  analogWrite(WIO_BUZZER, 200);
  delay(100);
  analogWrite(WIO_BUZZER, 0);
  delay(100);
}

//troubleshooting menu
void menu(){
  Serial.println("\t\t\t\tTroubleshooting Program");
  Serial.println("1. Read the temperature and print the value in Celsius");
  Serial.println("2. Read the humidity and print the value in %");
  Serial.println("3. Read the accelerometer and print the x, y, and z angles");
  Serial.println("4. Read the ambient light intensity and print the light value");
  Serial.println("5. Write the input text to the LCD screen and find the number of words and vowels");
  Serial.println("6. Blink the blue LED (LED_BUILTIN) based on the user input");
  Serial.println("7. Generate a beep tone using the buzzer");
  Serial.print("\nPlease input the selection [1 - 7]:");
}

//Get user input from serial Terminal.
//Display first character as uppercase
//and the remaining characters as lowercase.
//Count number of words and vowels and display on the LCD screen.
void lcd(){
  
  int vowels = 0;
  int words = 0;
  tft.setCursor(0,0);
  //Reset screen settings
  tft.fillScreen(TFT_GREEN);
  s.toLowerCase(); //set string to lower case
  s.setCharAt(0, toupper(s.charAt(0))); //set first letter to uppercase
  tft.println(s);
  //count number of vowels
  for(int i=0; i <= s.length(); i++){
    if(s.charAt(i) == 'a'|| s.charAt(i) == 'e'|| s.charAt(i) == 'i' || s.charAt(i) == 'o'|| s.charAt(i) == 'u' ||s.charAt(i) == 'A'|| s.charAt(i) == 'E'|| s.charAt(i) == 'I' || s.charAt(i) == 'O'|| s.charAt(i) == 'U')
     vowels++;
  }
  for(int i=0; i <= s.length(); i++){
    if(s.charAt(i) == ' ' || s.charAt(i) == '\0')
     words++;
  }
  tft.print("number of words: ");
  tft.println(words);
  tft.print("number of vowels: ");
  tft.println(vowels);


}

void cases(){
   
  switch(in){
      case 1:
        temp();
        break;
      case 2:
        humid();
        break;
      case 3:
        acc();
        break;
      case 4:
        light();
        break;
      case 5:
       //get user string
        Serial.print("Enter the string: "); 
        while(Serial.available()<=0);
        s = Serial.readStringUntil('\n'); //place in string
        Serial.println(s);
        lcd();
        prntMenu=false;
        break;
      case 6:
      //get user input for blinks
        Serial.print("Enter number of blinks: "); 
        while(Serial.available()<=0);
        s = Serial.readStringUntil('\n'); //place in string
        s.toCharArray(keyboard, s.length()); //place in char
        LEDblinks = atoi(keyboard); //char to int
        Serial.println(LEDblinks); 
        LEDblinks = LEDblinks*2+1; //account for on and off states
        prntMenu = false;
        break;
      case 7:
        buzzer();
        Serial.println("Tone generated!");
        break;
      default:
        break;
    }
}

void setup() {
  //set serial temrinal to 9600 baud
  Serial.begin(9600);
  /*configure screen settings*/
  tft.begin();
  tft.setRotation(3);                   //set screen rotation
  tft.fillScreen(TFT_GREEN);            //set the screen color
  tft.setTextColor(TFT_BLACK);          //sets the text colour to black
  tft.setTextSize(2);                   //set the text size
  digitalWrite(LCD_BACKLIGHT, HIGH);    //turn on backlight
  
  //start accelerometer and temp sensors
  lis.begin(Wire1); 
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ);
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); 
  AHT.begin();
  //set LED and Buzzer
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  //get program time
    currentMillis = millis();
  
  //Display the menu once on startup
  if(displayOnce != false){
   if((currentMillis - prevMillis >= initialTimer)) {
     prevMillis=currentMillis;  
     menu();
     displayOnce = false;
    }
  }
  //Get user input for cases
  if(Serial.available()){
    s = Serial.readStringUntil('\n'); //read input
    s.toCharArray(keyboard, s.length());//string to char
    in = atoi(keyboard); //char to int
    Serial.println(in); //get user choice
    prntMenu = true; //reset the flag
    cases(); //go to cases
    
  }
  
  //print the menu every 5 seconds once a selection is made
  if(prntMenu==true){
    if(currentMillis - prevMillis >= interval){
      prevMillis = currentMillis;
      if(count>0){
       menu();
       prntMenu=false;
      } 
      count++;
    }
  }else if(count >= 1) count=0;
  
  //allows the menu to print on time
  if(in==5||in==6){
    prntMenu = true;
   }

  //blink the LED
  if(LEDblinks > 0){
    if(currentMillis - prevMillis >= LEDinterval){
          prevMillis = currentMillis;
          if(LEDstate == LOW){
            LEDstate = HIGH;
          } else LEDstate = LOW;
            digitalWrite(LED_BUILTIN, LEDstate);
            LEDcount++;
            
          }
   //reset blinking
   if(LEDcount>LEDblinks){
      LEDcount = 0;
      LEDblinks = 0;
     LEDstate = LOW;
     digitalWrite(LED_BUILTIN, LEDstate);     
   }
   
  }
 
}
  