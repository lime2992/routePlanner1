//include headers
#include <algorithm> //for sorting
#include <fstream>	//used to std::ifstream` object to handle input file streams.
#include <iostream>	//used for cout and general io
#include <sstream>	//string stream is used stream a string
#include <string>	//used to work with strings
#include <vector>	//vector stores our objects 

//namespace declarations
using std::cout;	
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;
using std::abs;

//enum creates State class with kEmpty, kObstacle, kClosed, kPath, kStart, and kFinish as the only valid argurments of type State
enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

// directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}}; //const is used to make it immutable. This array is used to move left, down, right, and up

//Functions

/*
ParseLine is called from ReadBoardFile and takes in each line from the original map at the path, and marks it empty if it is 0 
or an obstacle if it is one into a row and then returns that row to ReadBoardFile. It gets the entire line which is a row. 
*/
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


/*
ReadBoard File takes an input of the path where the map is located, creates a board with a vector of vectors of states,
then for each line in the map, updates the board denoting if the element is an obstacle or empty. 
*/
vector<vector<State>> ReadBoardFile(string path) 
{
  ifstream myfile (path); //creates an ifstream variable myfile with argument path that reads the file
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

/*
 Compare has two constant vectors a and b that are the int vectors from open's begin and end.
 f1 adds the g and h values for current node, f2 adds the g and h values of the end of the open algorithm. 
 If vector a is larger than b, it tells the sort function to sort it by returning a bool value. 
 */
bool Compare(const vector<int> a, const vector<int> b) { //boolean function with const inputs a and b
  int f1 = a[2] + a[3]; // f1 = g1 + h1
  int f2 = b[2] + b[3]; // f2 = g2 + h2
  return f1 > f2; 
}

/*
CellSort accepts a reference to the open vector containting arrays of {x,y,g,h} and calls the sort function (which sorts elements in the ranges [first, last] into ascending orderpassing it the start and end of 
Open and calls compare on these arguments. 
*/
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare); /* sort is defined in the <algorithm> header. Sorts elements in increasing order by default using IntroSort (which is a comobo of quick, heap and insertion sort). 
  Here we are using a comparator (the Compare function), which is a binary predicate that compares its two arguments (the first and last index of the open vector), and returns a boolean value that 
  determines if the first argument goes before the second in the sorted sequence, i.e. if it is larger. So we end up with a descending order. 
  */
}

/*
Heuristic function will return the absolute value of (goal x - inital x) + (goal y - initaly). This is the taxicab geometry.
It measures the distance of every line as if it is made up of only vertical and horizontal segments. Sum of vertical and horizontal distances.
*/
int Heuristic(int x1, int y1, int x2, int y2) {
  return abs(x2 - x1) + abs(y2 - y1);
}

/*
CheckValidCell is called from expand neighbors. It is passed the x and y values of the neighborign cells (each iteration will send u/d/l/r) and the grid of states by reference. 
Returns true if the cell is a valid cell. 
*/
bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
  bool on_grid_x = (x >= 0 && x < grid.size()); //true if x is not the less than 0 (inital value) and x is less than the number  of vectors (rows aka x value) of states
  bool on_grid_y = (y >= 0 && y < grid[0].size()); //true if y is not the less than 0 (inital value) and y is less than the number  of columns in each state (y value)
  if (on_grid_x && on_grid_y) //both x and y are in the grid
    return grid[x][y] == State::kEmpty; //return true if the x and y states are empty 
  return false; //else return false
}


/*
AddToOpen  is called crom search after the heuristic value is calculated. It accepts the current x and y values, the cost of moves and 
the heuristic value, a vector of vectors of ints called openlist and the grid of states. It adds the current cell location, cost and heurisitc
value to the open list as a vector of ints and then sets the states in the grid to closed for the current location. Since openlist and grid are
passed by reference, the changes are made directly and exist outside the function. 

AddToOpen is also called from ExpandNeighbors. Here we add all the neighbors of the current cell to the back of open and then mark their states as
closed in the grid. 
*/
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<State>> &grid) {
  // Add node to open vector, and mark grid cell as closed.
  openlist.push_back(vector<int>{x, y, g, h});
  grid[x][y] = State::kClosed;
}

/*
ExpandNeighbors is called from search after we determine that we are not yet at our goal. we pass it the current cell (x,y,g,h) by reference, the goal array (x,y), the
openlist by reference and the grid of states by refernce. 
*/
void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<State>> &grid) {
  // Get current node's data.
  int x = current[0];
  int y = current[1];
  int g = current[2];

  // Loop through current node's potential neighbors.
  for (int i = 0; i < 4; i++) { //4 since we are checking all four directions
    int x2 = x + delta[i][0]; //set x2 to each delta array's 0 value
    int y2 = y + delta[i][1]; //set y2 to each delta array's 1 value

    // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
    if (CheckValidCell(x2, y2, grid)) { //pass each direction and the grid of states to check valid cell
      // Increment g value and add neighbor to open list.
      int g2 = g + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]); //get the h value for this particular cell
      AddToOpen(x2, y2, g2, h2, openlist, grid); //we add each of the directions that are valid to the end of the open vector of vectors of ints
    }
  }
}



/*
Search is called from main. It is passed the board of states from ReadBoardFile and arrays marking the initial and the goal locations.
*/
vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]) {
  // Create the vector of open nodes.
  vector<vector<int>> open {}; //create a vector of vectors of ints. 
  
  // Initialize the starting node.
  int x = init[0]; //sets x to the x value of the start point as passed from main
  int y = init[1]; //sets y to the y value of the start point as passed from main
  int g = 0; //g is the cost accumulated up to this cell, 0 to start
  int h = Heuristic(x, y, goal[0],goal[1]); //h is the heuristic function that is used to compare next steps. Passes the x and y values of the start and goal.
  AddToOpen(x, y, g, h, open, grid); /*the inital point's x and y values, the cost of moves up to now, the heuristic value 
                                      between current and goal cell, the empty vector of vector of ints and the board of states are passed to addToOpen.*/
/*
Following addtoOpen, the open vector has the current x and y values, cost value and heuristic value between current and goal. Grid has been updated to
reflect the current cells as closed
*/
  while (open.size() > 0) { //while open has inputs
    // Get the next node
    CellSort(&open); //results in a sorted open vector in descending order of g and h values
    auto current = open.back(); //the last value in open should be the lowest h value cell. assign this to current. 
    open.pop_back(); //we remove the last element in open since it is already assigned to current.  
    x = current[0]; //set x to the current cells x value
    y = current[1]; //set y to the current cells y value
    grid[x][y] = State::kPath; //in the grid of states, we make the current cell the state kPath

    // Check if we're done.
    if (x == goal[0] && y == goal[1]) { //if our x and y values matches goal's x and y values
      // TODO: Set the init grid cell to kStart, and 
      // set the goal grid cell to kFinish before returning the grid.	
      grid[0][0] = State::kStart;
      grid[x][y] = State::kFinish;
      return grid;
    }
    
    // If we're not done, expand search to current node's neighbors.
    ExpandNeighbors(current, goal, open, grid); //cell sort will then sort our new directions
  }
  
  // We've run out of new nodes to explore and haven't found a path.
  cout << "No path found!" << "\n";
  return std::vector<vector<State>>{};
}

// TODO: Modify the function signature below to accept
// cells with State type instead of int type.
string CellString(State cell) { //returns string and takes in a State which is what the ith row's jth element is for a vector of vectors of states
  switch(cell) { //switch statement matches to value of cell
    case State::kObstacle: return "⛰️   "; //case that it is an Obstacle
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦 ";
    case State::kFinish: return "🏁 ";
      
      // TODO: Add cases to return "🚦   " for kStart
    // and "🏁   " for kFinish.
    default: return "0   "; 
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
  int init[2]{0, 0}; //coordinates for the inital block
  int goal[2]{4, 5}; //coordinates for the final block
  auto board = ReadBoardFile("1.board"); //creates a board object, calls ReadBoardFile that passes a string argument 1.board and returns a vector<vector<State> 
  auto solution = Search(board, init, goal);
  PrintBoard(solution); //calls printboard and passes a vector of vectors of states
}