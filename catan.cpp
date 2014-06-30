#include "catan.h"
#include "UI.h"

//Draw each UI element
void drawGame()
{
  drawElement("res/tradebox",tradeBoxLoc);
  drawElement("res/tipsbox",tipsBoxLoc);
  //drawElement("res/tipsboxmonopoly",tipsBoxLoc);
  drawElement("res/catanboard", boardLoc);
  drawElement("res/devcardsbox",devCardsBoxLoc);
  drawElement("res/playerbox",playerBoxLoc);
  drawElement("res/resourcebox", resourceBoxLoc);
  drawElement("res/donebox", doneBoxLoc);
  //drawElement("res/nodonebox", doneBoxLoc);
  drawElement("res/buildingbox",buildBoxLoc);
  //drawElement("res/onfirmbox",confirmBoxLoc);
  drawElement("res/noconfirmbox",confirmBoxLoc);
}

int Player::getResourceCount(int resource)
{
  return resources[resource];
}

int Player::setResourceCount(int resource, int amount)
{
  total += amount - resources[resource];
  resources[resource] = amount;
  move(resourceLoc.y + resource, resourceLoc.x);

  string out = "";
  if(amount < 10)
    out += " " + SSTR(amount);
  else
    out += SSTR(amount);
  addstr(out.c_str());
  move(resourceLoc.y + 6, resourceLoc.x);
  out = "";
  if(total < 10)
    out += " " + SSTR(total);
  else
    out += SSTR(total);
  addstr(out.c_str());
  c->drawPlayerBox();
  refresh();
}

int Player::addResource(int resource, int amount)
{
  setResourceCount(resource, getResourceCount(resource) + amount);
}

Player::Player()
{
  total = 0;
  VPs = 0;
  tradingWith = -1;
  confirmBoxWaiting = false;
  YesAction = DoNothing;
  for(int i = 0; i < 5; i++)
  {
    resources[i] = 0;

    //setResourceCount(i, 0);
  }
}

CatanGame::CatanGame()
{
  numPlayers = 0;
  end = false;
  for(int i = 0; i < 5; i++) resourceCount[i] = 19;
  turn = 0;
}

int CatanGame::getActivePlayer()
{
  if(localGame)
    return turn;
  else
    return localPlayerNum;
}

void CatanGame::nextTurn()
{
  turn++;
  if(turn >= numPlayers)
    turn = 0;
  testPrint(SSTR(turn) + "        ");

  if(!localGame)
    if(turn == c->getActivePlayer())
      drawElement("res/donebox",doneBoxLoc);
    else
      drawElement("res/nodonebox",doneBoxLoc);
  c->drawPlayerBox();
}


void CatanGame::drawPlayerBox()
{
  for(int i = 0; i < numPlayers; i++)
  {
    if(i == turn)
    {
      attron(A_STANDOUT);
    }
    move(playerBoxLoc.y + 3*i + 3, playerBoxLoc.x);
    addstr(center(player[i].name,playerBoxWidth));
    move(playerBoxLoc.y + 3*i + 4, playerBoxLoc.x);
    string s = SSTR(player[i].total);
    if(player[i].total == 1)
      s += " Card";
    else
      s += " Cards/";

    s += SSTR(player[i].VPs);
    if(player[i].VPs == 1)
      s += " VP";
    else
      s += " VPs";

    addstr(center(s, playerBoxWidth));
    attroff(A_STANDOUT);
  }
  move(playerBoxLoc.y + 3*numPlayers + 3,playerBoxLoc.x);
  addstr(center("(click to trade)",playerBoxWidth));
}

void CatanGame::playGame()
{
  MEVENT event;
  timeout(0);
  while(!end) //Forever.
  {
    int ch = wgetch(stdscr);  //Get input.
    if(ch == '=')             //End the game on =
      end = true;
    if(ch == KEY_MOUSE) 
    {
      if(getmouse(&event) == OK)  
      {
        if(event.bstate & BUTTON1_CLICKED) //If it's a mouse click
        {
          move(0,0);  //Print the clicked coordinates (testing)
          string s = SSTR(event.y) + " " + SSTR(event.x) + "   ";
          addstr(s.c_str());
          refresh();
          handleClick(event.y, event.x);  //And handle the click
        }
      }
    }
    //Get network input here  
  }
}

int main()
{
  //Initial setup
  c = new CatanGame();
  while(c->numPlayers < 1 || c->numPlayers > 4)
  {
    cout << "How many players? (1-4): ";
    cin >> c->numPlayers;
  }
  c->player = new Player[c->numPlayers];
  for(int i = 0; i < c->numPlayers; i++)
  {
    cout << "Enter player " << i << "'s name: ";
    cin >> c->player[i].name;
  }
  
  initscr();  //Start the ncurses screen
  int y, x;   //Get the size of the term for debugging
  getmaxyx(stdscr, y, x);
  endwin(); 
  cout << "Terminal is " << y << "x" << x << endl;
  if(y < 40 || x < 141) //And refuse to run if it's too small >:D
  {
    cout << "Requires a larger screen!" << endl;
    return 0;
  }
  noecho();       //Don't print input automatically
  cbreak();       //Feed input with minor processing
  start_color();  //Allow color usage
  curs_set(0);    //Make the cursor inisible
  
  keypad(stdscr, TRUE); //Allow keypad/mouse events
  mousemask(ALL_MOUSE_EVENTS, NULL);  //Read all mouse events

  clear();  //Clear the screen

  //Pair foreground and background colors
  init_pair(1, COLOR_RED,    COLOR_BLACK);
  init_pair(2, COLOR_GREEN,  COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE,   COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA,COLOR_BLACK);
  init_pair(6, COLOR_CYAN,   COLOR_BLACK);
  init_pair(7, COLOR_WHITE,  COLOR_BLACK);

  c->tiles = new vector<Tile*>;
  vector<coord> tileLocs = buildTileWindows(); //Associate each tile with a coordinate
  
  drawGame();                   //Draw the game elements
  c->drawPlayerBox();
  RandTile rt;                  //Generate the random resource set and the ordered chip set
  OrderedValues ov;
  int tileOrder = rand() % 12;  //Pick a random outer-tile to start placing chips on
  for(int i = 0; i < 19; i++)   //And place a random chip + random resource on each spot
  {
    string resource = rt.pop();
    if(resource == "")          //Except the desert!  Fuck the desert.
      continue;
    string value = ov.pop();
    Tile* t = new Tile(&tileLocs[i],value,resource);
    c->tiles->push_back(t);
    printTile(t);
    //fillTile(tiles[tilePlaceOrder[tileOrder][i]], value, resource); //Fill each tile with its stuff
  }

  c->playGame();


  //Cleanup:
  curs_set(1);  //Make the cursor visible again
  endwin();     //Close the ncurses window
  clear();      //And clear it?? (idk, but it works so I'm not gonna fuck with it)
  return 0;     //Aaaand, done.
}
