//============================================
// 6x24 = 134 splot is 1 day
//============================================
#include <M5EPD.h>
#define uS_TO_S_FACTOR 1000000
#define SMALL_FONT_SIZE 24
#define MIDIUM_FONT_SIZE 32
#define LARGE_FONT_SIZE 180
#define PLOT_MAX 72

char temStr[10];
float tem;
char humStr[10];
float hum;
byte Otem[PLOT_MAX+1], Ohum[PLOT_MAX+1];
const int pitch = 36;
  
rtc_time_t RTCtime;
rtc_date_t RTCDate;
File file;
M5EPD_Canvas canvas(&M5.EPD);

void readEEPROM(){
  int i;
  for(i=0; i < PLOT_MAX;i++){
    Wire.beginTransmission(0x50);
    Wire.write(i*2);             // read address
    Wire.endTransmission();
    Wire.requestFrom(0x50, 1);
    delay(10);
      if(Wire.available()){
    Otem[i] = Wire.read();
  }
    Wire.beginTransmission(0x50);
    Wire.write(i*2+1);
    Wire.endTransmission();
    Wire.requestFrom(0x50, 1);
    delay(10);
      if(Wire.available()){
    Ohum[i] = Wire.read();
  }
  }
  
}
void writeEEPROM(){
  int i;
  for(i=0; i < PLOT_MAX;i++){
    Wire.beginTransmission(0x50);
    Wire.write(i*2);              // write address
    Wire.write(Otem[i]);          // write data
    Wire.endTransmission();
    Wire.requestFrom(0x50, 1);
    delay(5);

    Wire.beginTransmission(0x50);
    Wire.write(i*2+1);
    Wire.write(Ohum[i]);
    Wire.endTransmission();
    Wire.requestFrom(0x50, 1);
    delay(5);
    }
}
void drawG(){
  int i;
  int humY, temY, temH, plotX;
  const int offsetY = 120;
  const int grH = 400;

  // tem is 0 to 40 / hum is 10 to 80 / 400 pixel
  Otem[PLOT_MAX] = (int)(tem*255.0/30.0);   // temp 0-40(0-255)
  Ohum[PLOT_MAX] = (int)(hum*255.0/90.0);
 for(i=0 ; i < PLOT_MAX ; i++){
  plotX = (int)((520.0 / (float)PLOT_MAX)*(float)i);
  Otem[i] = Otem[i+1];

  temH = (int)((float)Otem[i]*(grH/255.0));
  temY = grH - temH;
  
    canvas.fillRect(plotX, temY + offsetY,  6,temH, 10);

  }
  for(i=0 ; i < PLOT_MAX ; i++){
     plotX = (int)((520.0 / (float)PLOT_MAX)*(float)i);
     Ohum[i] = Ohum[i+1];
     humY = (int)(grH - (float)Ohum[i]*(grH/255.0));

     canvas.fillCircle(plotX +10, humY + offsetY, 8, 15);
  }
  
}
//===========================
// main disp time ant temp and log
// go sleep
//===========================
void setup() {

    M5.begin();
    M5.RTC.begin();
    SD.begin();       // for SD card file read/write

    M5.EPD.SetRotation(90);
//    M5.EPD.Clear(true);
    Wire.begin();
    readEEPROM();
     
      canvas.createCanvas(540, 960);
//      canvas.loadFont("/fonts/MonospaceTypewriter.ttf", SD); // Load font files from SD Card
      canvas.loadFont("/fonts/ipaexg.ttf", SD); // Load font files from SD Card
      canvas.createRender(MIDIUM_FONT_SIZE);
      canvas.createRender(LARGE_FONT_SIZE);

}

void loop() {
  char  buf[128];
  int locY=1;
      
      canvas.fillCanvas(0);
      canvas.setTextSize(MIDIUM_FONT_SIZE);
      canvas.setTextColor(15);
      M5.RTC.getTime(&RTCtime);
      M5.RTC.getDate(&RTCDate);
      sprintf(buf,"Date %04d/%02d/%02d %02d:%02d:%02d",
                        RTCDate.year,RTCDate.mon,RTCDate.day,
                        RTCtime.hour,RTCtime.min,RTCtime.sec);
      canvas.drawString(String(buf) , 10, locY++ * pitch);
      sprintf(buf,"BatteryVoltage %06d",M5.getBatteryVoltage());
      canvas.drawString(String(buf) , 10, locY++ * pitch);
      canvas.drawString("Temperature", 10, 540);
      canvas.drawString("Humidity", 10, 740);
      canvas.drawFastHLine(0, 580, 540, 15);
      canvas.drawFastHLine(0, 780, 540, 15);
 
      M5.SHT30.UpdateData();
      tem = M5.SHT30.GetTemperature();
      hum = M5.SHT30.GetRelHumidity();
      dtostrf(tem, 2, 2 , temStr);
      dtostrf(hum, 2, 2 , humStr);
      canvas.setTextColor(15);
      canvas.setTextSize(LARGE_FONT_SIZE);
      canvas.drawString(String(temStr), 10, 580);
      canvas.drawString(String(humStr), 10, 780);

drawG();

//       canvas.pushCanvas(0,0,UPDATE_MODE_DU);
       canvas.pushCanvas(0,0,UPDATE_MODE_GLR16);
        writeEEPROM();
       
  file =SD.open("/temp_log.txt", FILE_APPEND);  // not FILE_WRITE
  sprintf(buf,"%04d/%02d/%02d,%02d:%02d:%02d,%04d,%6.2f,%6.2f",RTCDate.year,RTCDate.mon,RTCDate.day,
                        RTCtime.hour,RTCtime.min,RTCtime.sec,
                        M5.getBatteryVoltage(),
                        tem, hum
                        );

  file.println(buf);
  file.close();

      // shutdown until RTC + 10min
    delay(1000); // need delay for EPD driver ??
    M5.shutdown(600);  // 10 min interval at battery operation
    sleep(60); // 1 min interval at power connected

//  int nextmin;
//  nextmin = ((RTCtime.min/10)+1)*10;
//  esp_sleep_enable_timer_wakeup(((nextmin-RTCtime.min)*60-RTCtime.sec-12) * uS_TO_S_FACTOR);
//esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
//  esp_deep_sleep_start();

//    esp_deep_sleep(600*1000*1000);
//      esp_deep_sleep(60*1000*1000);  // only power cable is connected

}
