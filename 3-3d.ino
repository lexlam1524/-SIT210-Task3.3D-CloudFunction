// Include the necessary libraries
#include <MQTT.h>

// Define the pins used for the ultrasonic sensor and LED light
#define TRIGGER_PIN D1
#define ECHO_PIN D2
#define LED_PIN D0

// Define the maximum distance in centimeters for an object to trigger the wave detection
#define MAX_DISTANCE 20

// Initialize variables for tracking if there is an object detected and the last time an object was detected
bool objectDetected = false;
unsigned long lastDetectionTime = 0;
void messageReceived(char* topic,byte*payload, unsigned int length);
// Initialize the MQTT client and define the MQTT broker settings
MQTT client("broker.emqx.io", 1883, messageReceived, true);
const char* topic = "SIT210/wave";


// Define a callback function for when a message is received on the subscribed topic
void messageReceived(char* topic,byte*payload, unsigned int length) {
  // Print the received message on the serial monitor
  
    String topicStr = String(topic);
  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }
  // Print message to serial monitor
  Serial.print("Received message: ");
  Serial.println(payloadStr);
  // Flash the LED 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}

void setup() {
  // Start the serial communication and initialize the LED and ultrasonic sensor pins
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Connect to the MQTT broker and subscribe to the "SIT210/wave" topic
  client.connect("particle_device");
  client.subscribe(topic);
}

void loop() {
  // Send a 10us pulse to the ultrasonic sensor to trigger a measurement
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  
  // Read the time it takes for the ultrasonic sensor to receive the echo
  int duration = pulseIn(ECHO_PIN, HIGH);
  
  // Convert the duration to distance in centimeters
  int distance = duration / 58;
  
  // If an object is detected within the maximum distance, publish a message to the "SIT210/wave" topic containing the user's name
  if (distance > 0 && distance < MAX_DISTANCE) {
    if (!objectDetected) {
      client.publish(topic, "my name");
      objectDetected = true;
    }
    lastDetectionTime = millis();
  }
  // If there is no object detected within a certain time frame, reset the object detection flag
  else if (millis() - lastDetectionTime > 1000) {
    objectDetected = false;
  }
  

  
  // Loop through the MQTT client to check for incoming messages on the subscribed topic
  client.loop();
}
