/*
  Made by Chris Dubois
  For Gorik Aelbrecht

  Project is a Shuffleton Electronics
*/
#define DEBUG
#ifdef DEBUG
  #define DEBUG_PRINT(x)  Serial.println (x)
#else
  #define DEBUG_PRINT(x)
#endif

//#define SIMULATION


#ifndef SIMULATION
#include "TM1637.h"
#else
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#endif


#include "math.h"

// Timings needed to make program work
//Button
#define SHORTPRESS 10  // Shortpress of button in ms 
#define ADDPLAYERDURATION 1000 // ends addplayer mode, starts game, if button pushed 
#define STARTGAME 3050 // ends GAME, goes in reset if button pushed longer 
// Hole
#define VALIDBEAMBROKEN 10 // min time in ms to detect a sjoel in the hole
//Physical
#define AMOUNTHOLES 10

#define HOLER1H1PIN 19 //R1 = CLOSEST TO SCOREBOARD, COUNTING LEFT TO RIGHT
#define HOLER1H1SCORE 250

#define HOLER1H2PIN 18
#define HOLER1H2SCORE 450

#define HOLER1H3PIN 17
#define HOLER1H3SCORE 250

#define HOLER2H1PIN 16 //R2
#define HOLER2H1SCORE 200

#define HOLER2H2PIN 15
#define HOLER2H2SCORE 20

#define HOLER2H3PIN 13
#define HOLER2H3SCORE 20

#define HOLER2H4PIN 12
#define HOLER2H4SCORE 20

#define HOLER2H5PIN 11
#define HOLER2H5SCORE 200

#define HOLER3H1PIN 1
#define HOLER3H1SCORE 105

#define HOLER3H2PIN 0
#define HOLER3H2SCORE 105


// Game
#define MAXPLAYERS 4
#define WINNINGSCORE 6001
#define SJOELDETECTION 5
#define BONUSUPPERROW 550
#define BONUSEXTREMITIES 600

#define UPPERROWBONUSPIN1 HOLER1H1PIN
#define UPPERROWBONUSPIN2 HOLER1H2PIN
#define UPPERROWBONUSPIN3 HOLER1H3PIN

#define EXTREMITIESBONUSPIN1 HOLER2H1PIN
#define EXTREMITIESBONUSPIN2 HOLER2H5PIN


// Pins definitions for TM1637 and can be changed to other ports
// Define the Displays

#define CLK_1 4
#define DIO_1 5

#define CLK_2 6
#define DIO_2 7

#define CLK_3 8
#define DIO_3 9

#define CLK_4 2
#define DIO_4 3

// Pin Definitions for Push Button

#define RESETBUTTON 14

// Generic Class to be able so simulate in WOKWI.COM

#ifdef SIMULATION
class GenericSevenSegment: public SevenSegmentExtended
{
  public:
    GenericSevenSegment(const int& CLK, const int& DIO): SevenSegmentExtended(CLK, DIO) {};
    void scrollingText(const char* str, uint8_t repeats);
    void nightrider();
    void displayStr(String Text);
    void displayNum(int Num);
    void set(int Brightness);
};

void GenericSevenSegment::nightrider() {
  nightrider();
}

void GenericSevenSegment::scrollingText(const char* str, uint8_t repeats) {
  scrollingText(str,repeats);
}

void GenericSevenSegment::displayStr(String Text)
{
  print(Text);
};

void GenericSevenSegment::displayNum(int Num)
{
  char sctr[5];
  sprintf(sctr, "%04d", Num);
  print(sctr);
};


void GenericSevenSegment::set(int Brightness) {
  setBacklight(Brightness);
}
#endif

//INITIALISE GLOBAL VARIABLES

#ifndef SIMULATION
TM1637  tm1637_1(CLK_1, DIO_1);
TM1637  tm1637_2(CLK_2, DIO_2);
TM1637  tm1637_3(CLK_3, DIO_3);
TM1637  tm1637_4(CLK_4, DIO_4);
TM1637 tml1637Array[] = {tm1637_1, tm1637_2, tm1637_3, tm1637_4};
#else

#define BRIGHT_TYPICAL 100

GenericSevenSegment  tm1637_1(CLK_1, DIO_1);
GenericSevenSegment  tm1637_2(CLK_2, DIO_2);
GenericSevenSegment  tm1637_3(CLK_3, DIO_3);
GenericSevenSegment  tm1637_4(CLK_4, DIO_4);
GenericSevenSegment  tml1637Array[] = {tm1637_1, tm1637_2, tm1637_3, tm1637_4};
#endif

int BeamPins[] = {HOLER1H1PIN, HOLER1H2PIN, HOLER1H3PIN, HOLER2H1PIN, HOLER2H2PIN, HOLER2H3PIN, HOLER2H4PIN, HOLER2H5PIN, HOLER3H1PIN, HOLER3H2PIN};
int Points[] = {HOLER1H1SCORE, HOLER1H2SCORE, HOLER1H3SCORE, HOLER2H1SCORE, HOLER2H2SCORE, HOLER2H3SCORE, HOLER2H4SCORE, HOLER2H5SCORE, HOLER3H1SCORE, HOLER3H2SCORE};

int val = 0;  // variable to store the value read
int resetState = 0;
int sensorState = 0;
int x = 0;
//char* Players[] = {"P1", "P2", "P3", "P4"};
long int countblinkstart;
long int countblinkend;
bool blinkon;


enum gameStates {
  RESET,// GAME Has been Reset
  END, // Games has ended
  ONGOING, // Choosing PLayers
  PLAYING // Players are playing ;)
};

enum sensorStates {
  BeamOngoing,
  BeamBroken
};

enum pointStates {
  noPointsToAdd,
  PointsToAdd
};

enum debugStates {
  noDebug,
  Debug
};

enum resetStates {
  noReset,
  resetButtonPressed,
  fullReset,
  startSelectPlayers,
  endSelectPlayers
};

enum displayStates {
  counting,
  playing,
  adding,
  resetting,
  chooseplayers
};




class MatrixDisplay {
  private:
    String Display1Content;
    String Display2Content;
    String Display3Content;
    String Display4Content;
    char Display1ContentC[5];
    char Display2ContentC[5];
    char Display3ContentC[5];
    char Display4ContentC[5];
    char* DisplayContent[4];
    displayStates DisplayState;
  public:
    MatrixDisplay();
    void Init();
    void ShowDisplays();
    void ChoosePlayer(int amountPlayers);
    void DisplayStringPlayer(int Player, char* Text);
    void DisplayNumPlayer(int Player, int Num);
    void CountTime(int Counter, int CurrentPlayer);
    void ClearDisplay();
    void StartGame(int amountPlayers);
    void ResetDisplayContent();
    void ShowAmountPlayers(int AmountPlayers);
    void PrepareDisplayContent();
    void ShowAllScores(int Scores[4], int amountplayers);
    void AddPlayerMode();
    void ShowResetMessage();
    void ShowOngoingAmountPlayers(int Players);
    void ShowScore(int Score, int Player);
    void ShowBonus(int Player);
    void BlinkingPlayer(int Player, int PlayerScore, bool ButtonPushed);
    void WinningPlayer(int Player, int PlayerScore, bool ButtonPushed);
};

MatrixDisplay::MatrixDisplay() {
  this->ResetDisplayContent();
  this->PrepareDisplayContent();
  this->DisplayState = playing;
};

void MatrixDisplay::BlinkingPlayer(int Player, int PlayerScore, bool ButtonPushed) {
  int brightness;
  countblinkend = millis();
  if (!ButtonPushed) {
    int duration = (int) ((countblinkend - countblinkstart) / 1000);
    if ((duration % 2) == 0) {
      tml1637Array[Player - 1].displayStr("    ");
    } else
    {
      ShowScore(PlayerScore, Player);
    }
  }
}
void MatrixDisplay::WinningPlayer(int Player, int PlayerScore, bool ButtonPushed) {
  char sctr[5];
  char showstr[5];
  int shift;

  sprintf(sctr, "%04d", PlayerScore);
  showstr[4]=sctr[4];
  countblinkend = millis();
  if (!ButtonPushed) {
    int duration = (int) ((countblinkend - countblinkstart) / 500);
    shift = duration % 4;
    for (int x=0;x<4;x++) {
      showstr[x]=sctr[(x+shift) % 4];
    }
      tml1637Array[Player - 1].displayStr(showstr);
  }
}


void MatrixDisplay::PrepareDisplayContent() {
  this->Display1Content.toCharArray(Display1ContentC, 5);
  this->Display2Content.toCharArray(Display2ContentC, 5);
  this->Display3Content.toCharArray(Display3ContentC, 5);
  this->Display4Content.toCharArray(Display4ContentC, 5);

  this->DisplayContent[0] = this->Display1ContentC;
  this->DisplayContent[1] = this->Display2ContentC;
  this->DisplayContent[2] = this->Display3ContentC;
  this->DisplayContent[3] = this->Display4ContentC;
};

void MatrixDisplay::ResetDisplayContent() {
  this->Display1Content = "PLAY";
  this->Display2Content = "PLAY";
  this->Display3Content = "PLAY";
  this->Display4Content = "PLAY";
};

void MatrixDisplay::Init() {
  DEBUG_PRINT("Initialise Displays");
  countblinkstart = millis();
  this->ShowDisplays();
};

void MatrixDisplay::ShowAmountPlayers(int amountPlayers) {
  this->Display1Content = "PLA ";
  this->Display2Content = "YERS";
  this->Display3Content = ":   ";
  char b[4];
  String str;
  str = String(amountPlayers);
  str.toCharArray(b, 2);
  this->Display4Content = b; // convert int to char
  this->PrepareDisplayContent();
  this->ShowDisplays();
};

void MatrixDisplay::ShowAllScores(int scores[], int amountplayers) {
  //Serial.println("Show Scores");
  this->ClearDisplay();
  for (x = 0; x < amountplayers; x++) {
    //Serial.println(scores[x]);
    if (scores[x] == 0) tml1637Array[x].displayStr("0000");
    else
      ShowScore(scores[x], x + 1);
  }

};

void MatrixDisplay::ShowBonus(int currentPlayer){
  DisplayStringPlayer(currentPlayer,"_--_");
  delay(300);
}

void MatrixDisplay::StartGame(int amountPlayers) {
  int scores[4] = {0, 0, 0, 0};
  this->ShowAllScores(scores, amountPlayers);
  delay(1000);
};

void MatrixDisplay::ShowDisplays() {
  for (int x = 0; x < 4; x++) {
    tml1637Array[x].init();
    tml1637Array[x].set(BRIGHT_TYPICAL);
    tml1637Array[x].displayStr(DisplayContent[x]);
    DEBUG_PRINT(this->DisplayContent[x]);
    //delay(1);
  }
};

void MatrixDisplay::ShowResetMessage() {
  this->ResetDisplayContent();
  this->PrepareDisplayContent();
  this->ShowDisplays();
};

void MatrixDisplay::DisplayStringPlayer(int Player, char* Text) {
  tml1637Array[Player - 1].displayStr(Text);
};

void MatrixDisplay::DisplayNumPlayer(int Player, int Num) {
  tml1637Array[Player - 1].displayNum(Num);
};

void MatrixDisplay::ShowScore(int Score, int Player) {
  char sctr[5];
  sprintf(sctr, "%04d", Score);
  //Serial.println(sctr);
  this->DisplayStringPlayer(Player, sctr);
}

void MatrixDisplay::AddPlayerMode() {
  this->Display1Content = "ADD ";
  this->Display2Content = "PLAY";
  this->Display3Content = "ERS ";
  this->Display4Content = "   ";
  this->PrepareDisplayContent();
  this->ShowDisplays();
  delay(2000);
  this->StartGame(1);
};

void MatrixDisplay::ClearDisplay()
{
  for (x = 0; x < 4; x++) {
    tml1637Array[x].displayStr("    ");
  }
};

void MatrixDisplay::ShowOngoingAmountPlayers(int Players) {
  int emptyScores[] = {0, 0, 0, 0};
  ShowAllScores(emptyScores, Players);
};

void MatrixDisplay::CountTime(int Duration, int CurrentPlayer) {
  String DisplayStr="-   ";
  int DisplayCounter;
  int x, y;
  int maxCounter;

  int Counter = (int) (Duration / 1000);
  DisplayCounter = Counter % 4;
  DisplayStr[0]='-';
  for (x = 1; x <= (DisplayCounter); x++) {
    DisplayStr[x]='-';
  }
  for (y=x+1;y<4;y++) {
    DisplayStr[y]=' ';
  }
  DEBUG_PRINT(DisplayCounter);
#ifndef SIMULATION
  tml1637Array[CurrentPlayer - 1].displayStr((char*) DisplayStr.c_str());
#else
  tml1637Array[CurrentPlayer - 1].displayStr(DisplayStr);
#endif

};

MatrixDisplay SjoelBakDisplay;

class SjoelBak {
  private:
    int amountPlayers;
    int currentPlayer;
    int scores[MAXPLAYERS];
    gameStates gameState;
    bool buttonpushed;
    bool tempWinner;
    int tempWinningPlayer;
    int WinningPlayer;
    byte upperRowBonusCheck;
    byte extremitiesBonusCheck;
    int PlayerBonusCheck;

  public:
    SjoelBak();
    void Init();
    void ResetGame();
    void StartGame(int amountPlayers);
    void AddPoints(int Scores, int PinHole);
    void NextPlayer();
    void CheckScore();
    gameStates ReturnState();
    void SetState(gameStates gameState);
    int GetAmountPlayers();
    void SetAmountPlayers(int AmountPlayers);
    void EndGame();
    int GetCurrentPlayer();
    int GetScoreCurrentPlayer();
    void SetButtonPushed(bool pushed);
    bool GetButtonPushed();
    void SetWinningPlayer(int winningPlayer);
    int GetWinningPlayer();
    int GetWinningPlayerScore();
    void SetGameWinnerMode(int winningPlayer);
};

SjoelBak::SjoelBak() {
};

void SjoelBak::SetWinningPlayer(int winningPlayer){
  this->WinningPlayer=winningPlayer;
}

int SjoelBak::GetWinningPlayer()
{
  return this->WinningPlayer;
}

int SjoelBak::GetWinningPlayerScore()
{
  return scores[this->WinningPlayer-1];
}


bool SjoelBak::GetButtonPushed() {
  return this->buttonpushed;
}

void SjoelBak::SetButtonPushed(bool pushed) {
  this->buttonpushed = pushed;
}


int SjoelBak::GetScoreCurrentPlayer() {
  return this->scores[this->currentPlayer - 1];
}


int SjoelBak::GetCurrentPlayer() {
  return this->currentPlayer;
}
void SjoelBak::SetAmountPlayers(int AmountPlayers) {
  this->amountPlayers = AmountPlayers;
}

int SjoelBak::GetAmountPlayers() {
  return this->amountPlayers;
}

void SjoelBak::SetState(gameStates gameState) {
  this->gameState = gameState;
};

gameStates SjoelBak::ReturnState() {
  return this->gameState;
};

void SjoelBak::Init() {
  DEBUG_PRINT("Initialise Sjoelbak");
  this->tempWinner = false;

};

void SjoelBak::ResetGame() {
  this->amountPlayers = 1;
  this->currentPlayer = 1;
  this->PlayerBonusCheck=1;
  this->gameState = RESET;
  this->tempWinner = false;
  upperRowBonusCheck =0;
  extremitiesBonusCheck=0;

  int x = 0;
  for (int x = 0; x < 4; x++) {
    this->scores[x] = 0;
  }
  SjoelBakDisplay.ShowResetMessage();
  delay(2000);
  SjoelBakDisplay.AddPlayerMode();
  delay(1000);
  this->gameState = ONGOING;
};


void SjoelBak::EndGame() {
  SjoelBakDisplay.ShowAllScores(this->scores, this->amountPlayers);
};

void SjoelBak::StartGame(int amountPlayers) {
  this->amountPlayers = amountPlayers;
  this-> currentPlayer = 1;
  this->gameState = ONGOING;
  for (int x = 0; x < 4; x++) {
    this->scores[x] = 0;
  }
  SjoelBakDisplay.StartGame(this->amountPlayers);
};

void SjoelBak::AddPoints(int Score, int PinHole) {
  if (this->gameState == PLAYING) {
    if (this->currentPlayer!= this->PlayerBonusCheck) {
      this->PlayerBonusCheck=this->currentPlayer;
      upperRowBonusCheck=0;
      extremitiesBonusCheck=0;
    }
    this->scores[currentPlayer - 1] += Score;
    SjoelBakDisplay.ShowScore( this->scores[this->currentPlayer - 1], this->currentPlayer);

    switch (PinHole) {
      case UPPERROWBONUSPIN1:
          upperRowBonusCheck= upperRowBonusCheck | 8;
        break;
      case UPPERROWBONUSPIN2:
          upperRowBonusCheck= upperRowBonusCheck | 4;
        break;
      case UPPERROWBONUSPIN3:
          upperRowBonusCheck= upperRowBonusCheck | 2;
        break;
      case EXTREMITIESBONUSPIN1:
          extremitiesBonusCheck =extremitiesBonusCheck | 8;
        break;
      case EXTREMITIESBONUSPIN2:
        extremitiesBonusCheck =extremitiesBonusCheck | 4;
        break;

    }

    if (upperRowBonusCheck==(8+4+2)) 
    {
      this->scores[this->currentPlayer - 1] +=BONUSUPPERROW;
      SjoelBakDisplay.ShowBonus(this->currentPlayer);
      SjoelBakDisplay.ShowScore( this->scores[this->currentPlayer - 1], this->currentPlayer);
      upperRowBonusCheck=0;
    }

    if (extremitiesBonusCheck==(8+4)) 
    {
      this->scores[this->currentPlayer - 1] +=BONUSEXTREMITIES;
      SjoelBakDisplay.ShowBonus(this->currentPlayer);
      SjoelBakDisplay.ShowScore( this->scores[this->currentPlayer - 1], this->currentPlayer);
      extremitiesBonusCheck=0;
    }

  }

};

void SjoelBak::NextPlayer() {
  this->CheckScore();
  this->currentPlayer++;
  if (this->currentPlayer > this->amountPlayers) this->currentPlayer = 1;
  SjoelBakDisplay.ShowAllScores(this->scores, this->amountPlayers);
  this->CheckScore();

  //SjoelBakDisplay.ShowScore( this->scores[this->currentPlayer-1],this->currentPlayer);
};

void SjoelBak::SetGameWinnerMode(int winningPlayer){
  this->WinningPlayer=winningPlayer;
  this->gameState=END;
}

void SjoelBak::CheckScore() {

  bool BREAK = false;

  if (this->currentPlayer == 1 and this->tempWinner and !BREAK) {
    DEBUG_PRINT("Winning state 6");
    SetGameWinnerMode(this->tempWinningPlayer);
    BREAK=true;
  }

  if (this->currentPlayer == this->amountPlayers and !this->tempWinner and !BREAK)
  {
    if (this->scores[this->currentPlayer-1] >= WINNINGSCORE) {
      DEBUG_PRINT("Winning state 1");
      SetGameWinnerMode(this->currentPlayer);
    }
    BREAK=true;
  }

  if (this->currentPlayer < this->amountPlayers and !this->tempWinner and !BREAK) {
    if (this->scores[this->currentPlayer-1] >= WINNINGSCORE) {
      DEBUG_PRINT("Winning state 2");
      this->tempWinner = true;
      this->tempWinningPlayer = this->currentPlayer;
    }
    BREAK=true;
  }
  if (this->currentPlayer < this->amountPlayers and this->tempWinner and !BREAK) {
    if (this->scores[this->currentPlayer-1] >= WINNINGSCORE) {
      DEBUG_PRINT("Winning state 3");
      if (this->scores[this->currentPlayer-1] > this->scores[this->tempWinningPlayer-1]) {
        this->tempWinningPlayer = this->currentPlayer;
      }
    }
    BREAK=true;
  }
  if (this->currentPlayer == this->amountPlayers and this->tempWinner and !BREAK)
  {
    if (this->scores[this->currentPlayer-1] >= WINNINGSCORE) {
      if (this->scores[this->currentPlayer-1] > this->scores[this->tempWinningPlayer-1]) {
        DEBUG_PRINT("Winning state 4");
        this->tempWinner=false;
        SetGameWinnerMode(this->currentPlayer);
      } else
      {
        DEBUG_PRINT("Winning state 5");
        this->tempWinner=false;
        SetGameWinnerMode(this->tempWinningPlayer);
      }
    }

    BREAK=true;
  }

  


};

SjoelBak Game;

class Hole {
  private:
    int duration;
    int measuredState;
    sensorStates holeState;
    sensorStates previousHoleState;
    pointStates pointState;
    debugStates debugState;
    int points;
    int digitalPin;
    bool sjoeldetected;
    long int beginfallsjoel;
    long int endfallsjoel;
    int durationfall;
    int scorestocount = 0;

  public:
    Hole(int digitalpin, int points, debugStates debug);
    void Init();
    void checkBeam();
    void preparePoints();
    int getPoints();
};


Hole::Hole(int digitalpin, int points, debugStates debug) {
  this->points = points;
  this->digitalPin = digitalpin;
  this->debugState = debug;
  this->duration = 0;
  this->holeState = BeamOngoing;
  this->previousHoleState = BeamOngoing;
  this->pointState = noPointsToAdd;
  this->measuredState = HIGH;
  //tm1637_1.displayStr("PZZ");
};

void Hole::Init() {
  DEBUG_PRINT("Initialise Hole");
  pinMode(digitalPin, INPUT_PULLUP);
};

int Hole::getPoints() {
  int pointstoreturn;
  if (this->pointState == PointsToAdd) {
    pointstoreturn = this->points;
  } else
  {
    pointstoreturn = 0;
  }

  return pointstoreturn;
};

void Hole::preparePoints() {
  if (this->duration > VALIDBEAMBROKEN) {
    this->pointState = PointsToAdd;
  }
};

void Hole::checkBeam() {
  this->measuredState = digitalRead(this->digitalPin);
  if (this->measuredState == LOW and this->sjoeldetected == false) {
    this->sjoeldetected = true;
    this->beginfallsjoel = millis();
  }

  if (this->measuredState == LOW and this->sjoeldetected) {
    this->endfallsjoel = millis();
  }

  if (this->measuredState == HIGH and this->sjoeldetected) {
    this->endfallsjoel = millis();
    this->duration = this->endfallsjoel - this->beginfallsjoel;
    this->sjoeldetected = false;
    //Serial.println("Beam broken");
    //Serial.println(this->duration);
    if (this->duration > SJOELDETECTION) {
     DEBUG_PRINT("SjoelDetected");
      DEBUG_PRINT(this->points);
      Game.AddPoints(this->points,this->digitalPin);
    }
  }
};


class ResetButton {
  private:
    int duration;
    int measuredState;
    int previousmeasuredState;
    int amountPlayers;
    int digitalPin;
    bool startpushing;
    bool buttonpushed;
    long int startcounterpush;
    long int endcounterpush;
    resetStates resetState;
  public:
    ResetButton(int digitalPin);
    void Init();
    void ChoosePlayers();
    void Measure();
    void CheckAction();
    void FullReset();
};

ResetButton::ResetButton(int digitalPin) {
  this->digitalPin = digitalPin;
  this->FullReset();
};

void ResetButton::Init() {
  DEBUG_PRINT("Initialise ResetButton");
  pinMode(this->digitalPin, INPUT);
};


void ResetButton::Measure() {

  this->measuredState = digitalRead(this->digitalPin);

  if (this->buttonpushed) DEBUG_PRINT("Button Pushed");

  if (this->measuredState == HIGH and not startpushing) {
    this->startpushing = true;
    DEBUG_PRINT("Start Pushing");
    this->buttonpushed = false;
    this->startcounterpush = millis();
    this->duration = 0;
    SjoelBakDisplay.CountTime(0, Game.GetCurrentPlayer());
  };

  if (this->measuredState == HIGH and startpushing) {
    //Serial.println("Start Counting");
    this->endcounterpush = millis();
    this->buttonpushed = false;
    this->duration = endcounterpush - startcounterpush;
    SjoelBakDisplay.CountTime(this->duration, Game.GetCurrentPlayer());
    };

  if (this->measuredState == LOW and startpushing) {
    this->endcounterpush = millis();
    this->startpushing = false;
    this->buttonpushed = true;
    DEBUG_PRINT("Buttonreleased");
    this->duration = endcounterpush - startcounterpush;
    SjoelBakDisplay.CountTime(this->duration, Game.GetCurrentPlayer());
    this->CheckAction();
  };

  Game.SetButtonPushed(this->startpushing);
};


void ResetButton::CheckAction() {
  DEBUG_PRINT("CheckAction");
  DEBUG_PRINT(this->duration);
  bool BREAK = false;
  if (this->duration >= STARTGAME && !BREAK) {
    DEBUG_PRINT("Reset Game Press");
    Game.ResetGame();

    BREAK = true;
  }
  if (this->duration >= ADDPLAYERDURATION && !BREAK) {
    DEBUG_PRINT("Add Player Duration Press");
    if (Game.ReturnState() == ONGOING) {
      Game.SetState(PLAYING);
    }
    BREAK = true;
  }
  if (this->duration >= SHORTPRESS && !BREAK) {
    DEBUG_PRINT("ShortPRess");
    if (Game.ReturnState() == PLAYING) {
      DEBUG_PRINT("Next Player");
      Game.NextPlayer();
    }
    if (Game.ReturnState() == END) {
      Game.EndGame();
    }
    if (Game.ReturnState() == ONGOING) {
      this->amountPlayers++;
      if (this->amountPlayers > MAXPLAYERS) {
        this->amountPlayers = 1;
      }
      Game.SetAmountPlayers(this->amountPlayers);
      DEBUG_PRINT(this->amountPlayers);
      SjoelBakDisplay.ShowOngoingAmountPlayers(this->amountPlayers);
    }
    BREAK = true;
  };
  this->buttonpushed = false;
};

void ResetButton::FullReset() {
  this->duration = 0;
  this->measuredState = HIGH;
  this->previousmeasuredState = HIGH;
  this->amountPlayers = 1;
  this->resetState = fullReset;
};


// SET_UP OBJECTS

Hole HoleArray[AMOUNTHOLES] = {
  Hole(BeamPins[0], Points[0], Debug),
  Hole(BeamPins[1], Points[1], noDebug),
  Hole(BeamPins[2], Points[2], noDebug),
  Hole(BeamPins[3], Points[3], noDebug),
  Hole(BeamPins[4], Points[4], noDebug),
  Hole(BeamPins[5], Points[5], noDebug),
  Hole(BeamPins[6], Points[6], noDebug),
  Hole(BeamPins[7], Points[7], noDebug),
  Hole(BeamPins[8], Points[8], noDebug),
  Hole(BeamPins[9], Points[9], noDebug)
};


ResetButton SjoelBakResetButton(RESETBUTTON);


void setup() {

  Serial.begin(115200);
  Serial.println("Booting");
  SjoelBakDisplay.Init();
  SjoelBakResetButton.Init();
  Game.Init();
  for (int x = 0; x < AMOUNTHOLES; x++) {
    HoleArray[x].Init();
  };
  Game.ResetGame();
}

void measureAll() {
  for (x = 0; x < AMOUNTHOLES; x++) {
    HoleArray[x].checkBeam();
  };
  SjoelBakResetButton.Measure();
  if (Game.ReturnState() == PLAYING) {
    SjoelBakDisplay.BlinkingPlayer(Game.GetCurrentPlayer(), Game.GetScoreCurrentPlayer(), Game.GetButtonPushed());
  }
  if (Game.ReturnState() == END) {
    SjoelBakDisplay.WinningPlayer(Game.GetWinningPlayer(), Game.GetWinningPlayerScore(), Game.GetButtonPushed());
  }


}

void loop() {
  measureAll();

}
