#include <Keypad.h>
#include <SPI.h>
#include <SD.h>
//Includes the Servo library
#include <Servo.h>.
#define Time 50 

// Defines Tirg and Echo pins of the Ultrasonic Sensor
const int trigPin = 22;
const int echoPin = 23;
// Variables for the duration and the distance
long duration;
int distance;
boolean startmonitoring = true;
boolean alarmStart = false;
Servo myServo; // Creates a servo object for controlling the servo motor
boolean setAlarm = false;
File myFile;
String password;
int current_password;
String tempPassword;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keypressed;
boolean active = false;
int screenOffMsg = 0;
boolean start = true;
int buzzer = 9;
//define the cymbols on the buttons of the keypads
char keyMap[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {34, 35, 36, 37}; 
byte colPins[COLS] = {38, 39, 40,  41};
Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS); 
int monitoring;


void setup() { 
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input 
  myServo.attach(8); // Defines on which pin is the servo motor attached
  //Serial.print("Initializing SD card...");

  if (!SD.begin(53)) {
    //Serial.println("initialization failed!");
    return;
  }
  //Serial.println("initialization done.");

  myFile = SD.open("password.txt");
  if (myFile) {// read from the file until there's nothing else in it:
       while(myFile.available()) 
       { // Something left to read
          char c = myFile.read(); // Get the next character
          if (isdigit(c)) { // It's a number 0-9
          current_password *= 10; // Make room by decimal shifting to the left
          current_password += (c - '0'); // Convert the character to a number and add it to the total
                          }
        }
      //Serial.println("Password_initialize");
      password = String(current_password);
    myFile.close();     // close the file:
        } 
        else {
    //Serial.println("error opening password file");         // if the file didn't open, print an error:
          }
        
 pinMode(13, OUTPUT);
  //Serial.println("Welcome to the world of Security");
  
}




void loop() {
        // for 4 digit passkey
        boolean stats = false;
        
        if(!alarmStart)
        {
            //Serial.println("Press-> A : For set alarm");
            //Serial.println("Press-> B : for change the password");
            while(1){
            keypressed = myKeypad.getKey();
            if (keypressed != NO_KEY)
                        break;
                  }
            if(keypressed == 'A'){
            tone(buzzer,1000);
            delay(Time);
            noTone(buzzer);
            setAlarm = true;
            }
            
            else if(keypressed == 'B')
            { tone(buzzer,1000);
          delay(Time);
            noTone(buzzer);
          //Serial.print("Enter the Current password : ");
            int k = 4;
            tempPassword = "";
            int getpass = 1;
             while(getpass)
              {
              keypressed = myKeypad.getKey();
              if(keypressed != NO_KEY)
              {tempPassword += keypressed;
              tone(buzzer, 1000);
              //Serial.print("*");
              delay(Time);
              noTone(buzzer);
              k--;}
              if(k==0)
              {
                getpass = 0;
                //Serial.println("");
              }
              }
            if(tempPassword != password)
              {
              //Serial.println("Sorry you enter wrong password");
              //Serial.println("Please try again later");
              screenOffMsg = 0;
              }
            else if(tempPassword == password)
              {
                changePassword();
              }
        active = true;
        }
            
        }


        if(setAlarm){
        alarmStart =true;
       // Serial.println("Alarm Set on count of 10: ");
        for(int i = 0; i<10; i += 1)
        {
          tone(buzzer,1000);
          delay(Time);
          noTone(buzzer);
          delay(1000);
         // Serial.print(i+1);
          //Serial.print(" ");
        }
        //Serial.println(" ");
        startmonitoring = true;
        setAlarm = false;
        }
        if(startmonitoring){
          int otherset = 1;
        for(int i=0;i<=180;i++){  
             myServo.write(i);
             delay(30);
             distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
             Serial.print(i);
             Serial.print(",");
             Serial.print(distance);
             Serial.print(".");
             if(distance<100)
             {int alarmoff= 1;
             while(alarmoff){
              tone(buzzer,1000);
              alarmoff = enterPasswordforalarm(alarmoff);
              }
              noTone(buzzer);
              alarmStart = false;
              startmonitoring = false;
              otherset = 0;
              break;  
             }
             
        }
        if(otherset){
        for(int i=180;i>0;i--){  
          myServo.write(i);
          delay(30);
          distance = calculateDistance();
          Serial.print(i); // Sends the current degree into the Serial Port
          Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
          Serial.print(distance); // Sends the distance value into the Serial Port
          Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
                  //Serial.println("");
          if(distance<100)
             {int alarmoff= 1;
             while(alarmoff){
              tone(buzzer,1000);
              alarmoff = enterPasswordforalarm(alarmoff);}
              noTone(buzzer);
              alarmStart = false;
              startmonitoring = false;
              
              break;
              
             }
             }
        }
        }
  
      /* if(active == true)
        {
          Serial.println("Security is maintain");
          active = false;
        }*/
        
      


        
       /* else if(!active){
          if(screenOffMsg == 0)
          {
            Serial.println("Press-> A : For go inside");
            Serial.println("Press-> B : for change the password");
            screenOffMsg = 1;
          }
          
        keypressed = myKeypad.getKey();
        
        
        if(keypressed == 'A'){
            tone(buzzer,1000);
            delay(Time);
            noTone(buzzer);
            enterPassword(stats);
            
        }


        
        else if(keypressed == 'B')
        { tone(buzzer,1000);
          delay(Time);
            noTone(buzzer);
          Serial.print("Enter the Current password : ");
            int k = 4;
            tempPassword = "";
            int getpass = 1;
             while(getpass)
              {
              keypressed = myKeypad.getKey();
              if(keypressed != NO_KEY)
              {tempPassword += keypressed;
              tone(buzzer, 1000);
              Serial.print("*");
              delay(Time);
              noTone(buzzer);
              k--;}
              if(k==0)
              {
                getpass = 0;
                Serial.println("");
              }
              }
            if(tempPassword != password)
              {
              Serial.println("Sorry you enter wrong password");
              Serial.println("Please try again later");
              screenOffMsg = 0;
              }
            else if(tempPassword == password)
              {
                changePassword();
              }
        active = true;
        }
        
}*/
}



int enterPasswordforalarm(int alarmoff)
{
  int k = 4;
            //Serial.print("Enter the password if you want to go inside: ");
            tempPassword = "";
            int getpass = 1;
            while(getpass){
             keypressed = myKeypad.getKey();
              if (keypressed != NO_KEY){
                        tempPassword += keypressed;
                        //Serial.print("*");
                        k--;
              }
              
              if(k == 0)
              {
                //Serial.println("");
                getpass = 0;
              }
            }
            if(tempPassword != password)
            {
              //Serial.println("Sorry you enter wrong password");
              //Serial.println("Please try again later");
              alarmoff = 1;  
              return alarmoff; 
            }
            else if(tempPassword == password)
            {
             // Serial.println("No Trouble any more");
              alarmoff = 0;
              return alarmoff;
            }
            
}







void enterPassword(boolean stats) {
            int k = 4;
           // Serial.print("Enter the password if you want to go inside: ");
            tempPassword = "";
            int getpass = 1;
            while(getpass){
             keypressed = myKeypad.getKey();
              if (keypressed != NO_KEY){
                        tempPassword += keypressed;
                        tone(buzzer, 1000);
                        //Serial.print("*");
                        delay(Time);
                        noTone(buzzer);
                        k--;
              }
              
              if(k == 0)
              {
                ///Serial.println("");
                getpass = 0;
              }
            }
       if(tempPassword != password)
            {
             // Serial.println("Sorry you enter wrong password");
             // Serial.println("Please try again later");  
              screenOffMsg = 0; 
            }
            else if(tempPassword == password)
            {
              //Serial.println("Welcome to the Arduino World");
              screenOffMsg = 0;
              active = true;
              stats = true;
               screenOffMsg = 0;
            }
           
}







void changePassword()
    {
            int k = 4;
            tempPassword = "";
            int getpass = 1;
            // Serial.print("Enter New 4-digit Password: ");
             while(getpass)
              {
              keypressed = myKeypad.getKey();
              if(keypressed != NO_KEY)
              {
              tempPassword += keypressed;
              tone(buzzer,1000);
              //Serial.print("*");
              delay(Time);
              noTone(buzzer);
              k--;}
              
              if(k == 0)
                getpass = 0;
              }
             String againPassword = "";
             Serial.println("");
             //Serial.print("Re-Entered password: ");
             k = 4;
             int getagainpass = 1;
             while(getagainpass)
              {
              keypressed = myKeypad.getKey();
              if(keypressed != NO_KEY)
              {
                againPassword += keypressed;
                tone(buzzer, 1000);
               // Serial.print("*");
                delay(Time);
                noTone(buzzer);
                k--;
                }
              if(k == 0)
                {//Serial.println("");
                getagainpass = 0;}
                
              }
             if(tempPassword != againPassword)
            {
              //#Serial.println("");
              //Serial.println("Sorry you enter wrong password");
              //Serial.println("Please try again");
              changePassword();  
            }
             else if(tempPassword == againPassword)
             {
                password = tempPassword;
              // delete previos password file
               // Serial.println("Removing password.txt...");
                SD.remove("password.txt");
                myFile = SD.open("password.txt", FILE_WRITE);
              if (myFile)
                  {
                  // Serial.print("Writing to the text file...");
                   myFile.println(password);
                   myFile.close(); // close the file:
                  // Serial.println("done closing.");
                  } else
                        {//Serial.println("error opening the text file!");
                         }
              //  Serial.println("Congratulation! Your password is updated");
                screenOffMsg = 0;
             }
}
    
            
int calculateDistance(){ 
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration / 29 / 2;
  return distance;
}



