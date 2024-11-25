#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define wifi_ssid "Soi13"
#define wifi_password ""

#define mqtt_server "10.0.0.4"
#define mqtt_user "mqtt_user"
#define mqtt_password ""

#define gr1_flame_sensor "homeassistant/sensor/gr1_flame_sensor"
#define gr1_smoke_sensor "homeassistant/sensor/gr1_smoke_sensor"
#define gr2_flame_sensor "homeassistant/sensor/gr2_flame_sensor"
#define gr2_smoke_sensor "homeassistant/sensor/gr2_smoke_sensor"
#define gr3_flame_sensor "homeassistant/sensor/gr3_flame_sensor"
#define gr3_smoke_sensor "homeassistant/sensor/gr3_smoke_sensor"
#define gr4_flame_sensor "homeassistant/sensor/gr4_flame_sensor"
#define gr4_smoke_sensor "homeassistant/sensor/gr4_smoke_sensor"

#define MUX_A 12 //D6 on ESP8266 board
#define MUX_B 13 //D7 on ESP8266 board
#define MUX_C 15 //D8 on ESP8266 board

#define analogPin A0

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);

void changeMux(int c, int b, int a) {
  digitalWrite(MUX_A, a);
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_C, c);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

struct group_of_sensors {
  int flame_sensor;
  int smoke_sensor;
};

// This function returns the structure which contain values of the 1st group of the flame and smoke sensors
struct group_of_sensors grSensors(int a, int b, int c, int d, int e, int f) {
  group_of_sensors sensors_group;
  delay(100);
  changeMux(a, b, c); //Change multiplexer for reading flame sensor
  sensors_group.flame_sensor = analogRead(analogPin);
  delay(100);
  changeMux(d, e, f); //Change multiplexer for reading smoke sensor
  sensors_group.smoke_sensor = analogRead(analogPin);
  return sensors_group;
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_C, OUTPUT);

  reconnect();

  //Initializing display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  //Display greetings.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Hello!!!");
  display.display();
  delay(2000);
}

void loop() {
  //if (!client.connected()) {
  //  reconnect();
  //}
  client.loop();

  struct group_of_sensors sensors_values1 = grSensors(LOW, LOW, LOW, LOW, LOW, HIGH);
  struct group_of_sensors sensors_values2 = grSensors(LOW, HIGH, LOW, LOW, HIGH, HIGH);
  struct group_of_sensors sensors_values3 = grSensors(HIGH, LOW, LOW, HIGH, LOW, HIGH);
  struct group_of_sensors sensors_values4 = grSensors(HIGH, HIGH, LOW, HIGH, HIGH, HIGH);
  Serial.print("Flame sensor (Group 1): ");
  Serial.println(String(sensors_values1.flame_sensor).c_str());
  Serial.print("Smoke sensor (Group 1): ");
  Serial.println(String(sensors_values1.smoke_sensor).c_str());
  Serial.print("Flame sensor (Group 2): ");
  Serial.println(String(sensors_values2.flame_sensor).c_str());
  Serial.print("Smoke sensor (Group 2): ");
  Serial.println(String(sensors_values2.smoke_sensor).c_str());
  Serial.print("Flame sensor (Group 3): ");
  Serial.println(String(sensors_values3.flame_sensor).c_str());
  Serial.print("Smoke sensor (Group 3): ");
  Serial.println(String(sensors_values3.smoke_sensor).c_str());
  Serial.print("Flame sensor (Group 4): ");
  Serial.println(String(sensors_values4.flame_sensor).c_str());
  Serial.print("Smoke sensor (Group 4): ");
  Serial.println(String(sensors_values4.smoke_sensor).c_str());

  //Dsiplay data on the OLED screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Gr1-Flame sensor:");
  display.println(sensors_values1.flame_sensor);
  display.print("Gr1-Smoke sensor:");
  display.println(sensors_values1.smoke_sensor);
  display.print("Gr2-Flame sensor:");
  display.println(sensors_values2.flame_sensor);
  display.print("Gr2-Smoke sensor:");
  display.println(sensors_values2.smoke_sensor);
  display.print("Gr3-Flame sensor:");
  display.println(sensors_values3.flame_sensor);
  display.print("Gr3-Smoke sensor:");
  display.println(sensors_values3.smoke_sensor);
  display.print("Gr4-Flame sensor:");
  display.println(sensors_values4.flame_sensor);
  display.print("Gr4-Smoke sensor:");
  display.println(sensors_values4.smoke_sensor);
  display.display();
  delay(1000);

}