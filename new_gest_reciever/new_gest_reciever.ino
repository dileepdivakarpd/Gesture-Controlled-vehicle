#define left_wheel_f 3               //left wheel forward pin
#define right_wheel_f 5              //right wheel forward pin
#define left_wheel_b 6               //left wheel backward pin
#define right_wheel_b 9              //right wheel backward pin
#define left_indic 4                 //left indication pin
#define right_indic 7                //right indication pin
//#define rev_indic 8                  //reverse indication pin
#define break_indic 10                //break indication pin

boolean dir=HIGH;                    //direction of rotation of wheel
boolean led=HIGH;                     
int l_indic=0,r_indic=0;             //indication
int l_speed=0,r_speed=0;             //received from the transmitter
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
      l_speed=Serial.read();
      while(Serial.available()==0);
      r_speed=Serial.read();
      
    }
  }

  speed_mag_func();
  if((l_speed/128) && (r_speed/128))
  {
    reverse_motion();
    speed2indication_func();
    indicator_func();
  }
  else
  {
    forward_motion();
    speed2indication_func();
    indicator_func();
  }
  break_func();
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
  l_indic=l_speed/128;
  r_indic=r_speed/128;
  l_speed=l_speed%128;
  r_speed=r_speed%128;
  if((l_speed==0)&&(r_speed==0))
  {
    digitalWrite(break_indic,HIGH);
  }

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
  
  
}
void reverse_motion()
{
  analogWrite(left_wheel_b,l_wheel);
  analogWrite(right_wheel_b,r_wheel);
  digitalWrite(right_wheel_f,LOW);
  digitalWrite(left_wheel_f,LOW);
}

//***************************INDICATION FUNCTION******************************

void speed2indication_func()
{
  if(l_indic==1)
  {
    if(r_indic==1)
    {
      indication='b';
      dir=LOW;
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
      indication='v';
    }
    else
    {
      indication='f';
      dir=HIGH;
    }
  }

  indicator_func();
  
}
void break_func()
{
  if(l_speed_curr<l_speed_hist)
  {
    indication='b';
  }
  else
  {
    indication='s';
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
      led=~led;
      digitalWrite(right_indic,led);
      digitalWrite(left_indic,LOW);
      break;
    }
    case 'l' : 
    {
      led=~led;
      digitalWrite(left_indic,led);
      digitalWrite(right_indic,LOW);
      break;
    }
    case 'v' : 
    {
      led=~led;
      digitalWrite(left_indic,led);
      digitalWrite(right_indic,led);
      break;
    }
    case 'b' : 
    {
      digitalWrite(break_indic,HIGH);
    }
    case 's' : 
    {
      digitalWrite(break_indic,LOW);
    }
    default :
    {
      digitalWrite(left_indic,LOW);
      digitalWrite(right_indic,LOW);
      //digitalWrite(rev_indic,LOW);
    }
  }
}