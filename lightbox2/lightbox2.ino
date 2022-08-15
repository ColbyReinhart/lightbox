#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define RED_PIN 14
#define GREEN_PIN 0
#define BLUE_PIN 2

WiFiClient client;
int red = 255;
int green = 0;
int blue = 255;

void handle_req(char* message)
{
  const char* const req_type = strtok(message, " ");
  Serial.print(req_type);
  if (strcmp(message, "PUT") == 0)
  {
    red = atoi(strtok(NULL, " "));
    green = atoi(strtok(NULL, " "));
    blue = atoi(strtok(NULL, " "));

    Serial.printf("Setting to: %i %i %i\n", red, green, blue);
    analogWrite(RED_PIN, 255 - red);
    analogWrite(GREEN_PIN, 255 - green);
    analogWrite(BLUE_PIN, 255 - blue);
  }
  else if(strcmp(message, "GET") == 0)
  {
    char res[100];
    bzero(res, 100);
    sprintf(res, "%i %i %i\n", red, green, blue);
    client.print(res);
  }
}

void setup() {
  // Setup controller
  Serial.begin(115200);
  Serial.println("Setting up pins . . .");
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
  Serial.println("Success!");

  // Connect to wifi
  Serial.println("Connecting to wifi . . .");
  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  wm.autoConnect();
  Serial.println("Success!");

  // Set default color
  Serial.println("Setting the default colors . . .");
  analogWrite(RED_PIN, 255 - red);
  analogWrite(GREEN_PIN, 255 - green);
  analogWrite(BLUE_PIN, 255 - blue);
  Serial.println("Success!");
}

void loop() {
  // Connect to server
  if (!client.connect("colbyreinhart.com", 5000))
  {
    Serial.println("Connection to server failed. Trying again.");
    delay(5000);
    return;
  }

  // Introduce ourselves to the server
  Serial.println("Sending request.");
  if (client.connected())
  {
    client.print("CONNECT /lightbox HTTP/1.0\r\nUser-Agent: lightbox\r\n\r\n");
  }

  // Constantly receive input from server
  char buffer[100];
  int count = 0;
  Serial.println("Receiving response from server.");
  while (client.connected() || client.available())
  {
    if (client.available())
    {
      char ch = static_cast<char>(client.read());
      ++count;
      if (count > 99 || ch == '\n')
      {
        handle_req(buffer);
        bzero(buffer, 100);
        count = 0;
      }
      else
      {
        buffer[count - 1] = ch;
      }
    }  
  }

  // If we got here, then we must've lost connection
  Serial.println("Lost connection to server, trying again.");
  client.stop();
  delay(5000);
}
