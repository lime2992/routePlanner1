//include headers
#include <fstream>	//used to std::ifstream` object to handle input file streams.
#include <iostream>	//used for cout and general io
#include <sstream>	//string stream is used stream a string
#include <string>	//used to work with strings
#include <vector>	//vector stores our objects 

//namespace declarations
using std::cout;	
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;

//enum creates State class with kEmpty and kObstacle as the only valid argurments of type State
enum class State {kEmpty, kObstacle};

//Functions

vector<State> ParseLine(string line) //takes in input string and returns a Vector of States
{
    istringstream sline(line); //string stream variable sline with string line passed to it
    int n;
    char c;
    vector<State> row; //vector of states
    while (sline >> n >> c && c == ',') //while there is an int and char, and the char is ,
    {
      if (n == 0) {	//if the input line's first item is 0
        row.push_back(State::kEmpty);	//vector's next index is a state kEmpty
      } else {
        row.push_back(State::kObstacle);	//vector's next index is a state kObstacle
      }
    }
    return row;	//return the vector
}


//ReadBoard File returns a vector>vector<State> and takes argument string
vector<vector<State>> ReadBoardFile(string path) 
{
  ifstream myfile (path); //creates an ifstream variable myfile with argument path
  vector<vector<State>> board{}; //creates board vector of vectors of states.
  if (myfile) { //if myfile stream has been created succesfully
    string line; //create string var named line
    while (getline(myfile, line)) { //while there is a line (delimited item) in myfile
      vector<State> row = ParseLine(line); //creates a vector of States that calls ParseLine whose argument is each line in myfile
      board.push_back(row); //add the return of ParseLine (a vector of states) to this vector's next index
    }
  }
  return board; //return the vector of vectors of states
}

// TODO: Modify the function signature below to accept
// cells with State type instead of int type.
string CellString(State cell) { //returns string and takes in a State which is what the ith row's jth element is for a vector of vectors of states
  switch(cell) { //switch statement matches to value of cell
    case State::kObstacle: return "⛰️   "; //case that it is an Obstacle
    default: return "0   "; //else
  }
}

void PrintBoard(const vector<vector<State>> board) //takes in a vector of vectors
{
  for (int i = 0; i < board.size(); i++) { 
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);	//calls CellString and passes the board's ith row's jth element and prints it
    }
    cout << "\n";
  }
}

int main() {
  auto board = ReadBoardFile("1.board"); //creates a board object, calls ReadBoardFile that passes a string argument 1.board and returns a vector<vector<State> 
  PrintBoard(board); //calls printboard and passes a vector of vectors of states
}