#include <ESP8266WiFi.h> 
#include <ESP8266WiFiMulti.h>
#include <HTTPSRedirect.h>

ESP8266WiFiMulti wifiMulti;

const char* ssid[3] = {"UIT_Guest", "@ss", "Lu Khai Thong"}; 
const char* password[3] = {"1denmuoi1", "thomasthong", "0981671490"};

// The ID below comes from Google Sheets. 
// Towards the bottom of this page, it will explain how this can be obtained 

//https://script.google.com/macros/s/AKfycbzdZRUIw1TjiTetUmZ2kB2eLXS9bET2lglhn0a5jDU8Q6yJoQFB/exec
//linh kien : https://script.google.com/macros/s/AKfycbxtqyCsAUFHrjH2VsQDha1kleKTuj5730BcFCbzO7ZXvRBCkK4/exec
const char *GScriptId = "AKfycbzdZRUIw1TjiTetUmZ2kB2eLXS9bET2lglhn0a5jDU8Q6yJoQFB";

// Push data on this interval 
const int dataPostDelay = 60;     // 15 minutes = 15 * 60 * 1000

const char* host = "script.google.com"; 

const int httpsPort =     443; 
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data) 
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "F0 5C 74 77 3F 6B 25 D7 3B 66 4D 43 2F 7E BC 5B E9 28 86 AD";

// We will take analog input from A0 pin 
const int AnalogIn                 = A0;

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
    //client.setContentTypeHeader("application/json");
     
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

// This is the main method where data gets pushed to the Google sheet 
void postData(String tag, float value){ 
    if (!client.connected()){ 
            Serial.println("Connecting to client again…"); 
            client.connect(host, httpsPort); 
    } 
    String urlFinal = url + "tag=" + tag + "&value=" + String(value); 
    client.GET(urlFinal, host); 
}

// Continue pushing data at a given interval 
void loop() { 
     
    // Read analog value, in this case a soil moisture 
    int data = 1023 - analogRead(AnalogIn);

    // Post these information 
    postData("SoilMoisture", data); 
     
    delay (dataPostDelay); 
}
