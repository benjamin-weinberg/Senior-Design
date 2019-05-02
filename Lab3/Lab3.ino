#include <DallasTemperature.h>
#include <OneWire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define AC 3
#define HEAT 2

// RTC variables
RTC_DS3231 rtc;
DateTime now;
DateTime last;
int lastTenSec;
int lastOneSec;
char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

// Temp sensor variables
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius=0;
float Fahrenheit=0;
int oldTemp;
int newTemp;

// Graphics variables
int currentPage = 2;
int previousPage = 0;
//int heating = 0;
//int cooling = 0;
//int programable = 0;
//int autoMode = 0;
//int hold = 0;
bool heating = false;
bool cooling = false;
bool programmable = false;
bool autoMode = false;
bool hold = false;  
bool dayPoint1, dayPoint2, dayPoint3, dayPoint4 = false;
bool endPoint1, endPoint2, endPoint3, endPoint4 = false; 
TS_Point p2;

int newSetTemp = 70;
int oldSetTemp = 70;

void setup(void) {

  pinMode(HEAT, OUTPUT);
  pinMode(AC, OUTPUT);

  while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  
  tft.begin();

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");

  // Get initial temperature data
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0);
  oldTemp=(int)sensors.toFahrenheit(Celcius);
  newTemp=oldTemp;

  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  drawMenu(currentPage, previousPage);
  updateMenu(currentPage, previousPage);
  drawHomeScreen();

  // Set up the RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Get initial time
  last = rtc.now();
  lastTenSec = (int)last.second()/10;
  lastOneSec = (int)(last.second() - lastTenSec*10);
    
}

void loop() {

  // Get temperature
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0);
  newTemp=(int)sensors.toFahrenheit(Celcius);
  if(oldTemp != newTemp){ // detect change in temperature
    if(currentPage == 2){ // if on home screen, draw it
      drawTemp();
    }
    oldTemp = newTemp;
  }

  // Update clock
  drawClock();

  // Wait for a touch
  if (! ctp.touched()) {

  }

  else{
    // Retrieve a point  
    TS_Point p = ctp.getPoint();
    
  
    // Print out raw data from screen touch controller
    Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print(" -> ");
  
  
    // flip it around to match the screen.
    p.x = map(p.x, 0, 240, 240, 0);
    p.y = map(p.y, 0, 320, 320, 0);
  
    p2.x = p.y;
    p.y = p.x;
    p.x = p2.x;
  
    // Print out the remapped (rotated) coordinates
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
  
    //menu touch
    if(p.y<40){
      // ?? screen
      if(p.x<64){
        previousPage = currentPage;
        currentPage = 0;
        clearScreen();
        drawMenu(currentPage, previousPage);
        
      }
      // programming screen
      else if(p.x<128){
        previousPage = currentPage;
        currentPage = 1;
        clearScreen();
        drawMenu(currentPage, previousPage);
        drawSpScreen();
      }
      // home screen
      else if(p.x<192){
        previousPage = currentPage;
        currentPage = 2;
        clearScreen();
        drawMenu(currentPage, previousPage);
        drawHomeScreen();
      }
      // set temp screen
      else if(p.x<256){
        previousPage = currentPage;
        currentPage = 3;
        clearScreen();
        drawMenu(currentPage, previousPage);
        drawSetTemp();
      }
      // ?? screen
      else if(p.x<320){
        previousPage = currentPage;
        currentPage = 4;
        clearScreen();
        drawMenu(currentPage, previousPage);
      }
    }
    //listening for touches near HVAC settings
    //on home screen only
    else if(currentPage == 2){
      if(p.y>180 && p.y <235)
      {
        if(p.x < 320 && p.x> 200)
        {
          heating = !heating;
          drawHomeScreen();
        }
      }
      else if(p.y > 120 && p.y<200)
      {
        if(p.x<320 && p.x>200)
        {
          cooling = !cooling;
          drawHomeScreen();
        }
      }
      else if(p.y >85 && p.y<120)
      {
        if(p.x<320 && p.x>200)
        {
          autoMode = !autoMode; 
          drawHomeScreen();
        }
      }
      else if(p.y >40 && p.y < 85)
      {
        if(p.x<320 && p.y < 200)
        {
          hold = !hold;
          drawHomeScreen(); 
        }
      }
    }
    // listen for arrow up/down
    // on set temp screen
    else if(currentPage == 3){
      if(p.y>=115 && p.y <235)
        {
        //  if(p.x < 320 && p.x> 200)
        //  {
        Serial.println("Increasing temp...");
            oldSetTemp = newSetTemp;
            newSetTemp++;
            Serial.print("new set temp: ");
            Serial.print(newSetTemp);
            Serial.print("\n");
            drawSetTemp();
        //  }
        }
      else if(p.y >40 && p.y<115)
        {
          //if(p.x<320 && p.x>200)
          //{
          Serial.println("Decreasing temp...");
            oldSetTemp = newSetTemp;
            newSetTemp--;
            Serial.print("new set temp: ");
            Serial.print(newSetTemp);
            Serial.print("\n");
            drawSetTemp();
          //}
        }
    }
    else{
//      heating = !heating;
//      drawHomeScreen();
    }
  }
  
  if(!cooling)digitalWrite(AC, LOW);
  else            digitalWrite(AC, HIGH);
  if(!heating)digitalWrite(HEAT, LOW);
  else            digitalWrite(HEAT, HIGH);
}

/*
void drawHomeScreen(void){
  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
 
  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}
*/

void drawSpScreen(void){
  tft.setRotation(1);
  tft.setCursor(0, 45);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  //tft.println("\n\n ");
  tft.print("WeekDay1: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekDay2: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekDay3: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekDay4: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.println(" ");
  tft.print("WeekEnd1: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekEnd2: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekEnd3: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.print("WeekEnd4: XX:XX XX");
  tft.print((char)167);
  tft.print("F\n"); 
  tft.setRotation(0);

  if(!dayPoint1)      tft.fillCircle(190, 275, 6, ILI9341_RED);
  else                tft.fillCircle(190, 275, 6, ILI9341_GREEN);            

  if(!dayPoint2)      tft.fillCircle(174, 275, 6, ILI9341_RED);
  else                tft.fillCircle(174, 275, 6, ILI9341_GREEN);

  if(!dayPoint3)      tft.fillCircle(157, 275, 6, ILI9341_RED);
  else                tft.fillCircle(157, 275, 6, ILI9341_GREEN);

  if(!dayPoint4)      tft.fillCircle(140, 275, 6, ILI9341_RED);
  else                tft.fillCircle(140, 275, 6, ILI9341_GREEN);

  if(!endPoint1)      tft.fillCircle(108, 275, 6, ILI9341_RED);
  else                tft.fillCircle(108, 275, 6, ILI9341_GREEN);            

  if(!endPoint2)      tft.fillCircle(92, 275, 6, ILI9341_RED);
  else                tft.fillCircle(92, 275, 6, ILI9341_GREEN);

  if(!endPoint3)      tft.fillCircle(76, 275, 6, ILI9341_RED);
  else                tft.fillCircle(76, 275, 6, ILI9341_GREEN);

  if(!endPoint4)      tft.fillCircle(60, 275, 6, ILI9341_RED);
  else                tft.fillCircle(60, 275, 6, ILI9341_GREEN);

 
  
  //tft.fillCircle(80, 275, 7, ILI9341_GREEN);
  //tft.fillCircle(80, 310, 7, ILI9341_RED);
  //tft.fillCircle(80, 310, 7, ILI9341_GREEN);
  
}

void drawSetTemp(void){
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.setTextSize(2);
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.print(" Set Temp: ");
  
  // erase old temp
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.setTextSize(5);
  tft.print(" ");
  tft.print(oldSetTemp);
  tft.print("   ");
  
  // print new temp  
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println("        o");
  tft.setTextSize(5);
  tft.print(" ");
  tft.print(newSetTemp);
  tft.print(" F ");

  // print arrows
  //               apex   b l    b r
  tft.fillTriangle(200,75, 175,90, 225,90,ILI9341_WHITE);
  tft.fillTriangle(200,115, 175,100, 225,100,ILI9341_WHITE);
}

void drawClock(){
  now = rtc.now();
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.setTextSize(2);
  
  // clear old date/time
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(0, 5);
  tft.print("                 ");
  if(last.dayOfTheWeek() != now.dayOfTheWeek()) {
    tft.print(daysOfTheWeek[last.dayOfTheWeek()]);
  }
  else {
    tft.print("   ");
  }
  tft.print(" ");
  if(last.month() != now.month()){
    if(last.month()<10){
      tft.print("0");
    }
    tft.print(last.month(), DEC);
  }
  else {
    tft.print("  ");
  }
  tft.print(" ");
  if(last.day() != now.day()){
    if(now.day()<10){
      tft.print("0");
    }
    tft.print(now.day(), DEC);
  }
  else {
    tft.print("  ");
  }
  tft.print("\n                  ");
  if(last.hour() != now.hour()){
    if(last.hour()<10){
      tft.print("0");
    }
    tft.print(last.hour(), DEC);
  }
  else {
    tft.print("  ");
  }
  tft.print(" ");
  if(last.minute() != now.minute()){
    if(last.minute()<10){
      tft.print("0");
    }
    tft.print(last.minute(), DEC);
  }
  else {
    tft.print("  ");
  }
  tft.print(" ");
  if(lastTenSec != (int)now.second()/10){  
    tft.print(lastTenSec);
  }
  else {
    tft.print(" ");
  }
  if(lastOneSec != (now.second() - (int)(now.second()/10)*10)){
    tft.print(lastOneSec);
  }
  else {
    tft.print(" ");
  }
  
  // print new date/time
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(0, 5);
  tft.setTextSize(2);
  tft.print("                 ");
  tft.print(daysOfTheWeek[now.dayOfTheWeek()]);
  tft.print(" ");
  if(now.month()<10){
    tft.print("0");
  }
  tft.print(now.month(), DEC);
  tft.print("/");
  if(now.day()<10){
    tft.print("0");
  }
  tft.print(now.day(), DEC);

  tft.print("\n                  ");
  if(now.hour()<10){
    tft.print("0");
  }
  tft.print(now.hour(), DEC);
  tft.print(":");
    if(now.minute()<10){
    tft.print("0");
  }
  tft.print(now.minute(), DEC);
  tft.print(":");
  if(now.second()<10){
    tft.print("0");
  }
  tft.print(now.second(), DEC);

  last = now;
  lastTenSec = (int)now.second()/10;
  lastOneSec = (now.second() - (int)(now.second()/10)*10);
}

void drawTemp(){
  tft.setRotation(1);
  
  // erase old temp
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println(" ");
  tft.println("         ");
  tft.setTextSize(5);
  tft.print(" ");
  tft.print(oldTemp);
  tft.print("   ");
  
  // print new temp  
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println("Current Temp:");
  tft.println(" ");
  tft.println(" ");
  tft.println("        o");
  tft.setTextSize(5);
  tft.print(" ");
  tft.print(newTemp);
  tft.print(" F ");
}

void drawMenu(int current, int previous){
  tft.setRotation(0);
             //y0,  x0, h,  w
  tft.drawRect(0, 64*0, 40, 64, 0x5ACB);
  
  tft.drawRect(0, 64*1, 40, 64, 0x5ACB);
  
  tft.drawRect(0, 64*2, 40, 64, 0x5ACB);
  //drawHomeIcon();
  
  tft.drawRect(0, 64*3, 40, 64, 0x5ACB);
  
  tft.drawRect(0, 64*4, 40, 64, 0x5ACB);
  drawIcons();
  updateMenu(current, previous);
}

void updateMenu(int current, int previous){  
  tft.setRotation(0);
  tft.drawRect(0, 64*previous, 40, 64, 0x5ACB);
  tft.drawRect(0, 64*current, 40, 64, ILI9341_WHITE);
}

void drawIcons(void){
  drawHomeIcon();
  drawSpIcon();
  drawSetTempIcon();
}

void drawHomeIcon(void){
  tft.fillRect(20,145,13,8,0x5145);
  tft.fillTriangle(20,140,35,160,20,180,0x5145);
  tft.fillRect(5,150,15,20,0x5145);
  tft.fillRect(5,157,12,8,0x79E0);
}
void drawSpIcon(void){
  tft.fillCircle(20, 95, 15, 0x5145);
  tft.drawLine(20, 95, 20, 103, ILI9341_WHITE);
  tft.drawLine(20, 95, 33, 95, ILI9341_WHITE);
}

void drawSetTempIcon(void){
  tft.fillCircle(10, 223, 8, 0x5145);
  tft.fillCircle(30, 223, 4, 0x5145);
  tft.fillRect(15,219,15,9,0x5145);
}

void drawHomeScreen(void){
  // print current temp
  tft.setRotation(1);
  drawTemp();

  // print set temp
  tft.setTextColor(0xAD55);
  tft.setCursor(5, 180);
  tft.setTextSize(2);
  tft.print("Set Temp: ");
  tft.println(newSetTemp);
  
// Ben's
//  //Print heating setting
//  tft.setCursor(0, 5);
//  tft.setTextColor(ILI9341_RED);
//  tft.println("\n\n\n\n                 Heating: ");
//  
//  tft.setRotation(0);
//  if(!heating)  tft.fillCircle(163, 310, 7, ILI9341_RED);
//  else              tft.fillCircle(163, 310, 7, ILI9341_GREEN);
//  tft.setRotation(1);
//
//  // print cooling setting
//  tft.setTextColor(ILI9341_BLUE);
//  tft.println("");
//  tft.println("");
//  tft.println("                 Cooling: ");
//  
//  tft.setRotation(0);
//  if(!cooling)  tft.fillCircle(115, 310, 7, ILI9341_RED);
//  else              tft.fillCircle(115, 310, 7, ILI9341_GREEN);
//  tft.setRotation(1);
//
//  // print cooling setting
//  tft.setTextColor(ILI9341_MAGENTA);
//  tft.println("");
//  tft.println("");
//  tft.println("                 Program: ");
//
//  tft.setRotation(0);
//  if(!programmable)  tft.fillCircle(68, 310, 7, ILI9341_RED);
//  else                  tft.fillCircle(68, 310, 7, ILI9341_GREEN);
//  

// Elliot's

  tft.setTextWrap(0);
  tft.setCursor(0, 5);
  tft.setTextSize(2);
  tft.print("\n\n\n");
  
  //Print heating setting
  tft.setTextColor(ILI9341_RED);
  tft.println("                 Heating: ");
  
  tft.setRotation(0);
  if(!heating)  tft.fillCircle(180, 310, 7, ILI9341_RED);
  else              tft.fillCircle(180, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  // print cooling setting
  tft.setTextColor(ILI9341_BLUE);
  tft.println("");
  tft.println("                 Cooling: ");
  
  tft.setRotation(0);
  if(!cooling)  tft.fillCircle(145, 310, 7, ILI9341_RED);
  else              tft.fillCircle(145, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  // print cooling setting
  tft.setTextColor(ILI9341_MAGENTA);
  tft.println("");
  tft.println("                 Auto: ");

  tft.setRotation(0);
  if(!autoMode)  tft.fillCircle(110, 310, 7, ILI9341_RED);
  else                  tft.fillCircle(110, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  tft.setTextColor(ILI9341_CYAN);
  tft.println("");
  tft.println("                 Hold: ");

  tft.setRotation(0);
  if(!hold)     tft.fillCircle(80, 310, 7, ILI9341_RED);
  else              tft.fillCircle(80, 310, 7, ILI9341_GREEN);

  
  tft.setRotation(0);
}

/*
void clearScreen(){
  tft.fillRect(40,0,200,320,ILI9341_BLACK);
}
*/
// clears the WHOLE screen
void clearScreen(){
  tft.setRotation(1);
  tft.fillRect(0,0,320,240,ILI9341_BLACK);
}
