#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <EEPROM.h>

#define DHTTYPE DHT11 

const char* ssid = "EggWifi";
const char* pass = "987654321";

IPAddress local_ip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

int lampAddr = 0;
int mistAddr = 1;
int autoAddr = 2;
int setmaxlampAddr = 3;
int setminlampAddr = 4;
int setmaxmistAddr = 5;
int setminmistAddr = 6;

String lStatus;
String mStatus;
String aStatus;
String slmaxStatus;
String slminStatus;
String smmaxStatus;
String smminStatus;

uint8_t DHTPin = 2;
uint8_t lampPin = 4;
uint8_t mistPin = 5;

bool lampStatus = false;
bool mistStatus = false;
bool autoStatus = false;

float Temperature;
float Humidity;
int tempmaxSet;
int tempminSet;
int hummaxSet;
int humminSet;

unsigned long previousMillis = 0;
const long interval = 30000;

const char* PARAM_INPUT_1 = "settemp";
const char* PARAM_INPUT_2 = "humset";

DHT dht(DHTPin, DHTTYPE); 
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(DHTPin, INPUT);
  pinMode(lampPin, OUTPUT);
  pinMode(mistPin, OUTPUT);
  dht.begin();

  WiFi.softAP(ssid, pass);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  read_EEPROM();

  server.on("/", handle_OnConnect);
  server.on("/readsensor", read_sensor);
  server.on("/lampon", handle_lampon);
  server.on("/lampoff", handle_lampoff);
  server.on("/miston", handle_miston);
  server.on("/mistoff", handle_mistoff);
  server.on("/autoon", handle_autoon);
  server.on("/autooff", handle_autooff);  
  server.onNotFound(handle_NotFound);

  delay(1000);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(lampStatus)
  {digitalWrite(lampPin, LOW);}
  else
  {digitalWrite(lampPin, HIGH);}
  
  if(mistStatus)
  {digitalWrite(mistPin, LOW);}
  else
  {digitalWrite(mistPin, HIGH);}

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    read_sensor();
  }
}

void read_EEPROM(){
  lStatus = String(EEPROM.read(lampAddr));
  mStatus = String(EEPROM.read(mistAddr));
  aStatus = String(EEPROM.read(autoAddr));
  slmaxStatus = String(EEPROM.read(setmaxlampAddr));
  slminStatus = String(EEPROM.read(setminlampAddr));
  smmaxStatus = String(EEPROM.read(setmaxmistAddr));
  smminStatus = String(EEPROM.read(setminmistAddr));

  Serial.println("LAMP EEPROM: " + lStatus + " | MIST EEPROM: " + mStatus + " | AUTO EEPROM: " + aStatus);
  Serial.println("SMAXLAMP EEPROM: " + slmaxStatus + " | SMINLAMP EEPROM: " + slminStatus + " | SMAXMIST EEPROM: " + smmaxStatus + " | SMINMIST EEPROM: " + smminStatus);
  lampStatus = toBool(lStatus);
  mistStatus = toBool(mStatus);
  autoStatus = toBool(aStatus);
  tempmaxSet = slmaxStatus.toInt();
  tempminSet = slminStatus.toInt();
  hummaxSet = smmaxStatus.toInt();
  humminSet = smminStatus.toInt();  
}

void save_EEPROM(int address, int values){
  EEPROM.write(address, values);
  EEPROM.commit();
}

bool toBool(String status){
  if(status == "0"){
    return false;
  } else if (status == "1"){
    return true;
  }
}

String on_off(bool status){
  if(status == true){
    return "ON";
  } else if(status == false){
    return "OFF";
  }
}

void check_response(){
  //Serial.println(server.args());
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    //Serial.println(server.argName(i));
    if (server.argName(i) == "setmaxtemp") {
      tempmaxSet = server.arg(i).toInt();
      save_EEPROM(setmaxlampAddr, tempmaxSet);
    }
    if (server.argName(i) == "setmintemp") {
      tempminSet = server.arg(i).toInt();
      save_EEPROM(setminlampAddr, tempminSet);
    }    
    if (server.argName(i) == "setmaxhum") {
      hummaxSet = server.arg(i).toInt();
      save_EEPROM(setmaxmistAddr, hummaxSet);
    }
    if (server.argName(i) == "setminhum") {
      humminSet = server.arg(i).toInt();
      save_EEPROM(setminmistAddr, humminSet);
    }    
  }
  Serial.println("Max Temp SP: " + String(tempmaxSet) + " | Min Temp SP: " + String(tempminSet) + " | Max Hum SP: " + String(hummaxSet) + " | Min Hum SP: " + String(humminSet));
}

void read_sensor(){
  delay(1000);
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  if(autoStatus) {
    update_status();
  }
  Serial.println("Temperature: " + String(Temperature) + "| Humidity: " + String(Humidity));
  server.send(200, "text/html", SendHTML(lampStatus, mistStatus, autoStatus, tempmaxSet, tempminSet, hummaxSet, humminSet, Temperature, Humidity));
  check_response();
}

void update_status(){
  if((Temperature > tempminSet) && (Temperature < tempmaxSet)){
    lampStatus = true;
    save_EEPROM(lampAddr, 1);
  } else {
    lampStatus = false;
    save_EEPROM(lampAddr, 0);
  }

  if((Humidity > humminSet) && (Humidity < hummaxSet)){
    mistStatus = true;
    save_EEPROM(mistAddr, 1);
  } else {
    mistStatus = false;
    save_EEPROM(mistAddr, 0);
  }
  Serial.println("LAMP Status: " + on_off(lampStatus) + " | MIST Status: " + on_off(mistStatus));
}

void handle_OnConnect() {
  read_EEPROM();
  Serial.println("LAMP Status: " + on_off(lampStatus) + " | MIST Status: " + on_off(mistStatus) + " | AUTO Status: " + on_off(autoStatus));
  read_sensor();
}

void handle_lampon() {
  lampStatus = true;
  save_EEPROM(lampAddr, 1);
  Serial.println("LAMP Status: ON");
  read_sensor();
}

void handle_lampoff() {
  lampStatus = false;
  save_EEPROM(lampAddr, 0);
  Serial.println("LAMP Status: OFF");
  read_sensor();
}

void handle_miston() {
  mistStatus = true;
  save_EEPROM(mistAddr, 1);
  Serial.println("MIST Status: ON");
  read_sensor();
}

void handle_mistoff() {
  mistStatus = false;
  save_EEPROM(mistAddr, 0);
  Serial.println("MIST Status: OFF");
  read_sensor();
}

void handle_autoon() {
  autoStatus = true;
  save_EEPROM(autoAddr, 1);
  Serial.println("AUTO Status: ON");
  read_sensor();
}

void handle_autooff() {
  autoStatus = false;
  save_EEPROM(autoAddr, 0);
  Serial.println("AUTO Status: OFF");
  read_sensor();
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t lampstat, uint8_t miststat, uint8_t autostat, int maxtempsp, int mintempsp, int maxhumsp, int minhumsp, float tempstat, float humstat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 10px auto 30px;} h3 {color: #444444;margin-bottom: 30px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +=".button-refresh {background-color: #27db16;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 1px;}\n";
  ptr +="pi {font-size: 24px;color: #444444;margin-bottom: 1px;}\n";
  ptr +="pa {font-size: 14px;color: #444444;margin-bottom: 1px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Sistem Penetas Telur</h1>\n";
  ptr +="<h3>by 2black0@gmail.com</h3>\n";
  
  ptr +="<pi>Suhu: ";
  ptr +=(int)tempstat;
  ptr +="&#8451</pi>\n";
  ptr +="<br>";
  ptr +="<pi>Kelembaban: ";
  ptr +=(int)humstat;
  ptr +="%</pi>\n";

  if(autostat){
    ptr +="<p>Otomatis Status: ON</p><a class=\"button button-off\" href=\"/autooff\">OFF</a>\n";
  }
  else{
    if(lampstat)
    {ptr +="<p>Lampu Status: ON</p><a class=\"button button-off\" href=\"/lampoff\">OFF</a>\n";}
    else
    {ptr +="<p>Lampu Status: OFF</p><a class=\"button button-on\" href=\"/lampon\">ON</a>\n";}

    if(miststat)
    {ptr +="<p>Mist Maker Status: ON</p><a class=\"button button-off\" href=\"/mistoff\">OFF</a>\n";}
    else
    {ptr +="<p>Mist Maker Status: OFF</p><a class=\"button button-on\" href=\"/miston\">ON</a>\n";}

    ptr +="<p>Otomatis Status: OFF</p><a class=\"button button-on\" href=\"/autoon\">ON</a>\n";
  }

  ptr +="<p>Refresh</p><a class=\"button button-refresh\" href=\"/\">Refresh</a>\n";

  if(!autostat){
    ptr +="<br>";
    ptr +="<pa>Temp Max SP:";
    ptr +=(int)maxtempsp;
    ptr +="&#8451</pa>\n";
    ptr +="<pa>Temp Min SP:";
    ptr +=(int)mintempsp;
    ptr +="&#8451</pa>\n";
    ptr +="<br>";

    ptr +="<pa>Humi Max SP:";
    ptr +=(int)maxhumsp;
    ptr +="%</pa>\n";
    ptr +="<pa>Humi Min SP:";
    ptr +=(int)minhumsp;
    ptr +="%</pa>\n";    
    ptr +="<br>";

    //String IPaddress = WiFi.localIP().toString();
    String IPaddress = "192.168.4.1";
    ptr += "<form action='http://"+IPaddress+"' method='POST'>\n";
    ptr += "Temp Max SP:<input type='number' name='setmaxtemp'><input type='submit' value='Enter'>\n";
    ptr += "</form>\n";
    ptr += "<form action='http://"+IPaddress+"' method='POST'>\n";
    ptr += "Temp Min SP:<input type='number' name='setmintemp'><input type='submit' value='Enter'>\n";
    ptr += "</form>\n";

    ptr += "<br>";

    ptr += "<form action='http://"+IPaddress+"' method='POST'>\n";
    ptr += "Humi Max SP:<input type='number' name='setmaxhum'><input type='submit' value='Enter'>\n";
    ptr += "</form>\n";
    ptr += "<form action='http://"+IPaddress+"' method='POST'>\n";
    ptr += "Humi Min SP:<input type='number' name='setminhum'><input type='submit' value='Enter'>\n";
    ptr += "</form>\n";
  }

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}