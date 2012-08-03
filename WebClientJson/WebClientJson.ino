#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char api_server[] = "api.nedwave.com";   // name address for Nedwave API

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

boolean startRead = false;
String jsonString = "";

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac); 
  delay(1000);

  if (client.connect(api_server, 80)) {
    Serial.println("Client connected");
    // Make a HTTP request:
    client.println("GET /1.0/examples.json HTTP/1.1");
    client.println("Host: api.nedwave.com");
    client.println();
  } 
  else {
    Serial.println("Connection failed");
  }
}

void loop()
{
  if (client.available()) {
    char c = client.read();

    if( c == '{' ) { startRead = true; }
    if ( startRead ) { jsonString += c; }
  }
  
  if (!client.connected()) {
    client.stop();

    Serial.println("Client disconnected");
    Serial.println("Debug:");
    Serial.println(jsonString);
    
    parseJson();
    
    // Do nothing forevermore
    for(;;)
      ;
  }  
}

void parseJson() {
  // Parsing according to the json.org structures
  
  int c = 0; // Index counter while parsing
  int d = -1; // Dimension counter while parsing
  boolean keepLoop = true;
  boolean registerValue = false;
  jsonString.trim();
  
  while( c < jsonString.length() ) {
    // Loop
    if( jsonString.charAt(c) == '{' ) {
      c++; // Increase index counter by one
      d++; // Increase dimension by one
      Serial.print( "Object d:" );
      Serial.println( d );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.charAt(c) == '[' ) {
      c++; // Increase index counter by one
      d++; // Increase dimension by one
      Serial.print( "Array d:" );
      Serial.println( d );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.charAt(c) == ':' ) {
      c++; // Increase index counter by one
      //Serial.println( "Read value" );
      registerValue = true;
    }
    else if( jsonString.charAt(c) == '"' ) {
      c++; // Increase index counter by one
      int stringEnd = jsonString.indexOf('"', c );
      String parsedString = jsonString.substring( c, stringEnd );
      c = stringEnd + 1;
      
      if( registerValue ) {
        registerValue = false;
        Serial.print( "Value: " );
        Serial.println( parsedString );
      }
      else {
        Serial.print( "Name: " );
        Serial.println( parsedString );
      }
    }
    else if( jsonString.charAt(c) > 47 && jsonString.charAt(c) < 58 ) {
      Serial.print( "Number: " );
      Serial.println( jsonString.charAt(c) );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.indexOf( "true", c ) == c ) {
      Serial.println( "Boolean TRUE" );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.indexOf( "false", c ) == c ) {
      Serial.println( "Boolean FALSE" );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.indexOf( "null", c ) == c ) {
      Serial.println( "NULL" );
      
      if( registerValue ) { registerValue = false; }
    }
    else if( jsonString.charAt(c) == ',' ) {
      c++; // Increase index counter by one
      
      if( registerValue ) { registerValue = false; }
    }    
    else if( jsonString.charAt(c) == '}' ) {
      c++; // Increase index counter by one
      d--; // Decrease dimension by one
    }
    else if( jsonString.charAt(c) == ']' ) {
      c++; // Increase index counter by one
      d--; // Decrease dimension by one
    }
    
  }
  
  Serial.println( "Successfully parsed JSON data" );
}


