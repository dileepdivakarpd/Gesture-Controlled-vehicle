#define left_wheel_f 3               //left wheel forward pin
#define right_wheel_f 5              //right wheel forward pin
#define left_wheel_b 6               //left wheel backward pin
#define right_wheel_b 9              //right wheel backward pin
#define left_indic 4                 //left indication pin
#define right_indic 7                //right indication pin
#define break_indic 10                //break indication pin
boolean led=HIGH;
boolean countl=0,countr=0,countv=0;                     
int l_indic=0,r_indic=0;             //indication
int l_speed1=0,r_speed1=0,l_speed=0,r_speed=0;             //received from the transmitter
int l_speed_hist=0,r_speed_hist=0;   //speed history
int l_speed_curr=0,r_speed_curr=0;   //current speed
int max_speed=100;                   //maximum speed
int loop_count=0;                    //loop count
int led_count=0;                     //led count
int i,j,k=1;                             
char indication='f';                 //direction
int l_wheel,r_wheel;                 
int indicator_led;
int temp,time_sts;
int run_delay=10;                    //in microseconds
int indication_delay=250;            //in milliseconds
unsigned long int break_delay=0,indic_delay=0;

void setup() 
{
  pinMode(left_wheel_f, OUTPUT);
  pinMode(right_wheel_f, OUTPUT);
  pinMode(left_wheel_b, OUTPUT);
  pinMode(right_wheel_b, OUTPUT);
  pinMode(left_indic, OUTPUT);
  pinMode(right_indic, OUTPUT);
  //pinMode(rev_indic, OUTPUT);
  pinMode(break_indic,OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  time_sts=micros();
  if(Serial.available())
  {
    if(Serial.read()==255)
    {
      while(Serial.available()==0);
      l_speed1=Serial.read();//Serial.print(l_speed);Serial.print('\t');
      while(Serial.available()==0);
      r_speed1=Serial.read();//Serial.println(r_speed);
      
    }
  }

  speed_mag_func();
  if((l_indic==1) & (r_indic==1))
  {
    reverse_motion();
    speed2indication_func();
    indicator_func();
    //Serial.println("rev");
  }
  else
  {
    forward_motion();
    speed2indication_func();
    indicator_func();
    //Serial.println("for");
  }
  break_func();
  if((break_delay-millis())>2000)
  {
     digitalWrite(break_indic,LOW);
  }
  loop_count++;
  if(loop_count==1000)
  {
    loop_count=0;
    led_count++;
  }
  if(led_count==indication_delay)
  {
    led_count=0;
  }
  temp=micros();
  if((temp-time_sts)<run_delay)
  {
    delayMicroseconds(run_delay-(temp-time_sts));
  }
}

//***************************SPEED MAGNITUDE FROM ARRAY FUNCTION******************************

void speed_mag_func()
{
  l_indic=l_speed1/128;
  r_indic=r_speed1/128;
  l_speed=l_speed1%128;
  r_speed=r_speed1%128;
  /*if((l_speed==0)&&(r_speed==0))
  {
    digitalWrite(break_indic,HIGH);
  }*/

  current_speed();        // calculating current speed
  
}

//***************************CURRENT SPEED CALCULATOR FUNCTION******************************

void current_speed()
{
  if(l_speed_hist!=l_speed)
  {
    l_speed_curr=l_speed_hist+(k*(l_speed-l_speed_hist)/100);
  }
  if(r_speed_hist!=r_speed)
  {
    r_speed_curr=r_speed_hist+(k*(r_speed-r_speed_hist)/100);
  }
  k++;
  if(k==101)
  {
    l_speed_hist=l_speed;
    r_speed_hist=r_speed;
    k=1;
  }
  
  speed2pulse_width();        //speed to pulse width function
  
}

//***************************SPEED TO PULSE WIDTH FUNCTION******************************

void speed2pulse_width()
{
  l_wheel=(l_speed_curr*1000)/max_speed;
  r_wheel=(r_speed_curr*1000)/max_speed;  
}

//***************************WHEEL DIRECTION FUNCTION******************************

void forward_motion()
{
  analogWrite(left_wheel_f,l_wheel);
  analogWrite(right_wheel_f,r_wheel);
  digitalWrite(right_wheel_b,LOW);
  digitalWrite(left_wheel_b,LOW);
  //Serial.println('r');
  
}
void reverse_motion()
{
  digitalWrite(right_wheel_f,LOW);
  digitalWrite(left_wheel_f,LOW);
  analogWrite(left_wheel_b,r_wheel);
  analogWrite(right_wheel_b,l_wheel);
  //Serial.println('f');
}

//***************************INDICATION FUNCTION******************************

void speed2indication_func()
{
  if(l_indic==1)
  {
    if(r_indic==1)
    {
      indication='v';
    }
    else
    {
      indication='l';
    }
  }
  else
  {
    if(r_indic==1)
    {
      indication='r';
    }
    else
    {
      indication='f';
      
    }
  }

  indicator_func();
  
}
void break_func()
{
  if((l_speed_curr<l_speed_hist)&&(r_speed_curr<r_speed_hist))
  {
    indication='b';
  }
  indicator_func();
}

//***************************INDICATOR FUNCTION******************************

void indicator_func()
{
  switch(indication)
  {
    case 'r' : 
    {
      digitalWrite(left_indic,LOW);
      switch(countr)
      {
        case 0 : indic_delay=millis();countr++;break;
        case 1 : 
        {      
          if((millis()-indic_delay)>1000)
        {
                led=~led;
                digitalWrite(right_indic,led);
                countr++;Serial.println(countr);
        }
        }break;
        default : countr=0;
      }
      break;
    }
    case 'l' : 
    {
       digitalWrite(right_indic,LOW);
      switch(countl)
      {
        case 0 : indic_delay=millis();countl=1;break;
        case 1 : 
        {      
          if((millis()-indic_delay)>1000)
        {
              led=~led;
              digitalWrite(left_indic,led);
                countl=0;Serial.println('l');
        }
        }break;
        default : countl=0;
      }
      
      break;
    }
    case 'v' : 
    {
      switch(countv)
      {
        case 0 : indic_delay=millis();countv=1;break;
        case 1 : 
        {      
          if((millis()-indic_delay)>1000)
        {
              led=~led;
        digitalWrite(left_indic,led);
        digitalWrite(right_indic,led);
                countv=0;Serial.println('v');
           
        }break;
        }
        default : countv=0;
      }
      break;
    }
    case 'b' : 
    {
      digitalWrite(break_indic,HIGH);
      break_delay=millis();Serial.println('b');
    }
    default :
    {
      if(((millis()-indic_delay)>4000)|(indic_delay=0))
      {
        //indic_delay=0;
      digitalWrite(left_indic,LOW);Serial.print(countl);Serial.print(countr);Serial.print(countv);
      digitalWrite(right_indic,LOW);Serial.println('d');
      //digitalWrite(rev_indic,LOW);
      }
    }
  }
}
