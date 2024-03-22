#include <WiFiClient.h>

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define MODEL "NODEMCU"
#define VERSION "0.4"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimeLib.h>
#include <Time.h>
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
ESP8266WebServer server(80);
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct settings {
  char ssid[30];
  char password[30];
  char email[30];
  char value[30];
  float A = 0;
  float B = 0;
  float C = 0;
  float D = 0;
} user_wifi = {};

#define trigPin D6
#define echoPin D7
long duration;
int distance;
Ticker count_time1;
Ticker count_time2;

const int interruptPin = D2;
const int modePin = D0;

const int bluePin = D1;
const int redPin = D4;
const int greenPin = D3;

volatile bool oldsos = true;
volatile bool inttimer = false;
volatile bool inttimer2 = false;

bool bluevalue = 0;
bool greenvalue = 1;
bool redvalue = 0;

int modeButton = 1;

int RUNMODE = 0;

int timezone = 7 * 3600;
int dst = 0;

String datetime = "";

byte mac[6];
String str_mac = "";

// Change INFLUX CONFIG
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "B4Svc-_51XYr8JR9ZxK3F0U5AnuJEkBlac57Y1CjTee4SFkkfk3A8-Vwz7--TJLexfdvaSq7pIHPY42mbSqi0w=="
#define INFLUXDB_ORG "1d9573dc3ebd632b"
#define INFLUXDB_BUCKET "IOS"


// Time zone info
#define TZ_INFO "UTC7"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("PM");  //ชื่อโปรเจค

void c_time() {
  inttimer = true;
}

void c2_time() {
  inttimer2 = true;
}


void handlePortal() {

  if (server.method() == HTTP_POST) {

    strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
    strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
    strncpy(user_wifi.email, server.arg("email").c_str(), sizeof(user_wifi.email));
    strncpy(user_wifi.value, server.arg("value").c_str(), sizeof(user_wifi.value));
    user_wifi.A = atof(server.arg("A").c_str());
    user_wifi.B = atof(server.arg("B").c_str());
    user_wifi.C = atof(server.arg("C").c_str());
    user_wifi.D = atof(server.arg("D").c_str());

    EEPROM.put(0, user_wifi);
    EEPROM.commit();

    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*,::after,::before{box-sizing:border-box;}body{margin:0;font-family:'Segoe UI',Roboto,'Helvetica Neue',Arial,'Noto Sans','Liberation Sans';font-size:1rem;font-weight:400;line-height:1.5;color:#212529;background-color:#f5f5f5;}.form-control{display:block;width:100%;height:calc(1.5em + .75rem + 2px);border:1px solid #ced4da;}button{border:1px solid transparent;color:#fff;background-color:#007bff;border-color:#007bff;padding:.5rem 1rem;font-size:1.25rem;line-height:1.5;border-radius:.3rem;width:100%}.form-signin{width:100%;max-width:400px;padding:15px;margin:auto;}h1,p{text-align: center}</style> </head> <body><main class='form-signin'> <h1>Wifi Setup</h1> <br/> <p>Your settings have been saved successfully!<br />Please change mode and restart the device.</p></main></body></html>");
  } else {
    server.send(200, "text/html", "<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Wifi Setup</title><style>*, :after, :before {box-sizing: border-box;}body {margin: 0;font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, 'Noto Sans', 'Liberation Sans';font-size: 1rem;font-weight: 400;line-height: 1.5;color: #212529;background-color: #f5f5f5;}.form-control {display: block;width: 100%;height: calc(1.5em + .75rem + 2px);border: 1px solid #ced4da;}button {cursor: pointer;border: 1px solid transparent;color: #fff;background-color: #007bff;border-color: #007bff;padding: .5rem 1rem;font-size: 1.25rem;line-height: 1.5;border-radius: .3rem;width: 100%;}.form-signin {width: 100%;max-width: 400px;padding: 15px;margin: auto;}h1 {text-align: center}</style></head><body><main class='form-signin'><form action='/' method='post'><h1 class=''>Wifi Setup (" + str_mac + ") </h1><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid'> </div><div class='form-floating'><br /><label>Password</label><input type='password' class='form-control' name='password'></div><br /><div class='form-floating'><label>Email</label><input type='text' class='form-control' name='email'> </div><div> <input type='radio' name='value' value='1'> <label for='value'>วัดปริมาตรลูกบาต <small style='font-weight: 300; color: #777;'>A:กว้าง B:ยาว C:สูง D:ระห่างที่ติดเซนเซอร์</small></label> <br> <input type='radio' name='value' value='2'> <label for='value'>วัดปริมาตรทรงกระบอก <small style='font-weight: 300; color: #777;'>A:รัศมี B:สูง C:ระห่างที่ติดเซนเซอร์</small></label> <br> <input type='radio' name='value' value='3'> <label for='value'>วัดระยะ <small style='font-weight: 300; color: #777;'>A:ความลึก B:ระห่างที่ติดเซนเซอร์</small> </label></div><div class='form-floating'><label>A<small style='font-weight: 300; color: #777;' > หน่วย เซนติเมตร</small></label><input type='text' class='form-control' name='A'> </div><div class='form-floating'><label>B<small style='font-weight: 300; color: #777;' > หน่วย เซนติเมตร</small></label><input type='text' class='form-control' name='B'> </div><div class='form-floating'><label>C<small style='font-weight: 300; color: #777;' > หน่วย เซนติเมตร</small></label><input type='text' class='form-control' name='C'> </div><div class='form-floating'><label>D<small style='font-weight: 300; color: #777;' > หน่วย เซนติเมตร</small></label><input type='text' class='form-control' name='D'> </div><br /><br /><button type='submit'>Save</button><p style='text-align:right'><a href='https://www.thesaban.org' style='color:#32C5FF'>thesaban.org</a></p></form></main></body></html>");
  }
}

String macToStr(const uint8_t* mac) {
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}
float SUM;
float Pi = 3.142;
float PRIMART = 0;
float SUM1 = 0;
float SUM2 = 0;
int valueT = 0;
void influx() {
  sensor.clearFields();
  sensor.addField("rssi", WiFi.RSSI());
  sensor.addField("VALUE", valueT);
  sensor.addField("PRIMART", PRIMART);
  sensor.addField("SUM", SUM1);
  sensor.addField("SUM2", SUM2);
  sensor.addField("A", user_wifi.A);
  sensor.addField("B", user_wifi.B);
  sensor.addField("C", user_wifi.C);
  sensor.addField("D", user_wifi.D);

  Serial.println(sensor.toLineProtocol());
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}


void influxRegister() {
  sensor.clearFields();
  sensor.addField("register", 1);
  sensor.addField("rssi", WiFi.RSSI());


  Serial.println(sensor.toLineProtocol());
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

String getDate() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char buffer[40];
  sprintf(buffer, "%u/%u/%u %u:%u:%u", p_tm->tm_mday, (p_tm->tm_mon + 1), (p_tm->tm_year + 1900), p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);
  return buffer;
}

void setup() {
  EEPROM.begin(sizeof(struct settings));
  EEPROM.get(0, user_wifi);
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  Serial.begin(9600);
  Serial.println("");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(modePin, INPUT_PULLUP);
  pinMode(interruptPin, INPUT_PULLUP);

  delay(250);
  Serial.print("Mode Read : ");
  Serial.println(digitalRead(modePin));


  pinMode(bluePin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  digitalWrite(bluePin, bluevalue);
  digitalWrite(redPin, redvalue);
  digitalWrite(greenPin, greenvalue);

  modeButton = digitalRead(modePin);

  if (!modeButton) {
    WiFi.macAddress(mac);
    str_mac += macToStr(mac);

    String APname = "CPEiot_" + str_mac;

    Serial.println("WIFI AP MODE");
    WiFi.softAP(APname, "mirot.digit");

    Serial.println("HTTP server started");
    Serial.print("A:");
    Serial.println(user_wifi.A);
    Serial.print("B:");
    Serial.println(user_wifi.B);
    Serial.print("C:");
    Serial.println(user_wifi.C);
    Serial.print("D:");
    Serial.println(user_wifi.D);

    if (user_wifi.value[0] == '1') {
      Serial.println("CUBE");
    } else if (user_wifi.value[0] == '2') {
      Serial.println("Cylindrical");
    } else if (user_wifi.value[0] == '3') {
      Serial.println("Distance");
    }

    server.on("/", handlePortal);
    server.begin();
    digitalWrite(bluePin, 1);
    digitalWrite(redPin, 1);
    digitalWrite(greenPin, 1);
    RUNMODE = 1;
  } else {
    WiFi.mode(WIFI_STA);
    //wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    wifiMulti.addAP(user_wifi.ssid, user_wifi.password);
    Serial.print(user_wifi.ssid);
    Serial.println(user_wifi.password);
    Serial.println("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      for (int i = 0; i <= 4; i++) {
        digitalWrite(bluePin, 0);
        delay(200);
        digitalWrite(bluePin, 1);
        delay(200);
      }
    }
    digitalWrite(bluePin, bluevalue);
    Serial.println();
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");

    if (client.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }

    WiFi.macAddress(mac);
    str_mac += macToStr(mac);
    Serial.println(str_mac);
    Serial.println(user_wifi.email);
    sensor.addTag("model", MODEL);
    sensor.addTag("version", VERSION);
    sensor.addTag("device", str_mac);
    sensor.addTag("email", user_wifi.email);


    influxRegister();

    count_time1.attach(2, c_time);  //Interrupt ทุกๆ 60 วินาที
    count_time2.attach(30, c2_time);
    display.clearDisplay();  //Interrupt ทุกๆ 2 วินาที
  }
}

void loop() {

  if (RUNMODE == 1) {
    server.handleClient();
  } else {
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
      for (int i = 0; i <= 4; i++) {
        digitalWrite(bluePin, 0);
        delay(200);
        digitalWrite(bluePin, 1);
        delay(200);
      }
    } else {
      digitalWrite(bluePin, 0);

      if (inttimer) {


        greenvalue = !greenvalue;
        digitalWrite(greenPin, greenvalue);
        inttimer = false;
      }

      if (inttimer2) {
        influx();
        inttimer2 = false;
      }
    }
  }


  digitalWrite(trigPin, LOW);
  delay(100);
  digitalWrite(trigPin, HIGH);
  delay(100);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(100);

  if (user_wifi.value[0] == '1') {
    valueT = 1;
    PRIMART = (user_wifi.A * user_wifi.B) * user_wifi.C;
    SUM = (user_wifi.A * user_wifi.B) * (distance - user_wifi.D);
    SUM1 = (SUM / PRIMART) * 100;
    SUM2 = 100 - SUM1;
  } else if (user_wifi.value[0] == '2') {
    valueT = 2;
    PRIMART = Pi * pow(user_wifi.A, 2) * user_wifi.B;
    SUM = Pi * pow(user_wifi.A, 2) * (distance - user_wifi.C);
    SUM1 = (SUM / PRIMART) * 100;
    SUM2 = 100 - SUM1;
  } else if (user_wifi.value[0] == '3') {
    valueT = 3;
    PRIMART = user_wifi.A;
    SUM = distance - user_wifi.B;
    SUM1 = (SUM / PRIMART) * 100;
    SUM2 = 100 - SUM1;
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 5);
  if (user_wifi.value[0] == '1') {
    display.print("Dist");
    display.print(" P");
    display.print(PRIMART);
    display.print("D:");
    display.print(user_wifi.D);
    display.print("\nA:");
    display.print(user_wifi.A);
    display.print(" C:");
    display.print(user_wifi.C);
    display.print("K");
    display.print(SUM2);
    display.print("\nB:");
    display.print(user_wifi.B);
    display.print("L ");
    display.print(SUM1);
  } else if (user_wifi.value[0] == '2') {
    display.print("Dist");
    display.print(" P");
    display.print(PRIMART);
    display.print("D:");
    display.print(user_wifi.D);
    display.print("\nA:");
    display.print(user_wifi.A);
    display.print(" C:");
    display.print(user_wifi.C);
    display.print("K");
    display.print(SUM2);
    display.print("\nB:");
    display.print(user_wifi.B);
    display.print("L ");
    display.print(SUM1);
  } else if (user_wifi.value[0] == '3') {
    display.print("Dist");
    display.print(" P");
    display.print(PRIMART);
    display.print("D:");
    display.print(user_wifi.D);
    display.print("\nA:");
    display.print(user_wifi.A);
    display.print(" C:");
    display.print(user_wifi.C);
    display.print("K");
    display.print(SUM2);
    display.print("\nB:");
    display.print(user_wifi.B);
    display.print("L ");
    display.print(SUM1);
  }

  display.display();
  display.clearDisplay();

  Serial.println(PRIMART);
  Serial.println(SUM);
  Serial.println(SUM1);
  Serial.println(SUM2);
  delay(3000);
}