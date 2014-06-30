#ifndef CATAN_H
  #define CATAN_H

#include<iostream>
#include<curses.h>
#include<fstream>
#include<vector>
#include<sstream>
#include<algorithm>
using namespace std;

//Globals
int localPlayerNum = 0;

//Player stats.
class Player
{
public:
  string name;
  //{"Brick", "Ore", "Sheep", "Wheat", "Wood"};
  int resources[5]; //How many of each resource they have
  int total;        //Running total resource count
  int VPs;          //How many victory points they have
  int tradingWith;  //Who they're currently trading with, -1 for nobody
  bool confirmBoxWaiting; //If there's an 'are you sure' box left unanswered
  void (*YesAction)();

  //Get the number of resources of a type the player has
  int getResourceCount(int resource);
  
  //Set the players resources of one type to amount, updating total
  int setResourceCount(int resource, int amount);

  //adds amount resources of a type.  can be negative.
  int addResource(int resource, int amount);

  Player();
};

class Tile;

class CatanGame
{
public:
  int numPlayers;
  bool end;
  Player* player;
  int resourceCount[5];
  int turn;

  CatanGame();
  vector<Tile*>* tiles;
  int getActivePlayer();
  void nextTurn();
  void drawPlayerBox();
  void playGame();
};
#endif
