/*******************************************************************************
 Projektname:       ACS I.cprj
 Benötigte Libs´s:  IntFunc_lib.cc
 Routinen:          ASC_I.cc
 Autor:             UlliS
 Datum:             03.02.2009

 Funktion:          PRO-BOT128 fährt umher, und weicht dabei Hindernisse über
                    das ACS (Anti Collisions System) aus.
                    Der Roboter wird in diesem Demo immer eine leichte Kurve
                    fahren, da hier keine Motor-Gleichlaufregelung vorhanden ist.
                    Sollte PRO-BOT128 Hindernisse nicht erkennen, oder das ACS System
                    ständig auch ohne Hindernisse regieren, muss ACS_Init()
                    verändert werden! (Bereich von 1 bis 20)
                    Hier sollte man die Parameter Schritt für Schritt auf die
                    richtigen Werte trimmen.

                     Projektname:       LineRacer.cprj
 Benötigte Libs´s:  IntFunc_lib.cc
 Routinen:          LineRacer.cc, PRO-BOT128C_Lib.cc
 Autor:             UlliS
 Datum:             03.02.2009

 Funktion:          PRO-BOT128 folgt einer schwarzen Linie auf den Boden.
                    Dazu müssen wir uns einen hellen Untergrund z.B. einen
                    Parket Boden oder eine große weiße Pappe suchen.
                    Darauf kleben wir mit schwarzem Isolierband (etwa 2cm breit)
                    eine Kreisrunde Bahn auf (Durchmesser etwa 1m oder größer).
                    Natürlich kann die Bahn auch andere Formen besitzen, wichtig
                    die Kurvenradien sollten aber nicht zu klein sein!
                    Unser Roboter wird auf die so auf die Linie gestellt, das
                    ein Fototransitor (T1) auf die schwarze Line blickt, und der
                    andere (T2) auf den hellen Untergrund. Im Raum darf es auch
                    nicht zu hell sein, da sonst die Kontrastunterschiede zwischen
                    Untergrund und der Line zu klein werden, und der Roboter der
                    Klebebandspur nicht richtig folgen kann.
                    Nach dem wir den Roboter richtig platziert haben, schalten wir
                    unseren PRO-BOT128 ein. Die Line LED muss nach 1Sekunde
                    einschalten, und der Roboter folgt der Linie.
                    Sollte er in den Kurven ausbrechen, müssen die Regler Parameter
                    P, I, D verändert werden und evtl. T1 und T2 leicht justiert
                    werden, bis der  Roboter der Linie sauber folgt. Der Abstand
                    von T1, T2 und D1 sollte etwa 5mm zur Fahrbahn betragen!
                    Hier ist eventuell etwas Basteln und Forschergeist gefragt!
*******************************************************************************/

#define PWM_IR 35
#define IR_left 27
#define IR_right 29
#define TSOP 26

#define FLL 19
#define FLR 18
#define BLL 17
#define BLR 16

#define Motor_Enable 15

byte ACS_R, ACS_L;
byte Speed, Left_Dir, Right_Dir;
int Speed_Left, Speed_Right, Line_Sensor[2];
int x, xalt, don, diff, P, D, I, P1, D1, I1, drest, y1, y2, sum;

void main(void)
{

    ACS_Init(5);        //ACS setup / sensitivity 1 To 20 / 1=near / 20 =far
    LED_Init();         //Flash LEDs
    Motor_Init();       //Motor setup
    AbsDelay(1000);     //Wait 1Sec.
    PRO_BOT128_INIT();                             //System setup
    DRIVE_ON();                                    //Drive setup

    MOTOR_DIR(1,1);                                //Set up To drive forward
    Speed = 240;                                   //Speed value is 150
    P = 5;                                         //PID Parameter (To experience)
    I = 10;
    D = 25;
    Speed_Left = Speed;                            //Speed transfer
    Speed_Right = Speed;

    do                  //Endless Loop
     {
       Check_Left();   //Check IR Sensor "ACS" left
       Check_Right();  //Check IR Sensor "ACS" right
       LineFollow();
       Status_LEDS();  //Switsch Status LEDs

        //Drive behaviour
       if ((ACS_L == 1) && (ACS_R == 1)) Forward();
        if ((ACS_L == 0) && (ACS_R == 0)) Backward();
        if ((ACS_L == 1) && (ACS_R == 0)) Turn_Left();
        if ((ACS_L == 0) && (ACS_R == 1)) Turn_Right();

     } while (1);

}



void Motor_Init(void)
{
   Timer_T1PWMX(256,128,128,PS_8);               //Setting up PWM channel A und B Timer1
    Port_DataDirBit(Motor_Enable,PORT_OUT);      //Port Enable Motor = Output
    Port_WriteBit(Motor_Enable,1);               //Port = High +5V
    Timer_T1PWA(128);                            //Motor stop!
    Timer_T1PWB(128);                            //Motor stop!
}


void LED_Init(void)
{
    Port_DataDirBit(FLL,PORT_OUT);               //Port PC.0 = Output
    Port_DataDirBit(FLR,PORT_OUT);               //Port PC.1 = Output
    Port_DataDirBit(BLL,PORT_OUT);               //Port PC.2 = Output
    Port_DataDirBit(BLR,PORT_OUT);               //Port PC.3 = Output
    Port_Write(2,0x00);                          //All LEDs "ON"
    AbsDelay(500);                               //Wait 1Sec.
    Port_Write(2,0x1F);                          //All LEDs "OFF"
    AbsDelay(500);                               //Wait 1Sec.
    Port_Write(2,0x00);                          //All LEDs "ON"
    AbsDelay(500);                               //Wait 1Sec.
    Port_Write(2,0x1F);                          //All LEDs "OFF"
}


void ACS_Init(byte sensitivity)
{
    Port_DataDirBit(IR_left,PORT_OUT);
    Port_DataDirBit(IR_right,PORT_OUT);
    Port_DataDirBit(TSOP,PORT_IN);

    //Calculating the pulse width modulation
    //Timer_T3PWM(Par1,Par2,PS);
    //Period=Par1*PS/FOSC (51*8/14,7456MHz=27,66 µs)  = 36Khz
    //Pulse=Par2*PS/FOSC (25*8/14,7456MHz=13,56 µs) On Time

    //Timer_T3PWM(Word period,Word PW0,Byte PS)  --> 36Khz
    Timer_T3PWM(51,sensitivity,PS_8);   //with Par1, Par2 can reach altered!
                                        //Responding To the ACS must be sensitive To these parameters are screwed!
}


void Check_Right(void)
{
    Port_WriteBit(IR_left,PORT_OFF);     //Check right side
    Port_WriteBit(IR_right,PORT_ON);
    AbsDelay(5);
    ACS_R = Port_ReadBit(TSOP);
    Port_WriteBit(IR_right,PORT_OFF);
}


void Check_Left(void)
{
    Port_WriteBit(IR_right,PORT_OFF);     //Check left side
    Port_WriteBit(IR_left,PORT_ON);
    AbsDelay(5);
    ACS_L = Port_ReadBit(TSOP);
    Port_WriteBit(IR_left,PORT_OFF);
}


void Forward(void)                        //Drive forward
{
    Timer_T1PWA(220);
    Timer_T1PWB(220);
    AbsDelay(250);
}


void Backward(void)                       //Drive backward
{
    Timer_T1PWA(180);
    Timer_T1PWB(30);
    AbsDelay(1000);
}


void Turn_Left(void)                      //Turn left
{
    Timer_T1PWA(30);
    Timer_T1PWB(180);
    AbsDelay(150);
}


void Turn_Right(void)                     //Turn right
{
    Timer_T1PWA(180);
    Timer_T1PWB(30);
    AbsDelay(150);
}


void Status_LEDS(void)                    //Change Status LEDs
{
    if (ACS_L == 0)
      {
       Port_WriteBit(BLL,PORT_ON);
      }
      else Port_WriteBit(BLL,PORT_OFF);

    if (ACS_R == 0)
      {
       Port_WriteBit(BLR,PORT_ON);
      }
      else Port_WriteBit(BLR,PORT_OFF);
}

void LineFollow(void)
{

   LINE_LED_OFF();
   Line_Sensor[0] = READ_LINE_LEFT();              //Measurement with LED off
   Line_Sensor[1] = READ_LINE_RIGHT();             //Values of the photo transistors pick up
   diff = Line_Sensor[0] - Line_Sensor[1];         //To compensate For ambient light
   LINE_LED_ON();
   Line_Sensor[0] = READ_LINE_LEFT();              //Measurement with LED on
   Line_Sensor[1] = READ_LINE_RIGHT();             //Values of the photo transistors pick up
   don = Line_Sensor[0] - Line_Sensor[1];
   x = don - diff;                                 //Rule deviation

   sum = sum + x;
   if (sum > 20000) sum = 20000;                   //Limit To avoid overflow
   if (sum < -20000) sum = -20000;

   I1 = sum / 625 * I;                             //Calculate I part
   D1 = (x - xalt) * D;                            //D-share calculated And compared with
   D1 = D1 + drest;                                //Take rest add

   if (D1 > 255)
     {
      drest = D1 - 255;                             //Hold rest
     }
    else
     {
      if (D1 < -255)
        {
         drest = D1 + 255;
        }
       else drest = 0;
      }

   P1 = x * P;                                     //Calculate P part
   y1 = P1+I1+D1;                                  //Substitute size of the PID controller calculate
   y2 = y1 / 2;                                    //Sharing on both engines
   xalt = x;                                       //Hold X

   Speed_Left = Speed;
   Speed_Right = Speed;

   MOTOR_DIR(1,1);                                 //Set To forward
   if (y1 > 0)                                     //Drive right
     {
      Speed_Left = Speed + y2;                     //Left accelerate
      if (Speed_Left > 255)
        {
          Speed_Left = 255;                        //Limit
          y2 = Speed_Left - Speed;                 //Then consider the right balance
        }

      y1 = y1 - y2;
      Speed_Right = Speed - y1;                    //Right slow
      if (Speed_Right < 0)
        {
          Speed_Right = 0;
        }
      }


   if (y1 < 0)                                     //Left
     {
      Speed_Right = Speed - y2;                    //Right speed
      if (Speed_Right > 255)
        {
          Speed_Right = 255;                       //Limit
          y2 = Speed - Speed_Right;                //Then left the rest take into account
        }

      y1 = y1 - y2;
      Speed_Left = Speed + y1;                     //Slow down the left
      if (Speed_Left < 0)
        {
          Speed_Left = 0;
        }
      }


   MOTOR_DIR(1,1);                                 //Set up toforward
   MOTOR_POWER(ABS_INT(Speed_Left),ABS_INT(Speed_Right)); //Values To the engine driver (PWM)
}

