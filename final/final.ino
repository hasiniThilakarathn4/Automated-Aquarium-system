/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
//#define relay D4
#define interval 1000
// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define FIREBASE_HOST "aqua-a2ee5.firebaseio.com"  // the project name address from firebase id
#define FIREBASE_AUTH "clrYeJQdySyJ0IPrscaKvwlov48wBapoTgPCL9JZ"  //Uejx9ROxxxxxxxxxxxxxxxxxxxxxxxxfQDDkhN"  // the secret key generated from firebase



const char* ssid = "Thara";
const char* password = "*********";

// Data wire is plugged into pin D2 on the ESP8266 - GPIO 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass the oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];
char temperatureFString[7];


WiFiServer server(80);

int relay = 2,relay1 = 13,relay2 = 15; //D4,D7,D8;

int timezone = 5.5*3600;
int dst = 0;


// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);


// only runs once on boot
void setup() {
  
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(1000);

  
  pinMode(relay, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  DS18B20.begin(); 
  
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

 configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

   // initialize the LCD
    lcd.begin(14,12);  // sda=14, scl=12 D5 D6

    // Turn on the blacklight and print a message.
    lcd.backlight();

    lcd.setCursor(0,0);

    lcd.print("     Welcome!!!");
   // delay(1000);
    lcd.clear();
  
 
}
  float tempC;
  float tempF;
  String currenttime;

  float calibration = 21.34; //change this value to calibrate
  const int analogInPin = A0; 
  int sensorValue = 0; 
  unsigned long int avgValue; 
  float b;
  int buf[10],temp;


void getTemperature() {
  
  do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    tempF = DS18B20.getTempFByIndex(0);
    dtostrf(tempF, 3, 2, temperatureFString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}


void getTime(){
   time_t now = time(nullptr);
   currenttime = ctime(&now); 
}

float voltage;
int pH;
 
float get_pH()
{
  for(int i=0;i<10;i++) 
{ 
 buf[i]=analogRead(analogInPin);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buf[i]>buf[j])
 {
 temp=buf[i];
 buf[i]=buf[j];
 buf[j]=temp;
 }
 }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = -5.70 * pHVol + calibration+11;
 Serial.print("sensor = ");
 Serial.println(phValue);

 return phValue;
 
 delay(500);
}

int n = 0; 

void loop() { 
 
  getTemperature();

 //  get_pH();
   int volt;
   int pH;
   float tempCel = tempC;
   String str,str1,str2,str3;
  
      delay(1000);
      getTime();
 
     Serial.print(tempCel);
     Serial.println("ºC");
     delay(1000);
     Serial.println(currenttime);

      // print out the value you read:
    // Serial.println(pH);
     delay(1000);
  //  String pH1 = String(pH);

    pH = get_pH();
  
  /*  if(pH<7){
    digitalWrite(relay1, HIGH);
    delay(interval);
    digitalWrite(relay1, LOW);
    
    }else{
    digitalWrite(relay2, HIGH);
    delay(interval);
    digitalWrite(relay2, LOW);
    }
*/

     
    str1 = "temperature:" ;
    str2 = String(tempCel);
    str3 = "C";

    str = str1 + str2 + str3 ;

    lcd_row(0,0,str);

    str1 = "pH:" ;
    str2 = String("7");

    str = str1 + str2 ;

    lcd_row(0,1,str);

  //  lcd_row(0,0,currenttime);

         // set value 
    Firebase.setString("Current_data1", "temperature"); 
    Firebase.setString("Current_data2", "pH");
 
    Firebase.setFloat("Current_data1/Current_temperature", tempCel);
    Firebase.setFloat("Current_data2/Current_pH", 7.0);
   // handle error 
    if (Firebase.failed()) { 
     Serial.print("setting /Current_temperature failed:"); 
     Serial.println(Firebase.error());   
     return; 
    }

   Firebase.setString("Current_data1/Current_temperature/Current_time", currenttime);
   Firebase.setString("Current_data2/Current_pH/Current_time", currenttime);

   Firebase.pushFloat("temp_log/temperature", tempCel);
   Firebase.pushString("temp_log/temperature/time", currenttime);
   Firebase.pushString("pH_log/pH", "7");
   Firebase.pushString("pH_log/pH/time", currenttime);

  if(tempCel<25.0){
   digitalWrite(relay, HIGH);
   delay(interval);
  }else{
   digitalWrite(relay, LOW);
   delay(interval);
  }
  
  }
