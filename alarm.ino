#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "Soi13"
#define wifi_password ""

#define mqtt_server "10.0.0.4"
#define mqtt_user "mqtt_user"
#define mqtt_password ""

#define MUX_A 12 //D6 on ESP8266 board
#define MUX_B 13 //D7 on ESP8266 board
#define MUX_C 15 //D8 on ESP8266 board

#define analogPin A0

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
struct group_of_sensors grSensors1() {
  group_of_sensors sensors_group;
  changeMux(LOW, LOW, LOW); //Change multiplexer for reading flame sensor
  sensors_group.flame_sensor = analogRead(analogPin);
  delay(100);
  changeMux(LOW, LOW, HIGH); //Change multiplexer for reading smoke sensor
  sensors_group.smoke_sensor = analogRead(analogPin);
  return sensors_group;
}

// This function returns the structure which contain values of the 2nd group of the flame and smoke sensors
struct group_of_sensors grSensors2() {
  group_of_sensors sensors_group;
  changeMux(LOW, HIGH, LOW); //Change multiplexer for reading flame sensor
  sensors_group.flame_sensor = analogRead(analogPin);
  delay(100);
  changeMux(LOW, HIGH, HIGH); //Change multiplexer for reading smoke sensor
  sensors_group.smoke_sensor = analogRead(analogPin);
  return sensors_group;
}

// This function returns the structure which contain values of the 3rd group of the flame and smoke sensors
struct group_of_sensors grSensors3() {
  group_of_sensors sensors_group;
  changeMux(HIGH, LOW, LOW); //Change multiplexer for reading flame sensor
  sensors_group.flame_sensor = analogRead(analogPin);
  delay(100);
  changeMux(HIGH, LOW, HIGH); //Change multiplexer for reading smoke sensor
  sensors_group.smoke_sensor = analogRead(analogPin);
  return sensors_group;
}

// This function returns the structure which contain values of the 4th group of the flame and smoke sensors
struct group_of_sensors grSensors4() {
  group_of_sensors sensors_group;
  changeMux(HIGH, HIGH, LOW); //Change multiplexer for reading flame sensor
  sensors_group.flame_sensor = analogRead(analogPin);
  delay(100);
  changeMux(HIGH, HIGH, HIGH); //Change multiplexer for reading smoke sensor
  sensors_group.smoke_sensor = analogRead(analogPin);
  return sensors_group;
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  //Deifne output pins for Mux
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);
  pinMode(MUX_C, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  struct group_of_sensors sensors_values1 = grSensors1();
  struct group_of_sensors sensors_values2 = grSensors2();
  struct group_of_sensors sensors_values3 = grSensors3();
  struct group_of_sensors sensors_values4 = grSensors4();
  Serial.println(String(sensors_values1.flame_sensor).c_str());
  Serial.println(String(sensors_values2.flame_sensor).c_str());
  Serial.println(String(sensors_values3.flame_sensor).c_str());
  Serial.println(String(sensors_values4.flame_sensor).c_str());
  delay(1000);

}