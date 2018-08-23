#include <WEMOS_SHT3X.h> // This library in my GitHub
#include <ESP8266WiFi.h> // This library installed as part of ESP8266 board (see video)
const char* ssid = "YOUR SSID";
const char* password = "YOUR SSID PASSWORD";

// Create the object to hold all the server information
WiFiServer server(80);

// Create the object for the temperature module
SHT3X sht30(0x45);

// ==================================================
// SETUP    SETUP    SETUP    SETUP    SETUP    SETUP
// ==================================================
void setup() {
  // Start the Serial Monitor (debug window) comms
  Serial.begin(9600);
  delay(10);

  // Connect to WiFi network
  Serial.println();  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address on debug window
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

// ==================================================
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// ==================================================
void loop() {

  // Get the temperature etc data now (takes a while)
  sht30.get();
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("Browser connected");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request, we're not interested in content
  String request = client.readStringUntil('\r');
  client.flush();

  // Return the response as HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>WeMos D1 Mini DHT30</title>");
  client.println("<style type=text/css>");
  client.println(".status {font-weight:bold;color:red;font-size:36pt;}");
  client.println("</style>");
  client.println("</head>");
  client.print("<div class=status>Temperature: ");

  // Get the temperature from the object (will be quick as we retrieved
  // the values at the start of the loop)
  float tempC = sht30.cTemp;
  client.println(tempC);

  // Ditto for humidity
  client.print("</div><div class=status>Humidity: ");
  float humid = sht30.humidity;
  client.println(humid);

  // Finish off the HTML, closing necessary blocks etc
  client.println("</div><br><br>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

  // Wait for 1 second before getting the next temperature
  // This is clunky code and requires refactoring but OK for demo
  delay(1000);
}
