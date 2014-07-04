#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <list>
#include "catan.h"
using namespace std;

list<Message>* recvBuffer;;
list<Message>* sendBuffer;
class Message //Exchanged between online players
{
public:
  char sender;  //Who sent the message
  string msg;   //String value
  
  const char* c_str() //convert a message to a series of bytes.  The first is length, second is sender, the rest are the message
  {
    char* p = new char[sizeof(char) * (msg.length() + 2)];
    p[0] = msg.size();
    p[1] = sender;
    for(int i = 2; i < msg.size() + 2; i++)
    {
      p[i] = msg[i - 2];
    }
    return p;
  }

  Message(char *p)  //Build a message from a char*, opposite of c_str()
  {
    msg = "";
    sender = p[1];
    for(int i = 2; i < p[0] + 2; i++)
    {
      msg += p[i];
    }
  }
  Message(string s) //Build a quick message from a string, to pass things like "CLOSED" which don't have a sender
  {
    msg = s;
    sender = -1;
  }
  Message(string s, char i) //Build a message from a string and a sender given
  {
    sender = i;
    msg = s;
  }
};

list<int>* clientList;
void handleMessage(Message m);

void broadcastMessage(Message m)  //Sends from a host to all others.  Don't call this directly. 
{
  for(list<int>::iterator client = clientList->begin(); client != clientList->end(); client++)
    int n = write(*client, m.c_str(), m.msg.length() + 2);
}

void trysend()  
{
  Message m = sendBuffer->front();  //Get the first message needed to be send
  sendBuffer->pop_front();  //And remove it from the send buffer
  broadcastMessage(m);  //And send it to everyone
  if(isHost)  //If we're the host, handle it locally as well. 
    handleMessage(m);
}

void sendstr(string s)  //The ultimate function in this file.  Queues a string to be sent to and executed by every player!
{
  Message m (s, localPlayerNum);
  sendBuffer->push_back(m);
}

void endGame();
void getMessage()
{
  for(list<int>::iterator client = clientList->begin(); client != clientList->end(); client++)  //For every thing we can receive from
  {      
    char buffer[256]; //Make a 256-char buffer and zero it out
    bzero(buffer, 256);
    int n = recv((*client), buffer, 256, 0);  //Receive from that socket
    Message m(buffer);  //And make a message out of what we receive

    if(n > 0 && m.msg.length() > 0 && m.msg != "")  //If it's a valid message
    {
      //testPrint("Valid received: " + SSTR(TEMP++),4);
      recvBuffer->push_back(m); //Add it to the received buffer
    }
    else if(n == 0) //If it's a socket-closed message
    {
      close(*client); //Close the socket on our side
      clientList->erase(client);  //And remove the client from the list of clients
      if(clientList->size() == 0) //if the list is now empty, end the game for this player
      {
        recvBuffer->push_back(Message((string)"CLOSED"));
        endGame();
      }
    }
  }
}

int getClients(int numClients)
{
  recvBuffer = new list<Message>;
  sendBuffer = new list<Message>;
  clientList = new list<int>;
  int sockfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //socketNum = sockfd;
  if(sockfd < 0)
  {
    cout << "ERROR opening socket" << endl;
    return -1;
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    cout << "ERROR on binding" << endl;
    return -1;
  }
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  
  int newsockfd[numClients];
  for(int i = 0; i < numClients; i++)
  {
    cout << "Client " << i << endl;
    newsockfd[i] = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if(newsockfd[i] < 0)
    {
      cout << "ERROR on accept" << endl;
      return -1;
    }
    fcntl(newsockfd[i], F_SETFL, O_NONBLOCK);
    cout << "Made " << i + 1 << "/" << numClients << " connections!" << endl;
    //write(newsockfd[i], SSTR(i + 1).c_str(), 1);
    clientList->push_back(newsockfd[i]);
  }
}

int connectToHost()
{
  recvBuffer = new list<Message>;
  sendBuffer = new list<Message>;
  clientList = new list<int>;
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent *server;
 

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    cout << "ERROR opening socket" << endl;
    return -1;
  }

  server = gethostbyname(host.c_str());
  if(server == NULL)
  {
    cout << "ERROR no such host" << endl;
    return -1;
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port);
  if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    cout << "ERROR connecting" << endl;
    return -1;
  }

  char buffer[1];
  //recv(sockfd, buffer, 1, 0);
  //testPrint(SSTR(buffer[0]), 3);
  //localPlayerNum = buffer[0];

  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  //socket = sockfd;
  cout << "Socket is " << sockfd << endl;
  clientList->push_back(sockfd);
  return sockfd;
}
