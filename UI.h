#ifndef UI_H
  #define UI_H

#include "UIBase.h"

//GLOBALS
CatanGame* c;
string host = "localhost";
bool isHost = false;
string name = "Kalrax";
int port = 11232;
int socketNum;
bool localGame = false;
int playerBoxWidth = 15;
const int numClickableElements = 14; //Number of interactive UI elements

void sendstr(string s);

void printToConfirmBox(string s)
{
  move(confirmBoxLoc.y + 2, confirmBoxLoc.x + 1);
  s = center(s,23);
  addstr(s.c_str());
}

void printToTradeBox(string s)
{
  move(tradeBoxLoc.y + 2, tradeBoxLoc.x + 1);
  s = center(s,45);
  addstr(s.c_str());
}

void updateTradeResource(int resource, int value, bool localPlayer)
{
  int xloc = 6;
  if(!localPlayer)
    xloc = 29;
  
  if(value < 10)
  {
    move(tradeBoxLoc.y + 7 + resource, tradeBoxLoc.x + xloc);
  }
  else
    move(tradeBoxLoc.y + 7 + resource, tradeBoxLoc.x + xloc - 1);

  addstr(SSTR(value).c_str()); 
}

void engageTrade(int target)
{
  if(target == c->getActivePlayer() || target + 1 > c->numPlayers) //Do nothing if you click yourself or a nonplaying player
    return;
    //This needs a total overhaul
  else if(c->player[target].tradingWith == -1)
  {
    printToTradeBox("Trade with " + c->player[target].name);
  }
}

//Yes, this is actually important.
void DoNothing(){}

void confirmAction(string s, void (*success)())
{
  c->player[c->getActivePlayer()].confirmBoxWaiting = true;
  attron(A_BOLD);
  drawElement("res/confirmbox",confirmBoxLoc);
  attron(A_STANDOUT);
  printToConfirmBox(s);
  attroff(A_STANDOUT);
  attroff(A_BOLD);
  c->player[c->getActivePlayer()].YesAction = success;
}

//Functions ran when each UI element is clicked
void YesClicked(int y, int x)
{
  testPrint("Yes         ");
  if(c->player[c->getActivePlayer()].confirmBoxWaiting)
  {
    drawElement("res/noconfirmbox",confirmBoxLoc);
    c->player[c->getActivePlayer()].confirmBoxWaiting = false;
    c->player[c->getActivePlayer()].YesAction(); 
    c->player[c->getActivePlayer()].YesAction = DoNothing;
  }
}
void NoClicked(int y, int x)
{
  testPrint("No          ");
  if(c->player[c->getActivePlayer()].confirmBoxWaiting)
  {
    drawElement("res/noconfirmbox",confirmBoxLoc);
    c->player[c->getActivePlayer()].confirmBoxWaiting = false;
    c->player[c->getActivePlayer()].YesAction = DoNothing;
  }
}
void SettlementClicked(int y, int x)  
{
  testPrint("Settlement  ");
}
void CityClicked(int y, int x)
{
  testPrint("City        ");
}
void RoadClicked(int y, int x)
{
  testPrint("Road        ");
}
void DevCardClicked(int y, int x)
{
  testPrint("Dev. Card   ");
}
void YouReadyClicked(int y, int x)
{
  testPrint("YouReady    ");
}
void CancelClicked(int y, int x)
{
  testPrint("Cancel      ");
}
void UseDevCardClicked(int y, int x)
{
  int i = y - devCardsBoxLoc.y - 3;
  string s = SSTR(i);
  s = "Dev" + s + "        ";
  testPrint(s.c_str());
}
void EndTurnYes()
{
  sendstr("E");
  //c->nextTurn();
  //broadcast("NextTurn");
}
void EndTurnClicked(int y, int x)
{
  testPrint("EndTurn     ");
  if(c->getActivePlayer() == c->turn)
    confirmAction("End turn?", EndTurnYes);
}
void P0Clicked(int y, int x)
{
  testPrint("P0          ");
  engageTrade(0);
}
void P1Clicked(int y, int x)
{
  testPrint("P1          ");
  engageTrade(1);
}
void P2Clicked(int y, int x)
{
  testPrint("P2          ");
  engageTrade(2);
}
void P3Clicked(int y, int x)
{
  testPrint("P3          ");
  engageTrade(3);
}

//Bounds of each clickable area, and associated functions
Box Settlement(buildBoxLoc.y + 5, buildBoxLoc.y + 10, buildBoxLoc.x + 1, buildBoxLoc.x + 22, SettlementClicked);
Box City(buildBoxLoc.y + 11, buildBoxLoc.y + 16, buildBoxLoc.x + 1, buildBoxLoc.x + 22, CityClicked);
Box Road(buildBoxLoc.y + 17, buildBoxLoc.y + 22, buildBoxLoc.x + 1, buildBoxLoc.x + 22, RoadClicked);
Box DevCard(buildBoxLoc.y + 23, buildBoxLoc.y + 28, buildBoxLoc.x + 1, buildBoxLoc.x + 22, DevCardClicked);
Box YouReady(tradeBoxLoc.y + 13, tradeBoxLoc.y + 15, tradeBoxLoc.x + 1, tradeBoxLoc.x + 15, YouReadyClicked);
Box Cancel(tradeBoxLoc.y + 13, tradeBoxLoc.y + 15, tradeBoxLoc.x + 17, tradeBoxLoc.x + 29, CancelClicked);
Box UseDevCard(devCardsBoxLoc.y + 3, devCardsBoxLoc.y + 7, devCardsBoxLoc.x + 1, devCardsBoxLoc.x + 17, UseDevCardClicked);
Box EndTurn(doneBoxLoc.y + 1, doneBoxLoc.y + 4, doneBoxLoc.x + 1, doneBoxLoc.x + 8, EndTurnClicked);
Box P0(playerBoxLoc.y + 3, playerBoxLoc.y + 4, playerBoxLoc.x, playerBoxLoc.x + playerBoxWidth - 1, P0Clicked);
Box P1(playerBoxLoc.y + 6, playerBoxLoc.y + 7, playerBoxLoc.x, playerBoxLoc.x + playerBoxWidth - 1, P1Clicked);
Box P2(playerBoxLoc.y + 9, playerBoxLoc.y + 10, playerBoxLoc.x, playerBoxLoc.x + playerBoxWidth - 1, P2Clicked);
Box P3(playerBoxLoc.y + 12, playerBoxLoc.y + 13, playerBoxLoc.x, playerBoxLoc.x + playerBoxWidth - 1, P3Clicked);
Box Yes(confirmBoxLoc.y + 3, confirmBoxLoc.y + 5, confirmBoxLoc.x + 4, confirmBoxLoc.x + 8, YesClicked);
Box No(confirmBoxLoc.y + 3, confirmBoxLoc.y + 5, confirmBoxLoc.x + 17, confirmBoxLoc.x + 20, NoClicked);

//List of all clickable elements
Box clickable[numClickableElements] = {Settlement,City,Road,DevCard,YouReady,Cancel,UseDevCard,EndTurn,P0,P1,P2,P3,Yes,No};

//Run through all clickable elements, execute if the sent click matches its bounds
void handleClick(int y, int x)
{
  for(int i = 0; i < numClickableElements; i++)
  {
    if(y <= clickable[i].bottom && 
       y >= clickable[i].top && 
       x >= clickable[i].left && 
       x <= clickable[i].right)
    {
      if(!c->player[c->getActivePlayer()].confirmBoxWaiting || clickable[i].f == YesClicked || clickable[i].f == NoClicked)
        clickable[i].f(y, x);
      return;
    }
  }
}

#endif
