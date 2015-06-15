#include <Average.h>



#include <EEPROM.h>



#include <LiquidCrystal.h>

#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>



LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int BPM=0;
int BPM_avg=0;
int normal_BPM=EEPROM.read(0);
float BPM_threshold_l=normal_BPM-normal_BPM*0.08;
float BPM_threshold_u=100;
volatile int seconds=0;
volatile int minutes=0;
volatile int hours=0;
volatile bool flag_rest=false;
 bool flag_rest_l=false;
 bool flag_rest_u=false;
 bool flag_q=false;
 bool danger=false;
 
 
float coefficient_of_variation=0;
int i=0;

Average<float> ave(100);
 






float eye_blink_duration_avg=0;



void setup() {
   pinMode(13,OUTPUT);//bt
   digitalWrite(13,HIGH);
   pinMode(6,OUTPUT);//for data reset
   analogWrite(6,255);
  // put your setup code here, to run once:
  Serial.begin(38400);
   cli();
   //clock 1 at 1hz
    TCCR1A = 0;
    TCCR1B = 1<<CS12 | 0<<CS11 | 0<<CS10;
    TCNT1=0x0bdb;    
    TIMSK1 |= (1 << OCIE1A);
    
    sei();
  
  
  lcd.begin(16, 2);
  lcd.print("VISUAL ALERT");
  
  
  
  lcd.setCursor(0,1);
  lcd.print("RESET BPM");
  
  delay(3000);
  lcd.setCursor(0,1);
  lcd.print("         ");
 
  pinMode(7,INPUT);//reset data
  pinMode(9,OUTPUT);//for buzzer
  pinMode(8,OUTPUT);//for buzzer
  digitalWrite(8,LOW);
  
  if(digitalRead(7)==HIGH){
    reset_();
  }
  
  lcd.setCursor(0,1);
  lcd.print("RELAX!!");
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("       ");
  lcd.setCursor(0,1);
  lcd.print("BPM :");

}
void reset_(){
  Serial.println('R');
}

void alarm(){
  analogWrite(9,50);
  delay(1000);
  analogWrite(9,0);
  
  
}


void loop(){
  if(flag_rest==true)
  {
    alarm();
    flag_rest=false;
  }
  
  if(flag_rest_l==true)
  {
    alarm();
    lcd.setCursor(8,1);
    lcd.print("BPM LOW!");
    delay(2000);
    lcd.setCursor(8,1);
    lcd.print("        ");
    
    flag_rest_l=false;
  }
  
  if(flag_rest_u==true)
  {
    alarm();
    lcd.setCursor(8,1);
    lcd.print("BPM^^!");
    delay(2000);
    lcd.setCursor(8,1);
    lcd.print("      ");
    
    flag_rest_u=false;
  }
   if(flag_q==true){
     coefficient_of_variation=100*(ave.stddev())/(ave.mean());
        Serial.print("c");
        Serial.println(coefficient_of_variation);
        flag_q=false;
   }
   
  
   
   if(danger=true){
     danger=false;
     alarm();
     
   }
}

ISR(TIMER1_COMPA_vect)
{   TCNT1=0x0bdb; 
    seconds+=1;
    if(seconds>60){
      seconds=0;
      minutes+=1;}
      
    if(minutes>60){
       hours+=1;
       minutes=0;}
       
    if (hours>5){
      bool flag_rest= true;
      hours=0;}
    
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read(); 
    
    
    
    if(inChar=='B'){
      BPM=Serial.parseInt();
      lcd.setCursor(5,1);
      lcd.print(BPM);
      
     
      
    }
    
    if(inChar=='C'){
      eye_blink_duration_avg=Serial.parseFloat();
      Serial.print("d");
      Serial.println(eye_blink_duration_avg);
      
      if(eye_blink_duration_avg*1000<400){
        
        lcd.setCursor(15,0);
        lcd.print('S');
        
         
      }
      
      if(eye_blink_duration_avg*1000>=400 && eye_blink_duration_avg*1000<800)
      { 
        lcd.setCursor(15,0);
        lcd.print('C');
       
      }
      
      if(eye_blink_duration_avg*1000>=800){
        danger=true;
        
        lcd.setCursor(15,0);
        lcd.print('D');
        
        
      }
        
      
    }
    
    
     
    
    
   
    
    
    if(inChar=='A'){
      BPM_avg=Serial.parseInt();
     //Serial.println(BPM_avg);
      if(BPM_avg<BPM_threshold_l){
        flag_rest_l=true;
        
      }
      if(BPM_avg>BPM_threshold_u){
        flag_rest_u=true;
      }
      
    }
    
    if(inChar=='S'){
      normal_BPM=Serial.parseInt();
      EEPROM.update(0,normal_BPM);
      
    }
    
    if(inChar=='Q'){
     
      if(i<100){
  ave.push(Serial.parseFloat());
  //Serial.println(i);
  i++;
      }
      else{
         flag_q=true;
        
        
        i=0;
      }
    }
      
  
  
 
    
  }
}

   
    
      
 
 
  



