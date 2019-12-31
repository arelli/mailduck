/*  Started project with https://www.teachmemicro.com/simple-nodemcu-web-server/?fbclid=IwAR3t7f_soMh3nq68VfKGwujMyAe0doMfzpa6MfHRXri7Cc7vvrT4-FKqfBc
 *  for remote command execution ducky in combination with arduino leonardo. Its purpose is to create a WiFi Hotspot for you to connect, and then 
 *  it generates a server with an interactive website that allows you to type commands in a html form. The commands are directly "printed" in the 
 *  serial output(useful if you want to control any serial-capable device via WiFi).
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

 /*
// Replace with your network credentials
const char* ssid1 = "mywifi88";
const char* password1 = "password1";
*/

// Replace with your network credentials
String ssid1 = "mywifi88";
String password1 = "password1";


ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
int LEDPin = 13;
String messagetoprint = " ";
int command_counter = 0;  // counter of how many commands have been sent

String handleGenericArgs() { //Handler from https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
  String message = "";  
  for (int i = 0; i < server.args(); i++) {
    //message += "arg" + (String)i + ":";   //Include the current iteration value
    message = server.arg(i);              //Get the value of the parameter
  }
  return message;
} 


void setup(void){
  //the HTML of the web page
  page = "<h1>WifiDuck Remote Control</h1>";
  page += "Usage:";
  page += "</br><font size=\"1\">cmd(space)(text): Prints the text and then hits enter(return key)</br>";
  page += "wrt(space)(character): Writes a signle character</br>";
  page += "prs(space)(ASCII_or_Modifier_Key_Code): Presses the provided key";
  page += "rls(nothing): Releases all the pressed keys</br>";
  page += "dbg(anything): used for debugging.The ducky ignores them(available at leonardo usb serial).</font></br>";
  page += "</br><form>Command to Send:<br><input type=\"text\" name=\"command\"><br></form>";
  page += "Modifier Key Codes:</br>";  // TODO: add those in a seprate /help directory and add a key to acces it from "/"
  page += "<font size=\"1\">LEFT_CTRL->128,LEFT_SHIFT->129,LEFT_ALT->130,LEFT_GUI->131,RIGHT_CTRL->132</br>";
  page += "RIGHT_SHIFT->133,RIGHT_ALT->134,RIGHT_GUI->135,UP_ARROW->218,DOWN_ARROW->217</br>";
  page += "LEFT_ARROW->216,RIGHT_ARROW->215,BACKSPACE->178,TAB->179,RETURN->176,ESC->177</br>";
  page += "INSERT->209,DELETE->212,PAGE_UP->211,PAGE_DOWN->214,HOME->210,END->213</br>";
  page += "CAPS_LOCK->193,F1->194,F2->195,F3->196,F4->197,F5->198,F6->199,F7->200,F8->201</br>";
  page += "F9->202,F10->203,F11->204,F12->205</font></br>";
  //make the LED pin output and initially turned off
  delay(1000);
  Serial.begin(115200);
  WiFi.softAP(ssid1,password1); //begin WiFi connection https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-class.html
  WiFi.begin();
  Serial.println("");
 
  // Wait for connection
  while (WiFi.softAPgetStationNum() == 0) {
    delay(1000);
    Serial.print("dbg-no device connected to the AP.");
    //Serial.println(millis());
    //Serial.println(WiFi.status());
    //Serial.println(WiFi.softAPgetStationNum());  //using the SoftAP class
  }
  Serial.println("dbg-This is the debug output of NodeMCU. It is used to define which messages are to be ignored from the ducky.");
  String wifissidmsg = "dbg-Your software AP SSID is: " + ssid1;
  Serial.println(wifissidmsg);
  String wifipassmsg = "dbg-Your software AP password is: " + password1;
  Serial.println(wifipassmsg);
  //String ipadress = String(WiFi.softAPIP());
  //String ipadressmsg = "dbg-IP address: " + ipadress;
  //Serial.println(ipadressmsg);

  server.on("/", [](){
    messagetoprint = handleGenericArgs(); 
    if(server.args()>0){  //  if there are arguments(commands), add them in the html and increment the counter.
      page += "</br>command " + (String)command_counter +": " + messagetoprint;
      Serial.print(messagetoprint);
      server.send(200, "text/html", page);
      command_counter ++;
    }
    else{
      server.send(200, "text/html", page);
    }
  });
  server.begin();
  Serial.println("dbg-Web server started!");  // probably at 192.168.4.1
}

 
void loop(void){
  server.handleClient();
}
