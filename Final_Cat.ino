/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>
#include <Servo.h> 

const char* ssid = "ASUS";
const char* password = "x4le3ce4";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

Servo myservo; 
int pos = 0;    //servo position  
const int buzzerPin = 13; //D03

char notes[] = "aaa"; // a space = a rest
int beats[] = {1,1,1};
int tempo = 150;
int frequency(char note) 
{ 
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 656};
  
  for (int i = 0; i < sizeof(frequencies)/sizeof(int); i++)
  {
    if (names[i] == note)
    {
      return(frequencies[i]);
    }
  }
  return(0);
}
void music(){ 

    int i, duration;
    for (i = 0; i < sizeof(beats)/sizeof(int); i++)
  {
    duration = beats[i] * tempo;  // length of note/rest in ms
    
    if (notes[i] == ' ')         
    {
      delay(duration);            
    }
    else
    {
      tone(buzzerPin, frequency(notes[i]), duration);
      delay(duration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }
  //noTone(buzzerPin);
  
}
void moveServo() { 
    Serial.println("moving servo"); 
    for (pos = 0; pos <= 180; pos += 1) {  
    myservo.write(pos);   
    delay(15); 
  } 
  for (pos = 180; pos >= 0; pos -= 1) {  
   myservo.write(pos);  
   delay(15); 
 } 
} 

void setup() {
    myservo.attach(0);  //D07
   pinMode(buzzerPin, OUTPUT);
 
  Serial.begin(115200);
  delay(10);
  

  // prepare GPIO2
  pinMode(4, OUTPUT);
  digitalWrite(2, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(15);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = 0;
  else if (req.indexOf("/gpio/1") != -1)
    {val = 1; 
    music();
    moveServo(); 
    moveServo(); 
   delay(15);
    } 
   else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO2 according to the request
  digitalWrite(2, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

