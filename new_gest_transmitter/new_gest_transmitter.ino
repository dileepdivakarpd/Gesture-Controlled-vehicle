const int apx = A0;                   //accelerometer analog pin x
const int apy = A1;                   //accelerometer analog pin y
const int apz = A2;                   //accelerometer analog pin z

int x,y,z;                            //analog values from accelerometer

float g=9.81;                         //acceleration of gravity in m/s^2
float a,ax,ay,az;                     //total acceleration and its components in x,y&z directions

int g_zero=512;                       //corresponding analog value for zero acceleration
int g_gravity=104;                    //corresponding analog value for +g acceleration

float angle;                          //angle measured using accelerometer
int ang1;                             //steering angle
int ang2;                             //steering angle direction (+1   

unsigned long int temp_time;
int acc_flag=0;                       //checking acceleration or indication

int temp1,temp2;

long debouncing_time = 15;            //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

const byte interruptPin1 = 2;         //left indication & acceleration
const byte interruptPin2 = 3;         //right indication & deceleration

const byte switch_l = 5;              //left indication & acceleration
const byte switch_m = 6;              //right indication & deceleration
const byte switch_r = 7;              //break

int l_indic,r_indic;                  //speed of left and right wheels in array
int velocity=0;                       //reference velocity
int max_velocity=100;                 //max velocity
int velocity_div=10;                  //number of velocity divisions
int l_velocity=0,r_velocity=0,l_velocity_pre=0,r_velocity_pre=0;            //speed of left and right wheels
int i,j,k;
void setup() 
{
  Serial.begin(9600);
  noInterrupts();
  attachInterrupt(digitalPinToInterrupt(interruptPin1), debounceInterrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), debounceInterrupt2, RISING);
  pinMode(switch_l, INPUT);
  pinMode(switch_m, INPUT);
  pinMode(switch_r, INPUT);
  interrupts();
}

void loop() 
{
  break_func();
  angle_measure();
  l_r_velocity();
}

//*******************************ANGLE MEASURE FUNCTION****************************

void angle_measure()
{
  analogReference(EXTERNAL);   
  
  x = analogRead(apx);//Serial.print(x);   Serial.print(" ");         
  y = analogRead(apy);//Serial.print(y);   Serial.print(" ");            
  z = analogRead(apz);//Serial.print(z);   Serial.println(" ");
  
  x=4*(x/4); 
  y=4*(y/4);
  z=4*(z/4);

  ax=((x-g_zero)*g)/g_gravity;
  ay=((y-g_zero)*g)/g_gravity;
  az=((z-g_zero)*g)/g_gravity;
  


  a=sqrt(ax*ax+ay*ay);

  if(a<g)
  {
    angle=acos(a/g);
  }
  else
  {
    angle=0;
  }
  angle=angle*180/3.14;

  if((angle>85)&&(angle<90))
  {
    ang1=40;
  }
  else if((angle>80)&&(angle<85))
  {
    ang1=35;
  }
  else if((angle>75)&&(angle<80))
  {
    ang1=30;
  }
  else if((angle>70)&&(angle<75))
  {
    ang1=25;
  }
  else if((angle>60)&&(angle<70))
  {
    ang1=20;
  }
  else if((angle>50)&&(angle<60))
  {
    ang1=15;
  }
  else if((angle>40)&&(angle<50))
  {
    ang1=10;
  }
  else if((angle>30)&&(angle<40))
  {
    ang1=5;
  }
  else
  {
    ang1=0;
  }//Serial.print(" ");
//Serial.println(angle);
  if(ang1!=0)
  {
    if(az>0)
    {
      ang2=1;     // right
    }
    else
    {
      ang2=-1;    // left
    }
  }
  else
  {
    ang2=0;
  }
}

//*******************************LEFT & RIGHT VELOCITY FUNCTION****************************

void l_r_velocity()
{
  if(ang2==1)
  {
    r_velocity=velocity;
    l_velocity=velocity-((ang1/5)*(max_velocity/velocity_div));
    l_velocity=(l_velocity+abs(l_velocity))/2;//Serial.print(" ");
    //Serial.print(velocity);Serial.print(" ");
  }
  else if(ang2==-1)
  {
    l_velocity=velocity;
    r_velocity=velocity-((ang1/5)*(max_velocity/velocity_div));
    r_velocity=(r_velocity+abs(r_velocity))/2;//Serial.print(" ");
   // Serial.print(r_velocity);Serial.print(" ");
  }
  else
  {
    l_velocity=velocity;
    r_velocity=velocity;
  }

  if(l_indic==1)
  {
    l_velocity=l_velocity+128;
  }
  if(r_indic==1)
  {
    r_velocity=r_velocity+128;
  }
  transmit_values();
}

//*******************************LEFT INDICATION & DECELERATION FUNCTION****************************

void left_indic_dec_func()
{
  interrupts();
  if(velocity==0)
  {
    temp_time=millis();
    while(digitalRead(switch_l))
    {
      if(millis()>=temp_time+1000)
      {
        acc_flag=1;
        velocity=(max_velocity/velocity_div);
        l_indic=1;
        r_indic=1;
        l_r_velocity();
      }
    }
  }
  else
  {
    temp_time=millis();
    while(digitalRead(switch_l))
    {
      if(millis()>=temp_time+1000)
      {
        temp_time=millis();
        acc_flag=1;
        velocity=velocity-(max_velocity/velocity_div);
        velocity=(velocity+abs(velocity))/2;
        l_r_velocity();
      }
    }
  }
  if(acc_flag==0&&((l_indic+r_indic)!=2))
  {
    l_indic=1;
    r_indic=0;
  }
  acc_flag=0;
}

//*******************************RIGHT INDICATION & ACCELERATION FUNCTION****************************

void right_indic_acc_func()
{
  interrupts();
  temp_time=millis();
 
  while(digitalRead(switch_r))
  {
    if(millis()>=temp_time+1000)
    {
      temp_time=millis();
      acc_flag=1;
      if((velocity+(max_velocity/velocity_div))<=max_velocity)
      {
        velocity=velocity+(max_velocity/velocity_div);
        velocity=(velocity+abs(velocity))/2;
      }
      l_r_velocity();
    }
  }
  if(acc_flag==0&&((l_indic+r_indic)!=2))
  {
    l_indic=0;
    r_indic=1;
  }
  acc_flag=0;
}

//*******************************BREAK FUNCTION****************************

void break_func()
{
  temp_time=millis();
  while(digitalRead(switch_m))
  {
    if(millis()>=temp_time+100)
    {
      velocity=0;
    }
    l_r_velocity();
    if(((l_indic+r_indic)==1)||(velocity==0))
    {
      l_indic=0;
      r_indic=0;
    }
  }
}

//*******************************DEBOUNCE****************************

void debounceInterrupt1() {
  if((long)(micros() - last_micros) >= debouncing_time * 1000) 
  {
    delayMicroseconds(100);
    left_indic_dec_func();
    last_micros = micros();
  }
}

void debounceInterrupt2() {
  if((long)(micros() - last_micros) >= debouncing_time * 1000)
  {
    delayMicroseconds(100);
    right_indic_acc_func();
    last_micros = micros();
  }
} 



void transmit_values()
{
  if((l_velocity_pre!=l_velocity)||(r_velocity_pre!=r_velocity))
  {
  Serial.write(255);
  delay(1);
  Serial.write(l_velocity);
  delay(1);
  Serial.write(r_velocity);
  //Serial.print(l_velocity);Serial.print("\t");Serial.println(r_velocity);
  l_velocity_pre=l_velocity;
  r_velocity_pre=r_velocity;
  }
  
}


