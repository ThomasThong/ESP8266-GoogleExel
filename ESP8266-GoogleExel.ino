#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>
#include <HTTPSRedirect.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti wifiMulti;

const char* ssid[3] = {"UIT_Guest", "@ss", "Lu Khai Thong"}; 
const char* password[3] = {"1denmuoi1", "thomasthong", "0981671490"};

//https://script.google.com/macros/s/AKfycbzdZRUIw1TjiTetUmZ2kB2eLXS9bET2lglhn0a5jDU8Q6yJoQFB/exec
//linh kien : https://script.google.com/macros/s/AKfycbxtqyCsAUFHrjH2VsQDha1kleKTuj5730BcFCbzO7ZXvRBCkK4/exec
const char *GScriptId = "AKfycbxtqyCsAUFHrjH2VsQDha1kleKTuj5730BcFCbzO7ZXvRBCkK4";

// Push data on this interval 
const int dataPostDelay = 60*1000;     // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com"; 

const int httpsPort =     443; 
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data) 
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

//enum _component { Res = 1, Cap, Diode, Inductor, IC};
//enum _packet { Hole = 1, Smd, SmdC, Smd0603, Smd0805, Smd1206 };
//enum _icType { Power = 1, Amp, Logic, TransFet, MCU, Others};

struct ComponentQuerry{
  String Component;
  String Type;
  String Packet;
  String Value ;
  int Quantity;
};

void setup() { 
    Serial.begin(115200); 
    Serial.println("Connecting to wifi "); 
    //Serial.println(ssid[0]); 
    Serial.flush();

    WiFi.mode(WIFI_STA);

    wifiMulti.addAP(ssid[0], password[0]);
    wifiMulti.addAP(ssid[1], password[1]);
    wifiMulti.addAP(ssid[2], password[2]);

    while (wifiMulti.run() != WL_CONNECTED) { 
            //Serial.print("wifi status : ");
            //Serial.println(WiFi.status());
            delay(500); 
            Serial.print("."); 
    } 
    Serial.println(" IP address: "); 
    Serial.println(WiFi.localIP());

    client.setPrintResponseBody(true);
    client.setContentTypeHeader("application/json");
     
    Serial.print(String("Connecting to ")); 
    Serial.println(host);
    bool flag = false; 
    for (int i=0; i<5; i++){ 
            int retval = client.connect(host, httpsPort); 
            if (retval == 1) { 
                        flag = true; 
                        break; 
            } 
            else 
                    Serial.println("Connection failed. Retrying…"); 
    }

    // Connection Status, 1 = Connected, 0 is not. 
    Serial.println("Connection Status: " + String(client.connected())); 
    Serial.flush(); 
     
    if (!flag){ 
            Serial.print("Could not connect to server: "); 
            Serial.println(host); 
            Serial.println("Exiting…"); 
            Serial.flush(); 
            return; 
    }

    // Data will still be pushed even certification don’t match. 
    if (client.verify(fingerprint, host)) { 
            Serial.println("Certificate match."); 
    } else { 
            Serial.println("Certificate mis-match"); 
    } 
}

void InputBufferFlush()
{
    while (Serial.available() != 0) {
      Serial.read();
    }
}
void CreateJson (JsonObject& JsonQuerry,  ComponentQuerry Querry)
{
//  switch (Querry.Component)
//    {
//      case Res:
//        JsonQuerry["Component"] = "Res";
//        break;
//      case Cap:
//        JsonQuerry["Component"] = "Cap";
//        break;
//      case Diode:
//        JsonQuerry["Component"] = "Diode";
//        break;
//      case Inductor:
//        JsonQuerry["Component"] = "Inductor";
//        break;
//      case IC:
//        JsonQuerry["Component"] = "IC";
//        break;
//      default:
//        break;
//    }
//
//    switch (Querry.Type)
//    {
//      case Power:
//        JsonQuerry["Type"] = "Power";
//        break;
//      case Amp:
//        JsonQuerry["Type"] = "Amp";
//        break;
//      case Logic:
//        JsonQuerry["Type"] = "Logic";
//        break;
//      case TransFet:
//        JsonQuerry["Type"] = "TransFet";
//        break;
//      case MCU:
//        JsonQuerry["Type"] = "MCU";
//        break;
//      case Others:
//        JsonQuerry["Type"] = "Others";
//        break;
//      default:
//        break;
//    }
//
//    switch (Querry.Packet)
//    {
//      case Hole:
//        JsonQuerry["Packet"] = "Hole";
//        break;
//      case Smd:
//        JsonQuerry["Packet"] = "Smd";
//        break;
//      case SmdC:
//        JsonQuerry["Packet"] = "SmdC";
//        break;
//      case Smd0603:
//        JsonQuerry["Packet"] = "Smd0603";
//        break;
//      case Smd0805:
//        JsonQuerry["Packet"] = "Smd0805";
//        break;
//      case Smd1206:
//        JsonQuerry["Packet"] = "Smd1206";
//        break;
//      default:
//        break;
//    }
//
//    JsonQuerry["Value"] = Querry.Value ;
}

// This is the main method where data gets pushed to the Google sheet 
void GETData(ComponentQuerry Querry ){ 
    if (!client.connected()){ 
            Serial.println("Connecting to client again…"); 
            client.connect(host, httpsPort); 
    } 
    String Temp;
    StaticJsonBuffer<200> JsonBuffer;
    JsonObject& JsonQuerry = JsonBuffer.createObject();
    CreateJson (JsonQuerry,  Querry);
    
    JsonQuerry.prettyPrintTo(Serial);
    
    //String urlFinal = url + "tag=" + tag + "&value=" + String(value); 
    client.GET(url, host); 
}

// Continue pushing data at a given interval 
void loop() { 
    // Post these information 
    int Input ;
    bool IsIC = false, IsSmd = false ;
    ComponentQuerry Querry;

    //Chossing component
    Serial.println ("choose Component");
    Serial.println ("1: Res");
    Serial.println ("2: Cap");
    Serial.println ("3: Diode");
    Serial.println ("4: Inductor");
    Serial.println ("5: IC");

    while (Serial.available() == 0) {
      ESP.wdtFeed();
    }
    Input = Serial.read();
    InputBufferFlush();
    Serial.print(Input);
    Serial.print(Serial.available());
    switch (Input)
    {
      case '1':
        Querry.Component = "Res";
        break;
      case '2':
        Querry.Component = "Cap";
        break;
      case '3':
        Querry.Component = "Diode";
        break;
      case '4':
        Querry.Component = "Inductor";
        break;
      case '5':
        Querry.Component = "IC";
        IsIC = true;
        break;
      default:
        Serial.println("wrong input");
        return;
    }
    Serial.println();

    //Chossing ICtype
    if (IsIC) {
      Serial.println ("choose IC type: ");
      Serial.println ("1: Power");
      Serial.println ("2: Amp");
      Serial.println ("3: Logic");
      Serial.println ("4: TransFet");
      Serial.println ("5: MCU");
      Serial.println ("6: Others");
  
      while (Serial.available() == 0) {
        ESP.wdtFeed();
      }
      Input = Serial.read();
      InputBufferFlush();
      Serial.print(Input);
      Serial.print(Serial.available());
      switch (Input)
      {
        case '1':
          Querry.Type = "Power";
          break;
        case '2':
          Querry.Type = "Amp";
          break;
        case '3':
          Querry.Type = "Logic";
          break;
        case '4':
          Querry.Type = "TransFet";
          break;
        case '5':
          Querry.Type = "MCU";
          IsIC = true;
          break;
        case '6':
          Querry.Type = "Others";
          IsIC = true;
          break;
        default:
          Serial.println("wrong input");
          return;
      }
    }
    Serial.println();

    //Chossing packet 
    Serial.println ("choose packet");
    Serial.println ("1: Hole");
    Serial.println ("2: Smd");

    while (Serial.available() == 0) {
      ESP.wdtFeed();
    }
    Input = Serial.read();
    InputBufferFlush();
    Serial.print(Input);
    Serial.print(Serial.available());
    switch (Input)
    {
      case '1':
        Querry.Packet = Hole;
        break;
      case '2':
        if (! IsIC){
          Serial.println ("choose smd type: ");
          Serial.println ("1: SmdC");
          Serial.println ("2: Smd0603");
          Serial.println ("3: Smd0805");
          Serial.println ("4: Smd1206");

          while (Serial.available() == 0) {
            ESP.wdtFeed();
          }
          Input = Serial.read();
          InputBufferFlush();
          Serial.print(Input);
          Serial.print(Serial.available());

          switch (Input)
          {
            case '1':
              Querry.Packet = "SmdC";
              break;
            case '2':
              Querry.Packet = "Smd0603";
              break;
            case '3':
              Querry.Packet = "Smd0805";
              break;
            case '4':
              Querry.Packet = "Smd1206";
              break;
            default:
              Serial.println("wrong input");
              return;
          }
        }
        else {
          Querry.Packet = "Smd";
          
        }
        break;
      default:
        Serial.println("wrong input");
        return;
    }
    Serial.println();
    
    GETData(Querry); 
     
    //delay (dataPostDelay); 
}
