#define EN1 10
#define EN2 9

#define I11 2
#define I12 3
#define I13 4
#define I14 5

#define STEPS 50

#define P1 13
#define P2 12
#define P3 11
#define P4 7

#define STEP_PER_REV 4096 // steps for 360 degree turn

//Serial Console Commands

//MotorID = 1 2
//0: Stop
//1: CW
//2: CCW

//C,2,0   ==> stops the 2. motor
//M,1,255 ==> full throttles 1. motor

char c;
int n1;
int n2;

unsigned long t; // for stepper
unsigned long t_last; //last serial communication

int _step = 0;
int StepNum = 0;

void set4(int a, int b, int c, int d){
  digitalWrite(P1, a);
  digitalWrite(P2, b);
  digitalWrite(P3, c);
  digitalWrite(P4, d);
}

void setStep(int x){
  switch(x){
    case 0:
      set4(1, 0, 0, 0);
      break;
    case 1:
      set4(1, 1, 0, 0);
      break;
    case 2:
      set4(0, 1, 0, 0);
      break;
    case 3:
      set4(0, 1, 1, 0);
      break;
    case 4:
      set4(0, 0, 1, 0);
      break;
    case 5:
      set4(0, 0, 1, 1);
      break;
    case 6:
      set4(0, 0, 0, 1);
      break;
    case 7:
      set4(1, 0, 0, 1);
      break;
    default:
      setStep(x+8); // in case of negative integer modulus (they're negative too)
      break;
  }
}

void stepUp(){
  setStep(++_step %= 8);
}
void stepDown(){
  setStep(--_step %= 8);
}

int setMotorSpeed(int motorID, int x){
  switch(motorID){
    case 1:
      analogWrite(EN1, constrain(x, 0, 255));
      break;
    case 2:
      analogWrite(EN2, constrain(x, 0, 255));
      break;
    default:
      //Serial.println("setMotorSpeed yanlis motorID ile cagirildi. motorID'ler: 1 2");
      return 1;
  }
  return 0;
}

int stopMotor(int motorID){
  switch(motorID){
    case 1:
      digitalWrite(I11, LOW);
      digitalWrite(I12, LOW);
      break;
    case 2:
      digitalWrite(I13, LOW);
      digitalWrite(I14, LOW);
      break;
    default:
      //Serial.println("stopMotor yanlis motorID ile cagirildi. motorID'ler: 1 2");
      return 1;
  }
  return 0;
}

int cwMotor(int motorID){
  switch(motorID){
    case 1:
      digitalWrite(I11, HIGH);
      digitalWrite(I12, LOW);
      break;
    case 2:
      digitalWrite(I13, HIGH);
      digitalWrite(I14, LOW);
      break;
    default:
      //Serial.println("cwMotor yanlis motorID ile cagirildi. motorID'ler: 1 2");
      return 1;
  }
  return 0;
}

int ccwMotor(int motorID){
  switch(motorID){
    case 1:
      digitalWrite(I11, LOW);
      digitalWrite(I12, HIGH);
      break;
    case 2:
      digitalWrite(I13, LOW);
      digitalWrite(I14, HIGH);
      break;
    default:
      //Serial.println("ccwMotor yanlis motorID ile cagirildi. motorID'ler: 1 2");
      return 1;
  }
  return 0;
}

void setup() {
  pinMode(I11, OUTPUT);
  pinMode(I12, OUTPUT);
  pinMode(I13, OUTPUT);
  pinMode(I14, OUTPUT);
  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);
  pinMode(P1, OUTPUT);
  pinMode(P2, OUTPUT);
  pinMode(P3, OUTPUT);
  pinMode(P4, OUTPUT);
  digitalWrite(P1, LOW);
  digitalWrite(P2, LOW);
  digitalWrite(P3, LOW);
  digitalWrite(P4, LOW);
  digitalWrite(I11, LOW);
  digitalWrite(I12, LOW);
  digitalWrite(I13, LOW);
  digitalWrite(I14, LOW);
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);
  Serial.begin(9600);
  t = millis();
  t_last = t;
}

void loop() {
  if(millis() > t){
    t = millis();
    if(StepNum > 0){
      stepUp();
      --StepNum;
    }
    else if(StepNum < 0){
      stepDown();
      ++StepNum;
    }
  }
  if(millis() > t_last + 1000){
      t_last = millis();
      stopMotor(1);
      stopMotor(2);
  }
  
  if(Serial.available()){
    t_last = millis();
    c = Serial.read();
    if(c == 'C'){
      n1 = Serial.parseInt();
      n2 = Serial.parseInt();
      switch(n2){
        case 0:
          if(stopMotor(n1) == 0){
            //Serial.print(n1);
            //Serial.println(" no'lu motor durduruldu.");
          }
          break;
        case 1:
          if(cwMotor(n1) == 0){
            //Serial.print(n1);
            //Serial.println(" no'lu motor saat yonune ayarlandi.");
          }
          break;
        case 2:
          if(ccwMotor(n1) == 0){
            //Serial.print(n1);
            //Serial.println(" no'lu motor ters saat yonune ayarlandi.");
          }
          break;
        //default:
          //Serial.println("Usage: C,motorID,NUM ==> 0: stop motor, 1: CW rotation, 2: CCW rotation");
      }
    }
    else if(c == 'M'){
      n1 = Serial.parseInt();
      n2 = Serial.parseInt();
      //if(
      setMotorSpeed(n1, n2);
      //== 0){
        //Serial.print(n1);
        //Serial.print(" nolu motor ");
        //Serial.print(n2);
        //Serial.println(" hizina ayarlandi");
      }
    }
    else if(c == 'S'){
      StepNum += STEPS;
    }
    else if(c == 's'){
      StepNum -= STEPS;
    }
    else if(c == 'T'){
      StepNum += STEP_PER_REV;
  }
}
