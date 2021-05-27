#include <SoftwareSerial.h>

#define DEBUG true

String apiKey = "37WCLDF3K2KBG7FK"; // thingspeak write api key

SoftwareSerial ESP01(2, 3); // software serial set up (RX, TX)             

void setup() {
  pinMode(9, INPUT);
  pinMode(8, OUTPUT);
  
  pinMode(13, INPUT); // dip switch 1
  pinMode(12, INPUT); // dip switch 2
  pinMode(11, INPUT); // dip switch 3
  pinMode(10, INPUT); // dip switch 4
  
  Serial.begin(9600);
  Serial.println("Starting...");
  ESP01.begin(9600);
  
  sendData("AT+RST\r\n",1000,DEBUG);
  sendData("AT+CWMODE=1\r\n",1000,DEBUG);
  //sendData("AT+CWJAP=\"hotspot\",\"12345678\"\r\n",4000,DEBUG);  
  sendData("AT+CWJAP=\"wyper180910\",\"6664681234\"\r\n",4000,DEBUG);  
   
}

void loop() {
  sendData("AT+CIPMUX=0\r\n",2000,DEBUG);                                                                                                                
  Serial.println();
  
  String dipID = ""; // empty string to store trap ID
  
  if (digitalRead(13) == HIGH){ // if dip switch 1 is turned on
    dipID += "1"; // set first digit of trap ID to 1
  } else {
    dipID += "0"; // if not set first digit to 0
  }
  
  if (digitalRead(12) == HIGH){ // if dip switch 2 is turned on
    dipID += "2"; // set second digit of trap ID to 2
  } else {
    dipID += "0"; // if not set second digit to 0
  }
  
  if (digitalRead(11) == HIGH){ // if dip switch 3 is turned on
    dipID += "3"; // set third digit of trap ID to 3
  } else {
    dipID += "0"; // if not set third digit to 0
  }
  
  if (digitalRead(10) == HIGH){ // if dip switch 4 is turned on
    dipID += "4"; // set fourth digit of trap ID to 4
  } else {
    dipID += "0"; // if not set fourth digit to 0
  }
  
  Serial.print("Trap ID: " + dipID); 
  Serial.println();
  
  int sensorValue; // pir sensor value

  if (digitalRead(9) == LOW){ // if pir sensor detects mouse (sensor is inverted, motion = LOW)
    Serial.println("Mouse detected!");
    Serial.println();
    digitalWrite(8, HIGH); // turn on LED
    sensorValue = digitalRead(9); // set sensorValue to 1
    delay(100);
  } else { // if pir doesn't detect mouse
    Serial.println("No mouse detected.");
    Serial.println();
    digitalWrite(8, LOW); // turn off LED
    sensorValue = digitalRead(9); //set sensorValue to 0
    delay(100);
  }

  String pirValue = String(sensorValue);
  delay(100);
  
    // Make TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // Thingspeak.com's IP address  
  cmd += "\",80\r\n"; // port number
  sendData(cmd,2000,DEBUG);

    // Prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += pirValue; // add value for field 1 (pir value)
  //getStr += "\r\n";
  
  getStr +="&field2=";          
  getStr += dipID; // add value for field 2 (trap id)
  getStr += "\r\n";
  
    // Send data length & GET string
  ESP01.print("AT+CIPSEND=");
  ESP01.println (getStr.length());
  Serial.print("AT+CIPSEND=");
  Serial.println (getStr.length());  
  delay(500);
  
  if( ESP01.find( ">" ) ){
   Serial.print(">");
   sendData(getStr,2000,DEBUG);
  }
  
  // Close connection, wait a while before repeating...
  sendData("AT+CIPCLOSE",16000,DEBUG); // thingspeak needs 15 sec delay between updates
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    ESP01.print(command);
    long int time = millis();

    while( (time+timeout) > millis())
    {
      while(ESP01.available())
      {
        // "Construct" response from ESP01 as follows 
         // - this is to be displayed on Serial Monitor. 
        char c = ESP01.read(); // read the next character.
        response+=c;
      }  
    }

    if(debug)
    {
      Serial.print(response);
    }
    
    return (response);
}
