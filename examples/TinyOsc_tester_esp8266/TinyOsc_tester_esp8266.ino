//               _                      _
//              | |                    | |
//    _ __   ___| |___      _____  _ __| | __
//   | '_ \ / _ \ __\ \ /\ / / _ \| '__| |/ /
//   | | | |  __/ |_ \ V  V / (_) | |  |   <
//   |_| |_|\___|\__| \_/\_/ \___/|_|  |_|\_\
//
//
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
WiFiUDP udp;

// THE FOLLOWING FILE CONTAINS THE CREDETIALS TO CONNECT TO MY NETWORK.
// TO CONFIGURE FOR YOUR OWN NETWORK,
// 1) COMMENT OUT THE FOLLING LINE:
#include "HomeWiFiCredentials.h"
// 2) UNCOMMENT THE FOLLOWING LINES AND FILL OUT WITH THE PROPER INFORMATION:
// const char* WIFI_SSID = "YOUR_SSID";
// const char* WIFI_PASSWORD = "YOUR_PASSWORD";


IPAddress networkGateway(192, 168, 1, 31);
IPAddress networkSubnet(255, 255, 255, 1);

IPAddress localIp(192, 168, 1, 31);
unsigned int localPort = 8888;

IPAddress remoteIp(192, 168, 1, 111);
unsigned int remotePort = 7777;

IPAddress broadcastIp(255, 255, 255, 255);


//    _______ _              ____
//   |__   __(_)            / __ \          
//      | |   _ _ __  _   _| |  | |___  ___
//      | |  | | '_ \| | | | |  | / __|/ __|
//      | |  | | | | | |_| | |__| \__ \ (__
//      |_|  |_|_| |_|\__, |\____/|___/\___|
//                     __/ |
//                    |___/

#include <TinyOscSlip.h>
TinyOscSlip oscSlip(&Serial);

#include <TinyOscUdp.h>
TinyOscUdp oscUdp(&udp, remoteIp, remotePort);

//     _____ _
//    / ____| |
//   | |    | |__  _ __ ___  _ __   ___
//   | |    | '_ \| '__/ _ \| '_ \ / _ \ 
//   | |____| | | | | | (_) | | | | (_) |
//    \_____|_| |_|_|  \___/|_| |_|\___/
//
//

#include <Chrono.h>
Chrono messageOutputChrono;


//             _                __ __
//            | |              / / \ \ 
//    ___  ___| |_ _   _ _ __ | |   | |
//   / __|/ _ \ __| | | | '_ \| |   | |
//   \__ \  __/ |_| |_| | |_) | |   | |
//   |___/\___|\__|\__,_| .__/| |   | |
//                      | |    \_\ /_/
//
void setup() {

  // INITIATE SERIAL COMMUNICATION
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.config(localIp, networkGateway, networkSubnet);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  blinkBuiltinLedWhileWaitingForConnection();

  udp.begin(localPort);


}

//                      _               _  ____           __  __                                 __ __
//                     (_)             | |/ __ \         |  \/  |                               / / \ \ 
//    _ __ ___  ___ ___ ___   _____  __| | |  | |___  ___| \  / | ___  ___ ___  __ _  __ _  ___| |   | |
//   | '__/ _ \/ __/ _ \ \ \ / / _ \/ _` | |  | / __|/ __| |\/| |/ _ \/ __/ __|/ _` |/ _` |/ _ \ |   | |
//   | | |  __/ (_|  __/ |\ V /  __/ (_| | |__| \__ \ (__| |  | |  __/\__ \__ \ (_| | (_| |  __/ |   | |
//   |_|  \___|\___\___|_| \_/ \___|\__,_|\____/|___/\___|_|  |_|\___||___/___/\__,_|\__, |\___| |   | |
//                                                                                    __/ |     \_\ /_/
//                                                                                   |___/
// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE.
// IT IS CALLED
void receivedOscMessage( TinyOscMessage& message) {

  // WHEN THE "/test" MESSAGE THAT CONTAINS AN int, A float AND A string IS RECEIVED,
  // WE SEND AN "/echo" MESSAGE BACK WITH THE SAME ARGUMENTS BUT WITH THE NUMERICAL VALUES DOUBLED.

  if ( message.fullMatch("/test", "ifs") ) {
    int32_t firstArgument = message.getNextInt32();
    float secondArgument = message.getNextFloat();
    const char* thirdArgument = message.getNextString();

    firstArgument = firstArgument * 2;
    secondArgument = secondArgument * 2;

    oscUdp.sendMessage( "/echo",  "ifs",  firstArgument, secondArgument, thirdArgument );
    oscSlip.sendMessage( "/echo",  "ifs", firstArgument, secondArgument, thirdArgument );
  }

}


//    _                    __ __
//   | |                  / / \ \ 
//   | | ___   ___  _ __ | |   | |
//   | |/ _ \ / _ \| '_ \| |   | |
//   | | (_) | (_) | |_) | |   | |
//   |_|\___/ \___/| .__/| |   | |
//                 | |    \_\ /_/
//
void loop() {


  oscUdp.receiveMessages( receivedOscMessage );
  oscSlip.receiveMessages( receivedOscMessage );

  if ( messageOutputChrono.hasPassed(1000) ) {

    messageOutputChrono.restart();

    // STEP 2 : STREAM A OSC MESSAGE THROUGH SLIP
    oscUdp.sendMessage( "/ms",  "i",   millis() );
    oscSlip.sendMessage( "/ms",  "i",   millis() );



  }

}

void blinkBuiltinLedWhileWaitingForConnection() {

  boolean blinkerState = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    blinkerState != blinkerState;
    digitalWrite(LED_BUILTIN, blinkerState);
  }
  digitalWrite(LED_BUILTIN, LOW);

}
