/*******************************************************************************
 Projektname:       ACS II.cprj
 Benötigte Libs´s:  IntFunc_lib.cc
 Routinen:          ACS_II.cc, PRO-BOT128C_Lib.cc
 Autor:             UlliS
 Datum:             03.02.2009

 Funktion:          PRO-BOT128 fährt solange geradeaus bis er auf ein Hindernis
                    trifft. Je nachdem ob das Hindernis links oder rechts vom
                    Roboter aus liegt, weicht er dem Hindernis mit einer
                    links oder rechts Drehung aus.
                    Die Fahrtrichtung wird PRO-BOT128 über die Befehle
                    DRIVE_FORWARD() und GO_TURN() vorgegeben.
                    Der Befehle DRIVE_FORWARD() beinhaltet einen Geradeauslauf
                    der dazu die Odometer Sensoren verwendet. Sollte Ihr PRO-BOT128
                    nicht Geradeausfahren, sondern sich nur im Kreis drehen,
                    müssen die Odometer Sensoren (D2, T3 sowie T3, D4) justiert
                    werden. Dies kann sich durchaus als "Gefummel" herausstellen!
                    Die Bauteile D2, T3 sowie T3, D4 sollten dabei vorsichtig Richtung
                    Encoderscheiben gedrückt werden. Als Encoderscheibe wurde in
                    diesen Beispiel, die mit den vier schwarzen Flächen verwendet.
                    Es hilft auch zudem die Encoderscheiben mit schwarzem Edding-Stift
                    nachzuschwärzen. Hier ist ein wenig Fingerspitzengefühl und Ausdauer
                    gefragt ;-) Zudem sollte es nicht zu hell sein, da die
                    Foto-Transistoren bereits bei Sonnen-Einstrahlung durchsteuern.

*******************************************************************************/

void main(void)
{

    PRO_BOT128_INIT();    //PRO-BOT128 Setup
    ACS_INIT(5);          //ACS setup / sensitivity 1 To 20 / 1=near / 20 =far

    AbsDelay(1000);       //Wait 1Sec.
    BLL_ON();             //Back LED left "ON"
    BLR_ON();             //Back LED right "ON"
    ENC_LED_ON();         //Encoder IR-LEDs "ON"
    DRIVE_ON();           //Motor "ON"

    do                    //Endless Loop
      {
        //Drive behaviour
        if ((ACS_LEFT() == 1) && (ACS_RIGHT() == 1)) Forward();
        if ((ACS_LEFT() == 0) && (ACS_RIGHT() == 0)) Backward();
        if ((ACS_LEFT() == 1) && (ACS_RIGHT() == 0)) Turn_Left();
        if ((ACS_LEFT() == 0) && (ACS_RIGHT() == 1)) Turn_Right();

    } while (1);

}


void Forward(void)      //Drive forward
{
    FLL_OFF();          //Front LED left "OFF"
    FLR_OFF();          //Front LED right "OFF"
    DRIVE_FORWARD(7);   //Drive forward, speed 1 To 10 : Value = 7
    DELAY_MS(150);      //Wait 150ms
}

void Backward(void)     //Drive backward
{
    FLL_ON();           //Front LED left "ON"
    FLR_ON();           //Front LED right "ON"
    GO_TURN(-15,0,150); //Backward 15cm, Speed = 150
    GO_TURN(0,60,150);  //Turn right, speed = 150
}

void Turn_Left(void)    //Turn left
{
    FLL_OFF();          //Front LED left "OFF"
    FLR_ON();           //Front LED right "ON"
    GO_TURN(0,-45,150); //Turn left, Speed = 150
}

void Turn_Right(void)   //Turn right
{
    FLL_ON();           //Front LED left "ON"
    FLR_OFF();          //Front LED right "OFF"
    GO_TURN(0,45,150);  //Turn right, Speed = 150
}
