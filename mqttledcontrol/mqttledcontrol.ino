#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "djaweb";
const char* password = "wooowyyy";

// Replace with your MQTT Broker IP address
const char* mqtt_server = "broker.emqx.io";

// Define the pin for the LED
const int ledPin = D3; // Change this to the actual pin connected to your LED

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("NodeMCUClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("OUTTopic", "hello world");
      //... and resubscribe
      client.subscribe("smart-led/status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Callback function for when a PUBLISH message is received
void onMessage(char* topic, uint8_t* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Control the LED based on the message received
  if (message == "1") {
    digitalWrite(ledPin, HIGH); // Turn the LED on
  } else if (message == "0") {
    digitalWrite(ledPin, LOW); // Turn the LED off
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Set the callback function for receiving messages
  client.setCallback(onMessage);

  // Initialize the LED pin as an output
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
