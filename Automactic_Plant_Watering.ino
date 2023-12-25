#include <SoftwareSerial.h>

SoftwareSerial HC05(2, 3); //RX, TX

#define PUMP_CONTROL 5
#define SOIL_MOISURE_INPUT A0
#define WATER_LEVEL A1
#define WATER_ALERT 8

int const delayUnit = 500; //ms
int timeCoef = 3;
int moisureLimit = 50;
int moisure ; //random variable 
int percent;
int wL;
int waterLevel;
char t;
String command;
bool stopWaterAlertFlag = false; // Biến flag để kiểm soát việc dừng cảnh báo nước
bool waterAlertSent = false; // Biến flag để kiểm soát việc gửi cảnh báo nước
void setup() {
  Serial.begin(9600);
  HC05.begin(9600);
  pinMode(PUMP_CONTROL, OUTPUT); 
  pinMode(SOIL_MOISURE_INPUT, INPUT); 
}

void loop(){ 
  moisure = analogRead(SOIL_MOISURE_INPUT);  
  percent = ((1023 - moisure)/1023.00)*100;
  Serial.print("S");
  Serial.print(percent);
  Serial.print("\n");
  delay(timeCoef*delayUnit);
  if(percent < moisureLimit){
    pumpDueToSchedule();
  }

  wL = analogRead(WATER_LEVEL);
  waterLevel = (wL/660.00)*100;
  Serial.print("L");
  Serial.print(waterLevel);
  Serial.print("\n");
  delay(timeCoef*delayUnit);
 if (waterLevel < 20) {
    startAlertWaterLevel();
    //feedbackWater_alert();
  } else {
    stopAlertWaterLevel();
    //stopWaterAlertFlag = false; // Đặt lại cờ để có thể sử dụng trong lần tiếp theo
  }
  
  if (HC05.available()) {

    t = HC05.read();
    if (t != '\n') {
      command = command + t;
    } else {
      handlingCommand(command);
      command = "";
    }

  }
}

void pumpDueToSchedule() {
  digitalWrite(PUMP_CONTROL, HIGH);
  Serial.print("P");
  Serial.print("\n");
  delay(timeCoef*delayUnit);
  digitalWrite(PUMP_CONTROL, LOW);
  Serial.print("F");
  Serial.print("\n");
  //delay(timeCoef*delayUnit);
  //delay(1000);
}

void handlingCommand(String c) {

  if (c[0] == 'M') {
    int m = convert(c);
    moisureLimit = m;
    feedbackMoisure();
    Serial.print("C");
    Serial.print("SetMoisureThreshold:");
    Serial.print(moisureLimit);
    Serial.print(" ");
  } else if (c[0] == 'T') {
    int time = convert(c);
    timeCoef = time;
    feedbackTime();
    Serial.print("C");
    Serial.print("SetTimeCoeff:");
    Serial.print(timeCoef*delayUnit);
    Serial.print(" ");
  } else if (c[0] == 'H') {
    feedbackMoisure_sensor();
  } else if (c[0] == 'W') {
    feedbackWater_sensor();
  } /*else if (c[0] == 'K') {
   stopWaterAlertFlag = true; // Đặt biến flag để dừng cảnh báo nước
  }*/
}

int convert(String s) {
  int result = 0;
  for (int i = 1;i < s.length();i++)
    result = result*10 + (s[i] - 48);
  return result;  
}

void feedbackMoisure() {
  HC05.print("Moisure Limit: ");
  HC05.print(moisureLimit);
  HC05.println("%");
  Serial.print("M");
  Serial.print(moisureLimit);
  Serial.print(" ");
}

void feedbackMoisure_sensor() {
  HC05.print("Moisure current: ");
  HC05.print(percent);
  HC05.println("%\n");
}

void feedbackWater_sensor() {
  HC05.print("Water level: ");
  HC05.print(waterLevel);
  HC05.println("%\n");
}

void feedbackWater_alert()
{
  if (!waterAlertSent)
  {
    HC05.print("Water level: LOW\n");
  }
}


void feedbackTime() {
  HC05.print("Time: ");
  HC05.print(timeCoef*delayUnit);
  HC05.println("ms\n");
  Serial.print("T");
  Serial.print(timeCoef*delayUnit);
  Serial.print(" ");
}
void startAlertWaterLevel() {
  digitalWrite(WATER_ALERT, HIGH);
}

void stopAlertWaterLevel() {
  digitalWrite(WATER_ALERT, LOW);
}
