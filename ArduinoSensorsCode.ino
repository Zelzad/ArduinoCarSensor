/***************************************************
Code made for different engine sensors.

Much of the code is put together from different websites.
The oil pressure and oil temp sensor code is from Oven garage youtube channel https://www.youtube.com/c/TylerOvens.
 ****************************************************/
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MAX31855.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//Different variables used in the code
double maxtemp = 0;
long CurrentTime;
long NewTime;
double maxCn1;
double maxCn2;
double maxCnF;
int maxCnT;


//DISPLAYS
//Defining the pins for the ST7735 displays. Replace XX with the digital pin used.
#define CS_pin1 XX //yellow
#define CS_pin2 XX
#define A0_pin1 XX //blue
#define A0_pin2 XX
#define RST_pin1 XX //whie
#define RST_pin2 XX



//OILTEMP
#define coolantsensorDivider XXXX   //resistance in ohms for the voltage divider circuit used for the oil temp sensor
#define coolantsensorPin XX         //defines the analog pin of the input voltage from the voltage divider
#define NUMSAMPLES 5                //defines the number of samples to be taken for a smooth average




//EGTS
//Defining the pins for the MAX31855 interface, note that when running multiple MAX31855 the can share the DO and CLK pins but require individual CS pins.
#define MAXDO   XX
#define MAXCLK  XX
#define MAXCS1  XX
#define MAXCS2  XX
#define MAXCS3  XX
#define MAXCS4  XX

const int holdtimeEGT=10000;            //The amount of time in ms the maximum values achieve between all the EGTs is held


// Initialize the Thermocouple
Adafruit_MAX31855 thermocouple1(MAXCLK, MAXCS1, MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, MAXCS2, MAXDO);
Adafruit_MAX31855 thermocouple3(MAXCLK, MAXCS3, MAXDO);
Adafruit_MAX31855 thermocouple4(MAXCLK, MAXCS4, MAXDO);


//SCREENS
Adafruit_ST7735 tft1 = Adafruit_ST7735( CS_pin1, A0_pin1, RST_pin1 );
Adafruit_ST7735 tft2 = Adafruit_ST7735( CS_pin2, A0_pin2, RST_pin2 );



//OILPREASSURE
//These parameters are used for a 100PSI sensor.
const int pressureInput= XX;      //Analog input for the oil pressure sensor.
const int pressureZero= 102.4;
const int pressureMax= 921.6;
const int pressuretransducermaxPSI=100;
float pressureValue = 0;



//OILTEMP
const float steinA = XXX;                        //steinhart constant A
const float steinB = XXX;                        //steinhart constant B
const float steinC = XXX;                        //steinhart constant C
int samples[NUMSAMPLES];                              //variable to store number of samples to be taken


//VOLTAGE
const int voltageInput=XX;      //Analog input for voltage sensor.


void setup() {

  Serial.begin(9600);
  
  tft1.initR(INITR_BLACKTAB); 
  tft1.setSPISpeed(40000000);
  tft1.fillScreen(ST7735_BLACK);
  tft1.setTextColor(ST7735_WHITE, ST7735_BLACK);

  tft2.initR(INITR_BLACKTAB);
  tft2.setSPISpeed(40000000);
  tft2.fillScreen(ST7735_BLACK);
  tft2.setTextColor(ST7735_WHITE, ST7735_BLACK);


  delay(500);
  

}

void loop() {

  //EGTS
  CurrentTime=millis();
   double c1 = thermocouple1.readCelsius();
   double c2 = thermocouple2.readCelsius();
   double c3 = thermocouple3.readCelsius();
   double c4 = thermocouple4.readCelsius();

  Serial.print("      T1: ");
  Serial.print(c1);
  
  Serial.print("      T2: ");
  Serial.print(c2);

  Serial.print("      T3: ");
  Serial.print(c3);

  Serial.print("        T4: ");
  Serial.println(c4);

     tft2.setTextSize(2);
     
     tft1.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
     tft1.setTextSize(1);
     tft1.setCursor(5,5);
     tft1.print("Cyl:  T:");


     tft1.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
     tft1.setTextSize(2);
     tft1.setCursor(5,20);
     tft1.print("1  ");
     tft1.print(c1,1);
     tft1.println(" ");

     tft1.setTextSize(2);
     tft1.setCursor(5,45);
     tft1.print("2  ");
     tft1.print(c2,1);
     tft1.println(" ");

     tft1.setTextSize(2);
     tft1.setCursor(5,70);
     tft1.print("3  ");
     tft1.print(c3,1);
     tft1.println(" ");

     tft1.setTextSize(2);
     tft1.setCursor(5,95);
     tft1.print("4  ");
     tft1.print(c4,1);
     tft1.println(" ");
         

     maxCn1 = max(c1,c2);                 //Calculated max value of the first 2 EGTs
     maxCn2 = max(c3,c4);                 //Calculated max value of the remaining 2 EGTs

     maxCnF = max(maxCn1, maxCn2);        //Calculates the max values of all the EGTs and asings it's values to maxCnF 
    

    if (maxCnF > maxtemp){
      NewTime=CurrentTime+holdtimeEGT;
    }


     if(maxCnF>maxtemp){
      maxtemp=maxCnF;
     }
   
    if (NewTime < CurrentTime){
      maxtemp=maxCnF;
    }

    tft1.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft1.setTextSize(1);
    tft1.setCursor(0,115);
    tft1.println("Cyl:   Max T:");

     if (maxCnF==c1){
      maxCnT= 1;
     }

     if(maxCnF==c2){
      maxCnT= 2;
     }   
       
     if(maxCnF==c3){
      maxCnT= 3;
     }

     if(maxCnF==c4){
      maxCnT= 4;
     }
    
     tft1.setCursor(5,130);                                     //Displays the maximum EGT value and hold it for a set amount of time
     tft1.setTextSize(2);
     tft1.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
     tft1.print(maxCnT);
     tft1.print("  ");
     tft1.print(maxtemp,1);
     tft1.println(" ");


  //OILPREASSURE
  pressureValue = analogRead(pressureInput);
  pressureValue=((pressureValue-pressureZero)*pressuretransducermaxPSI)/(pressureMax-pressureZero);
  pressureValue = pressureValue/14.504;
     
  tft2.setCursor(0,5);                      //prints oil pressure value on screen 2
  tft2.setTextSize(1);
  tft2.print("Oil Preassure (bar): ");
  tft2.setTextSize(2);
  tft2.setCursor(0,20);
  tft2.println(pressureValue);



  

  //VOLTAGE
  int volt = analogRead(voltageInput);
  double voltage = map(volt,0,1023, 0, 2500);

  voltage /=100;
      
  tft2.setCursor(0,40);                      //Prints voltage value on screen 2.
  tft2.setTextSize(1);
  tft2.print("Voltage");
  tft2.setTextSize(2);
  tft2.setCursor(0,55);
  tft2.println(voltage);


  
  //OILTEMP
  uint8_t i;                                          //integer for loop
  float average;                                      //decimal for average
  
  for (i=0; i<NUMSAMPLES; i++) {                      
    samples[i] = analogRead(coolantsensorPin);        //takes samples at number defined with a short delay between samples
    delay(10);
  }
  

  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
    average += samples[i];                            //adds all number of samples together
  }
  average /= NUMSAMPLES;                              //divides by number of samples to output the average


  average = (coolantsensorDivider*average)/(1023-average);        //conversion equation to read resistance from voltage divider


  float steinhart;                              //steinhart equation to estimate temperature value at any resistance from curve of thermistor sensor
  steinhart = log(average);                     //lnR
  steinhart = pow(steinhart,3);                 //(lnR)^3
  steinhart *= steinC;                          //C*((lnR)^3)
  steinhart += (steinB*(log(average)));         //B*(lnR) + C*((lnR)^3)
  steinhart += steinA;                          //Complete equation, 1/T=A+BlnR+C(lnR)^3
  steinhart = 1.0/steinhart;                    //Inverse to isolate for T
  steinhart -= 273.15;                          //Conversion from kelvin to celcius

  tft2.setTextSize(1);                          //Prints temp value on screen 2
  tft2.setCursor(0,75);
  tft2.print("Oil Temp (C): ");
  tft2.setTextSize(2);
  tft2.setCursor(0,90);
  tft2.println(steinhart);


  delay(100);

   
}
