#include "catan.h"
#include "UI.h"
#include "comms.h"

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
  c->resourceCount[resource] += resources[resource] - amount;  //The bank loses this many of that resource. -1 * number being added
  total += amount - resources[resource];  //Update player total
  resources[resource] = amount; //Set the amount
  move(resourceLoc.y + resource, resourceLoc.x);  //Move to where we're going to write 

  string out = "";
  if(amount < 10) //if it's a single number, print it right-justified
    out += " " + SSTR(amount);
  else
    out += SSTR(amount);
  addstr(out.c_str());
  move(resourceLoc.y + 6, resourceLoc.x);
  out = "";
  if(total < 10)  //If it's double-digit, don't add that space
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
  total = 0;    //0 resources, 0 VPs, not trading with anyone, not waiting to confirm something, clicking yes will do nothing...
  VPs = 0;
  tradingWith = -1;
  confirmBoxWaiting = false;
  YesAction = DoNothing;
  for(int i = 0; i < 5; i++)
    resources[i] = 0;
}

CatanGame::CatanGame()
{
  numPlayers = 0;
  for(int i = 0; i < 5; i++) 
    resourceCount[i] = 19;
  turn = -1;
}

int CatanGame::getActivePlayer()  //Returns the player who has the screen.  For online games this is always the same player, but changes locally.
{
  if(localGame)
    return turn;
  else
    return localPlayerNum;
}

void CatanGame::nextTurn()
{
  turn++; //Increment the turn counter
  if(turn >= numPlayers)  //Go back to 0 if we've let everyone go
    turn = 0;

  if(!localGame)  //If this game is online
    if(turn == c->getActivePlayer())  //Draw the done box for whoever's turn it is
      drawElement("res/donebox",doneBoxLoc);
    else  //And clear it for everyone else
      drawElement("res/nodonebox",doneBoxLoc);
  c->drawPlayerBox(); //And update the player box
}

void endGame()
{
  curs_set(1);  //make the cursor visible again
  endwin();     //close the ncurses window
  clear();      //and clear it?? (idk, but it works so i'm not gonna fuck with it)
  if(isHost)  //Close all ports
    for(list<int>::iterator it = clientList->begin(); it != clientList->end(); it++)
      close(*it);
  else
    close(socketNum);
  exit(0);
}

void CatanGame::drawPlayerBox()
{
  for(int i = 0; i < numPlayers; i++) //For every player
  {
    if(i == turn) //If it's their turn, highlight them
      attron(A_STANDOUT);

    move(playerBoxLoc.y + 3*i + 3, playerBoxLoc.x); //Write the player's name, centered
    addstr(center(player[i].name,playerBoxWidth));
    move(playerBoxLoc.y + 3*i + 4, playerBoxLoc.x); //And move to the line below it
    string s = SSTR(player[i].total); //string s will be <numcards> <Card/Cards>/<numVPs> <VP/VPs>

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
  addstr(center("(click to trade)",playerBoxWidth));  //Draw the (click to trade) text after all the players
}

void handleMessage(Message* m) //Handles every message broadcasted, for every player.
{
  char ch;
  string s;
  switch(m->type)
  {
    case SOCKCLOSED:
      endGame();
      break;

    case PLAYERNUMBER:
      localPlayerNum = m->msg[0];
      //testPrint("num is " + SSTR(localPlayerNum));
      s = SSTR(localPlayerNum) + name + " " + SSTR(localPlayerNum);
      sendstr(PLAYERNAME, s);
      break;

    case REQUESTPLAYERNAME:
      sendstr(PLAYERNAME, name);
      break;

    case PLAYERNAME:
      ch = m->msg[0] - 48;
      s = m->msg.substr(1,m->length - 1);
      c->player[ch].name = s;
      c->drawPlayerBox();
      //testPrint(SSTR(ch) + " : " + c->player[ch].name, 30);
      break;
  
    case ENDTURN:
      testPrint("ENDING",4);
      c->nextTurn();
      break;

    case TEST:
      testPrint("Got " + m->msg);
  }
  /*
  if(m.type == SOCKCLOSED)
  {
    endGame();
  }
  else if(m.msg[0] == 'N') //N == playername being broadcasted
  {
    c->player[m.msg[2]].name = m.msg.substr(3, m.msg.length); //Extract playername
    c->drawPlayerBox(); //And redraw the player box
  }
  else if(m.msg[0] == 'E')  //E == End turn
    c->nextTurn();  //Goto next turn
  else if(m.msg[0] == 'P')
  {
    localPlayerNum = m.msg[1];
    testPrint("I am player " + m.msg[1], 10);
  }
  testPrint("               ");
  testPrint("Got " + m.msg);*/
}
bool firstLoop = true;
void CatanGame::playGame()
{
  MEVENT event;
  timeout(0);
  while(1) //Forever.
  {
    int ch = wgetch(stdscr);  //Get input.
    if(ch == '=')             //End the game on =
      endGame();
    else if(ch == KEY_MOUSE) 
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
          sendstr(TEST, SSTR(event.y) + " " + SSTR(event.x));
        }
      }
    }

    trysend();
    Message* m = getMessage(); //And try to get something to add to the received queue
    //testPrint(SSTR(recvBuffer->size()), 6);
    if(m)
    {
      if(isHost)
        broadcastMessage(m);
      handleMessage(m);
    }
  }
}

int main()
{
  //Initial setup
  c = new CatanGame();
  c->numPlayers = 3;
  while(c->numPlayers < 1 || c->numPlayers > 4)
  {
    cout << "How many players? (1-4): ";
    cin >> c->numPlayers;
  }
  c->player = new Player[c->numPlayers];

  if(localGame)
  {
    for(int i = 0; i < c->numPlayers; i++)
    {
      cout << "Enter player " << i << "'s name: ";
      cin >> c->player[i].name;
    }
  }
  else
  {
    //cout << "Player num: ";
    //cin >> localPlayerNum;
    //We get player names from the internets
  }

  string temp;
  cout << "Hosting? "; //read from a file pls
  cin >> temp;
  if(temp == "y")
  {
    isHost = true;
  }
  if(isHost) 
  {
    localPlayerNum = 0;
    int n = getClients(c->numPlayers - 1);
    if(n == -1)
      return 0;
    
    char i = 0;
    //Distribute player numbers here somehow
    for(list<int>::iterator it = clientList->begin(); it != clientList->end(); it++)
    {
      i++;
      Message m(PLAYERNUMBER, SSTR(i));
      write(*it, m.tochars(), m.length + 2);
    }
    string s = SSTR(localPlayerNum) + name + " " + SSTR(localPlayerNum);
    sendstr(PLAYERNAME, s);
    sendstr(ENDTURN);
  }
  else
  {
    localPlayerNum = 1;
    socketNum = connectToHost();
    if(socketNum == -1)
      return 0;
  }
  if(isHost)
  {
    //sendstr("E");
  }
  //sendstr("NKalrax " + SSTR(localPlayerNum));
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
  }
  
  c->playGame();


  //Cleanup:
  curs_set(1);  //make the cursor visible again
  endwin();     //close the ncurses window
  clear();      //and clear it?? (idk, but it works so i'm not gonna fuck with it)
  return 0;     //Aaaand, done.
}
