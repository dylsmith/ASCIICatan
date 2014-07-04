#ifndef UIBASE_H
  #define UIBASE_H

#define BOx boardLoc.x  //I spam these in one function, this makes it readable
#define BOy boardLoc.y

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

void move(coord c)  //Move to coordinate
{
  move(c.y, c.x);
}

void move(coord* c)  //Move to coordinate
{
  move(c->y, c->x);
}
class Tile
{
public:
  coord* offset;
  coord* adjacent;
  string value;
  string resource;

  Tile(coord* offsetVal, string valueVal, string resourceVal)
  {
    offset = offsetVal;
    value = valueVal;
    resource = resourceVal;
  }
};

//UI Constants
const int numResource[5] = {3, 3, 4, 4, 4}; // The number of brick, ore, sheep, wheat, wood to create
const string resourceName[5] = {"Brick", "Ore", "Sheep", "Wheat", "Wood"};  //The names of the resources (<= 5 chars pls)

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

//Cast stuff to a string
#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

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

void testPrint(string s, int i = 1)  //Temp, prints to top left
{
  attron(A_BOLD);
  move(i,0);
  addstr("                   ");
  move(i,0);
  addstr(s.c_str());
  attroff(A_BOLD);
  refresh();
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
void printTile(Tile* tile)
{
  move(tile->offset);
  if(tile->resource.length() <= 6)
  {
    attron(color(tile->resource));  //Print the resource in its color :D
    addstr(center(tile->resource));
    attroff(color(tile->resource));
    move(tile->offset->y + 2, tile->offset->x);

    attron(A_BOLD);
    addstr(center(tile->value));
    move(tile->offset->y + 3, tile->offset->x);
    
    if(tile->value == "2" || tile->value == "12") //Print the handy-dandy little dots, red if necessary
      addstr(center("."));
    else if(tile->value == "3" || tile->value == "11")
      addstr(center(".."));
    else if(tile->value == "4" || tile->value == "10")
      addstr(center("..."));
    else if(tile->value == "5" || tile->value == "9")
      addstr(center("...."));
    else if(tile->value == "6" || tile->value == "8")
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
