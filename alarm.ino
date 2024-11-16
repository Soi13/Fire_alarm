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

// This function returns the analog value of fire sensor
int fireSensor1() {
  changeMux(LOW, LOW, LOW); //Change multiplexer
  int val = analogRead(analogPin); //Value of the sensor connected Option 0 pin of Mux
  return val; // Return analog value from fire sensor
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

  int fire_sensor1_value = fireSensor1();
  Serial.println(String(fire_sensor1_value).c_str());
  delay(1000);

}