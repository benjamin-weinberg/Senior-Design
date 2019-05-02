#include <RTClib.h>  // RTC library
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

RTC_DS3231 rtc;

int currentPage = 2;
int previousPage = 0;
int setTemp = 30;
int heating = 0;
int cooling = 0;
int autoMode = 0;
int hold = 0;  
int dayPoint1, dayPoint2, dayPoint3, dayPoint4 = 0; 
TS_Point p2;

void setup(void) {
  
  
  pinMode(HEAT, OUTPUT);
  pinMode(AC, OUTPUT);

  
  while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  
  tft.begin();

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    //Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

 // Serial.println("Capacitive touchscreen started");

  tft.setRotation(0);
  tft.fillScreen(ILI9341_BLACK);
  drawMenu(currentPage);
  updateMenu(currentPage, previousPage);
  drawHomeScreen();
  
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
}

void loop() {
  // Get time
  DateTime now = rtc.now();
  
  // Display time in serial monitor  
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
    
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
      if(p.x<64){
        previousPage = currentPage;
        currentPage = 0;
        updateMenu(currentPage, previousPage);
        clearScreen();
        
      }
      else if(p.x<128){
        previousPage = currentPage;
        currentPage = 1;
        updateMenu(currentPage, previousPage);
        clearScreen();
        drawSpScreen();
      }
      else if(p.x<192){
        previousPage = currentPage;
        currentPage = 2;
        updateMenu(currentPage, previousPage);
        clearScreen(); 
        drawHomeScreen();
      }
      else if(p.x<256){
        previousPage = currentPage;
        currentPage = 3;
        updateMenu(currentPage, previousPage);
        clearScreen();
        drawSetTemp();
      }
      else if(p.x<320){
        previousPage = currentPage;
        currentPage = 4;
        updateMenu(currentPage, previousPage);
        clearScreen();
      }
    }
    //listening for touches near HVAC settings
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
    else{
      /*
       *  heating = !heating;
      cooling = !cooling;
      autoMode = !autoMode; 
      drawHomeScreen();
       */
     
    }
  }

  if(cooling == 0)digitalWrite(AC, LOW);
  else            digitalWrite(AC, HIGH);
  if(heating == 0)digitalWrite(HEAT, LOW);
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

void drawMenu(int current){
             //y0,  x0, h,  w
  tft.drawRect(0, 64*0, 40, 64, 0x5ACB);
  
  tft.drawRect(0, 64*1, 40, 64, 0x5ACB);
  drawSpIcon();
  
  tft.drawRect(0, 64*2, 40, 64, 0x5ACB);
  drawHomeIcon();
  
  tft.drawRect(0, 64*3, 40, 64, 0x5ACB);
  
  tft.drawRect(0, 64*4, 40, 64, 0x5ACB);
}

void updateMenu(int current, int previous){  
  tft.drawRect(0, 64*previous, 40, 64, 0x5ACB);
  tft.drawRect(0, 64*current, 40, 64, ILI9341_WHITE);
}

void drawHomeIcon(void){
  tft.fillRect(20,145,13,8,0x5145);
  tft.fillTriangle(20,140,35,160,20,180,0x5145);
  tft.fillRect(5,150,15,20,0x5145);
  tft.fillRect(5,157,12,8,0x79E0);
}
void drawSpIcon(void){
  /*
   * tft.setRotation(1);
  tft.setCursor(5,0); 
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println("SP");
   */
  //tft.fillRect(10, 120, 13, 8, 0x5145); 
  tft.setRotation(1);
  tft.setCursor(100,210);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println("SP");
  tft.setRotation(0);
}
void drawSpScreen(void){
  tft.setRotation(1);
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println("WD1:XX:XX Temp:XX E:"); 
  tft.println("WD2:XX:XX Temp:XX E:");
  tft.println("WD3:XX:XX Temp:XX E:");
  tft.println("WD4:XX:XX Temp:XX E:");
  tft.println(" ");
  tft.println("END1:XX:XX Temp:XX E:");
  tft.println("END2:XX:XX Temp:XX E:");
  tft.println("END3:XX:XX Temp:XX E:");
  tft.println("END4:XX:XX Temp:XX E:");
  tft.setRotation(0);

  if(dayPoint1 == 0)      tft.fillCircle(220, 275, 7, ILI9341_RED);
  else                    tft.fillCircle(220, 275, 7, ILI9341_GREEN);            

  if(dayPoint2 == 0)      tft.fillCircle(200, 275, 7, ILI9341_RED);
  else                    tft.fillCircle(200, 275, 7, ILI9341_GREEN);

  if(dayPoint3 == 0)      tft.fillCircle(180, 275, 7, ILI9341_RED);
  else                    tft.fillCircle(180, 275, 7, ILI9341_GREEN);

  if(dayPoint4 == 0)      tft.fillCircle(160, 275, 7, ILI9341_RED);
  else                    tft.fillCircle(160, 275, 7, ILI9341_GREEN);

 
  
  //tft.fillCircle(80, 275, 7, ILI9341_GREEN);

  
  //tft.fillCircle(80, 310, 7, ILI9341_RED);
  //tft.fillCircle(80, 310, 7, ILI9341_GREEN);
  
}

void drawSetTemp(void){
  tft.setRotation(1);
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println("Set Temp:" + setTemp);
  tft.setRotation(0);
}

void drawHomeScreen(void){
  // print current temp
  tft.setRotation(1);
  tft.setCursor(5, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.println(" ");
  tft.println("Current Temp:");
  tft.println(" ");
  tft.println(" ");
  tft.println("        o");
  tft.println(" XX F");

  // print set temp
  tft.setTextColor(0xAD55);
  tft.setCursor(5, 180);
  tft.setTextSize(2);
  tft.print("Set Temp: ");
  tft.println(setTemp);
  //tft.fillTriangle(180,180,130,135,150,135,ILI9341_WHITE);//
 
  // print home page settings
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextWrap(0);
  tft.setCursor(0, 5);
  tft.setTextSize(2);
  //TODO: DATE/TIME
  tft.println("               Date: ");
  tft.println("               Time: ");
  tft.println("");


  //Print heating setting
  tft.setTextColor(ILI9341_RED);
  tft.println("                 Heating: ");
  
  tft.setRotation(0);
  if(heating == 0)  tft.fillCircle(180, 310, 7, ILI9341_RED);
  else              tft.fillCircle(180, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  // print cooling setting
  tft.setTextColor(ILI9341_BLUE);
  tft.println("");
  tft.println("                 Cooling: ");
  
  tft.setRotation(0);
  if(cooling == 0)  tft.fillCircle(145, 310, 7, ILI9341_RED);
  else              tft.fillCircle(145, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  // print cooling setting
  tft.setTextColor(ILI9341_MAGENTA);
  tft.println("");
  tft.println("                 Auto: ");

  tft.setRotation(0);
  if(autoMode == 0)  tft.fillCircle(110, 310, 7, ILI9341_RED);
  else                  tft.fillCircle(110, 310, 7, ILI9341_GREEN);
  tft.setRotation(1);

  tft.setTextColor(ILI9341_CYAN);
  tft.println("");
  tft.println("                 Hold: ");

  tft.setRotation(0);
  if(hold == 0)     tft.fillCircle(80, 310, 7, ILI9341_RED);
  else              tft.fillCircle(80, 310, 7, ILI9341_GREEN);

  
  tft.setRotation(0);
}

void clearScreen(){
  tft.fillRect(40,0,200,320,ILI9341_BLACK);
}
