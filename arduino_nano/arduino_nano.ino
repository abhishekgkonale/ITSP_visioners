


#include <FilterDerivative.h>
#include <FilterOnePole.h>
#include <Filters.h>
#include <FilterTwoPole.h>
#include <FloatDefine.h>
#include <RunningStatistics.h>
float filterFrequency = 2.5;
FilterOnePole lowpassFilter( LOWPASS, filterFrequency );   



volatile int seconds =0;
volatile int minutes=0;



volatile int BPM=0;
volatile int BPM_avg=0;
volatile float hrv=0;


volatile float eye_blink_duration=0;
volatile float eye_blink_duration_avg=0;




volatile int count_2=0;

int Sensor;
int Sensor_e;

volatile bool flag=true;
volatile bool flag_e=true;
volatile bool flag_r=true;



int u_threshold=0;
int l_threshold=0;
int u_threshold_e=0;
int l_threshold_e=0;



void setup() {
 
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  
  pinMode(7,OUTPUT);      //eye-blink
  digitalWrite(7,HIGH);
  
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
  
  pinMode(4,OUTPUT);  
  digitalWrite(4,LOW);
  
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);  //eye-blink
  calibrate();
  cli();//stop interrupts
  // initialize the serial communication:
  
    Serial.begin(38400);
  //clock 1 at 1hz
    TCCR1A = 0;
    TCCR1B = 1<<CS12 | 0<<CS11 | 0<<CS10;
    TCNT1=0x0bdb;   
    TIMSK1 |= (1 << OCIE1A);
    
    //clock 2 at 1000hz
    TCCR2A = 0;        // set entire TCCR1A register to 0
    TCCR2B = 1<<CS22 | 0<<CS21 | 0<<CS20;
    TCNT2=5;
    TIMSK2 |= (1 << OCIE2A);
    
    sei();//allow interrupts
}

void sendDataToUNO(char symbol, int data ){
    Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
    Serial.println(data);                // the data to send culminating in a carriage return
  }
  
void sendFloatToUNO(char symbol,float data){
  Serial.print(symbol);
  Serial.println(data);
}
  
void reset_(){
  flag_r=false;
  
}
  
void calibrate (){
  
  
  delay(5000);
  
  int max_=0;
  for(int i=0;i<200;i++)
  {  if(lowpassFilter.input(analogRead(A0))>max_)
   {max_=lowpassFilter.input(analogRead(A0));
   }
   delay(20);
  }
  u_threshold=max_-15;
  l_threshold=max_ -30;
  
  delay(2000);
  
  int max_e=0;
  for(int i=0;i<800;i++)
  {  if(analogRead(A1)>max_e)
   {max_e=analogRead(A1);
   }
   delay(10);
  }
  u_threshold_e=max_e-40;
  l_threshold_e=u_threshold_e-30;
  
}


ISR(TIMER1_COMPA_vect)
{   TCNT1=0x0bdb;  
    seconds +=1;
    
  
      if(seconds>60)
   {    seconds=0;
        minutes+=1;
       
        
        
        sendDataToUNO('B',BPM);
        BPM_avg+=BPM;
        
        sendFloatToUNO('C',eye_blink_duration_avg/count_2);
        
        
        eye_blink_duration_avg=0;
        count_2=0;
        
        BPM=0;
        flag=true;
        
        
        
   }
    
      
      
   
   if(minutes>4 && flag_r==true)
   {  minutes=0;
      sendDataToUNO('A',BPM_avg/5);
     
      
      BPM_avg=0;
     
   }
   
   if(minutes>4 && flag_r==false)
   {  flag_r=true;
      sendDataToUNO('S',BPM_avg/5);
      minutes=0;
      BPM_avg=0;
      
      
   }
   
   
   
      
      
}
ISR(TIMER2_COMPA_vect)
{  TCNT2=5;
  hrv+=1;
  if(flag_e==false)
  {
  eye_blink_duration+=1;
  }
  
  
  
 
}

void loop() {
  // send the value of analog input 0:
  
  
  Sensor=lowpassFilter.input(analogRead(A0));
  Sensor_e=analogRead(A1);
  
  
  if(flag==true && Sensor>u_threshold)
  {    BPM+=1;
       flag=false;
       sendFloatToUNO('Q',hrv/1000);
       hrv=0;
  }
  
  if(flag==false && Sensor<l_threshold)
  {    flag=true;
  }
  
  //
 
  if(flag_e==true && Sensor_e>u_threshold_e)
  {    
     flag_e=false;
     
     
     
     
     
  }
  
  if(flag_e==false && Sensor_e<l_threshold_e)
  {    
       flag_e=true;
       if(eye_blink_duration>50){
       
       eye_blink_duration_avg+=eye_blink_duration/1000;
       digitalWrite(13,HIGH);
       delay(100);
       digitalWrite(13,LOW);
       Serial.println(eye_blink_duration);
       
       count_2+=1;
       }
     
       eye_blink_duration=0;
  }
  
  
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = Serial.read(); 
    if(inChar=='R'){
      reset_();}
  }
}
