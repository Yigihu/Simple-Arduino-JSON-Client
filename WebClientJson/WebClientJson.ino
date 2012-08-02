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
boolean isJsonArray = false;
String jsonString = "";
int currentIndex = 0;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  Ethernet.begin(mac); 

  delay(1000);

  // if you get a connection, report back via serial:
  if (client.connect(api_server, 80)) {
    Serial.println("Client connected");
    // Make a HTTP request:
    client.println("GET /1.0/examples.json HTTP/1.1");
    client.println("Host: api.nedwave.com");
    client.println();
  } 
  else {
    // kf you didn't get a connection to the server:
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
  
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    client.stop();

    //Serial.println();
    Serial.println("Client disconnected");
    Serial.println("Debug:");
    Serial.println(jsonString);
    
    jsonString.trim();
    int jsonStringLength = jsonString.length();
    
    // Set start index to start parsing
    currentIndex = jsonString.indexOf("{");
    
    while( currentIndex < jsonStringLength ) {    
      parseJson();
    }
    
    Serial.println( "Succesfully parsed JSON data" );
    // do nothing forevermore:
    for(;;)
      ;
  }  
}

void parseJson() {
  if( jsonString.charAt( currentIndex ) == '{' ) {
    Serial.println( "Parse JSON object" );
    currentIndex++;
    
    boolean loopObject = true;
    while (loopObject) {
      parseJsonObjectElement();
      if( jsonString.charAt( currentIndex ) == ',' ) {
        currentIndex++;
        loopObject = true;
      }
      else {
        loopObject = false;
      }
    }
  }
  else if( jsonString.charAt( currentIndex ) == '[' ) {
    Serial.println( "Parse JSON array" );
    currentIndex++;
  }
  else{
    currentIndex++;
  }
}

void parseJsonObjectElement() {
  // JSON object loop, for illustration see json.org
  getObjectElementName();
  if( jsonString.charAt( currentIndex ) == ':' ) { currentIndex++; }
  getObjectElementValue();
}

void getObjectElementName() {
  if( jsonString.charAt( currentIndex ) == '"' ) {
    currentIndex++;
    
    int stringStart = currentIndex;
    int stringEnd = jsonString.indexOf('"', currentIndex );
    currentIndex = stringEnd + 1;
    
    String elementName = jsonString.substring( stringStart, stringEnd );
    Serial.print( "Element name: ");
    Serial.println( elementName );
  }
  else {
    Serial.println( "Did not find element name string" );
  }
}

void getObjectElementValue() {      
  if( jsonString.charAt( currentIndex ) == '[' ) {
    // Do nothing
  }
  else if( jsonString.charAt( currentIndex ) == '{' ) {
    // Do nothing
  }
  else if( jsonString.charAt( currentIndex ) == '"' ) {
    currentIndex++;
    
    int stringStart = currentIndex;
    int stringEnd = jsonString.indexOf('"', currentIndex );
    currentIndex = stringEnd + 1;
    
    String value = jsonString.substring( stringStart, stringEnd );
    Serial.print( "Element value: " );
    Serial.println( value );
  }
  else if( jsonString.charAt( currentIndex ) > 47 && jsonString.charAt( currentIndex ) < 58 ) {
    Serial.println( "Parse JSON number" );
  }
  else if( jsonString.indexOf( "true", currentIndex ) == currentIndex ) {
    Serial.println( "Parse JSON boolean TRUE" );
  }
  else if( jsonString.indexOf( "false", currentIndex ) == currentIndex ) {
    Serial.println( "Parse JSON boolean FALSE" );
  }
  else if( jsonString.indexOf( "null", currentIndex ) == currentIndex ) {
    Serial.println( "Parse JSON NULL" );
  }
  else {
    Serial.println( "Error while parsing JSON" );
  }
}
  


