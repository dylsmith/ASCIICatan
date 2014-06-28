#ifndef BASE_H
  #define BASE_H

using namespace std;

//Simple coordinates used all over the place
class coord 
{
public:
  int y;
  int x;


  inline coord& operator+=(int rhs)
  {
    y += rhs;
    return *this;
  }
  inline coord& operator++()
  {
    y = y + 1;
    return *this;
  }
  coord(int yy, int xx)
  {
    y = yy;
    x = xx;
  }
};

//GLOBALS
#define BOx boardLoc.x  //I spam these in one function, this makes it readable
#define BOy boardLoc.y
bool localGame = false;
CatanGame* c;
int playerBoxWidth = 15;
const int numResource[5] = {3, 3, 4, 4, 4}; // The number of brick, ore, sheep, wheat, wood to create
const string resourceName[5] = {"Brick", "Ore", "Sheep", "Wheat", "Wood"};  //The names of the resources (<= 5 chars pls)
const int numClickableElements = 14; //Number of interactive UI elements

int tilePlaceOrder[12][19] ={{0,2,5,10,15,17,18,16,13,8,3,1,  4,7,12,14,11,6,  9},//All the different
                             {2,5,10,15,17,18,16,13,8,3,1,0,  4,7,12,14,11,6,  9},//ways to place the
                             {5,10,15,17,18,16,13,8,3,1,0,2,  7,12,14,11,6,4,  9},//chips
                             {10,15,17,18,16,13,8,3,1,0,2,5,  7,12,14,11,6,4,  9},
                             {15,17,18,16,13,8,3,1,0,2,5,10,  12,14,11,6,4,7,  9},
                             {17,18,16,13,8,3,1,0,2,5,10,15,  12,14,11,6,4,7,  9},
                             {18,16,13,8,3,1,0,2,5,10,15,17,  14,11,6,4,7,12,  9},
                             {16,13,8,3,1,0,2,5,10,15,17,18,  14,11,6,4,7,12,  9},
                             {13,8,3,1,0,2,5,10,15,17,18,16,  11,6,4,7,12,14,  9},
                             {8,3,1,0,2,5,10,15,17,18,16,13,  11,6,4,7,12,14,  9},
                             {3,1,0,2,5,10,15,17,18,16,13,8,  6,4,7,12,14,11,  9},
                             {1,0,2,5,10,15,17,18,16,13,8,3,  6,4,7,12,14,11,  9}};
//OFFSETS - where each UI element sits on screen. 
coord boardLoc(0,54);   
coord resourceBoxLoc(1,3);
coord buildBoxLoc(1,117);
coord tipsBoxLoc(33, 51);
coord tradeBoxLoc(24, 1);
coord playerBoxLoc(1,25); 
coord doneBoxLoc(35,105);
coord devCardsBoxLoc(14,1);
coord confirmBoxLoc(18,23);

//Auto-calculated coordinates
coord resourceLoc(resourceBoxLoc.y + 4,resourceBoxLoc.x);


void move(coord c)  //Move to coordinate
{
  move(c.y, c.x);
}

//Center a string, if given space len
const char* center(string s, int len = 5)
{
  int diff = len - s.length();
  if (diff % 2 == 1)
  {
    s += " ";
    diff--;
  }
  diff = diff / 2;
  for(int i = 0; i < diff; i++)
    s = " " + s + " ";
  return s.c_str();
}

void testPrint(string s)  //Temp, prints to top left
{
  attron(A_BOLD);
  move(1,0);
  addstr(s.c_str());
  attroff(A_BOLD);
  refresh();
}

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


//Copy the contents of a file, starting at the given coord
void drawElement(string filename, coord offset)
{
  ifstream inf;
  inf.open(filename.c_str());
  if(!inf.good())
    testPrint("FILE NOT FOUND");
  string s;
  int line = offset.y;
  while(getline(inf,s))
  {
    move(line++, offset.x);
    addstr(s.c_str());
  }
}


//Cast stuff to a string
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

class Box //Used to create clickable areas
{
  public:
  int top, left, bottom, right;
  void (*f)(int, int);  //Function that gets ran when the user clicks in these bounds
  Box(int t, int b, int l, int r, void (*fu)(int, int))
  {
    top = t;
    left = l;
    bottom = b;
    right = r;
    f = fu;
  }
};

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
  c->nextTurn();
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

//English-to-colorcode translator
int color(string c)
{
  if(c == "red" || c == "Brick")
    return COLOR_PAIR(1);
  else if(c == "blue")
    return COLOR_PAIR(4);
  else if(c == "white" || c == "Sheep")
    return COLOR_PAIR(7);
  else if(c == "green" || c == "Wood")
    return COLOR_PAIR(2);
  else if(c == "yellow" || c == "Wheat")
    return COLOR_PAIR(3);
  else if(c == "cyan" || c == "teal" || c == "Ore")
    return COLOR_PAIR(6);
  else if(c == "magenta" || c == "pink")
    return COLOR_PAIR(5);
}

//Takes tile coordinates, a weight, and a resource name, and prints it nicely
void fillTile(coord tile, string val, string resource)
{
  move(tile);
  if(resource.length() <= 6)
  {
    attron(color(resource));  //Print the resource in its color :D
    addstr(center(resource));
    attroff(color(resource));
    move(tile += 2);

    attron(A_BOLD);
    addstr(center(val));
    move(tile += 1);
    
    if(val == "2" || val == "12") //Print the handy-dandy little dots, red if necessary
      addstr(center("."));
    else if(val == "3" || val == "11")
      addstr(center(".."));
    else if(val == "4" || val == "10")
      addstr(center("..."));
    else if(val == "5" || val == "9")
      addstr(center("...."));
    else if(val == "6" || val == "8")
    {
      attron(color("red"));
      addstr(center("....."));
      attroff(color("red"));
    }
    attroff(A_BOLD);
  }
  else
  {
    cout << "Resource name too long!" << endl;
  }
}

//Adds the tiles by row to a vector, for easy translation between tile and coordinates
vector<coord> buildTileWindows()
{
  vector<coord> tiles;
  tiles.push_back(coord(BOy + 2, BOx + 24));
  
  tiles.push_back(coord(BOy + 5, BOx + 14));
  tiles.push_back(coord(BOy + 5, BOx + 34));
  
  tiles.push_back(coord(BOy + 8, BOx + 4));
  tiles.push_back(coord(BOy + 8, BOx + 24));
  tiles.push_back(coord(BOy + 8, BOx + 44));
  
  tiles.push_back(coord(BOy + 11, BOx + 14));
  tiles.push_back(coord(BOy + 11, BOx + 34));
  
  tiles.push_back(coord(BOy + 14, BOx + 4));
  tiles.push_back(coord(BOy + 14, BOx + 24));
  tiles.push_back(coord(BOy + 14, BOx + 44));
  
  tiles.push_back(coord(BOy + 17, BOx + 14));
  tiles.push_back(coord(BOy + 17, BOx + 34));
  
  tiles.push_back(coord(BOy + 20, BOx + 4));
  tiles.push_back(coord(BOy + 20, BOx + 24));
  tiles.push_back(coord(BOy + 20, BOx + 44));

  tiles.push_back(coord(BOy + 23, BOx + 14));
  tiles.push_back(coord(BOy + 23, BOx + 34));
  
  tiles.push_back(coord(BOy + 26, BOx + 24));
  
  return tiles;
}

//Random number.  Needed for shuffle.
int myrand(int i)
{
  return rand()%i;
}

//The standard alphabetized list of chips
class OrderedValues
{
  vector<string> values;
public:
  string pop()  //Return and remove the top chip
  {
    string ret = values.front();
    values.erase(values.begin());
    return ret;
  }

  OrderedValues()
  {
    values.push_back("5");
    values.push_back("2");
    values.push_back("6");
    values.push_back("3");
    values.push_back("8");
    values.push_back("10");
    values.push_back("9");
    values.push_back("12");
    values.push_back("11");
    values.push_back("4");
    values.push_back("8");
    values.push_back("10");
    values.push_back("9");
    values.push_back("4");
    values.push_back("5");
    values.push_back("6");
    values.push_back("3");
    values.push_back("11");
    values.push_back("");
  }
};

//Generates a random-ordered set of resources
class RandTile
{
  vector<string> tiles;
public: 
  string pop() //Return and remove the top resource
  {
    string ret = tiles.front();
    tiles.erase(tiles.begin());
    return ret;
  }

  RandTile()  //Pushes the appropriate number of each tile to a vector, and shuffles it
  {
    for(int i = 0; i < 5; i++)
    {
      for(int k = 0; k < numResource[i]; k++)
      {
        tiles.push_back(resourceName[i]);
      }
    }
    tiles.push_back("");
    srand(time(0));
    random_shuffle(tiles.begin(), tiles.end(), myrand);
  }
};

#endif
