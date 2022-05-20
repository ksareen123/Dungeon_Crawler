#include <iostream>
#include <fstream>

using std::cin, std::cout, std::endl, std::string, std::ifstream;

// Player object container
struct Player {
	int row = 0;
	int col = 0;
	int treasure = 0;
};

// constants for tile status
const char TILE_OPEN      = '-';    // blank tile
const char TILE_PLAYER    = 'o';    // tile for player's current location
const char TILE_TREASURE  = '$';    // tile for treasure location
const char TILE_AMULET    = '@';    // tile for hazard that enlarges the dungeon
const char TILE_MONSTER   = 'M';    // tile for monster current location
const char TILE_PILLAR    = '+';    // tile for unpassable pillar location
const char TILE_DOOR      = '?';    // tile for door to the next room
const char TILE_EXIT      = '!';    // tile for exit door out of dungeon

// constants for movement status flags 
const int STATUS_STAY     = 0;      // flag indicating player has stayed still
const int STATUS_MOVE     = 1;      // flag indicating player has moved in a direction
const int STATUS_TREASURE = 2;      // flag indicating player has stepped onto the treasure
const int STATUS_AMULET   = 3;      // flag indicating player has stepped onto an amulet
const int STATUS_LEAVE    = 4;      // flag indicating player has left the current room
const int STATUS_ESCAPE   = 5;      // flag indicating player has gone through the dungeon exit

// constants for user's keyboard inputs
const char INPUT_QUIT     = 'q';    // quit command
const char INPUT_STAY     = 'e';    // no movement
const char MOVE_UP        = 'w';    // up movement
const char MOVE_LEFT      = 'a';    // left movement
const char MOVE_DOWN      = 's';    // down movement
const char MOVE_RIGHT     = 'd';    // right movement


char** createMap(int maxRow, int maxCol) {
    const int rows = maxRow;
    const int cols = maxCol;
    char** map = new char*[rows];
    for (int i = 0; i < rows; i++) {
      map[i] = new char[cols];
    }

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            map[i][j] = TILE_OPEN;
        }
    }
    //std::cout << "map made" << std::endl;
    return map;
}


void deleteMap(char** &map, int& maxRow) {
    if (map) {
        for (int i = 0; i < maxRow; i++) {
            delete[] map[i];
        }
        delete[] map;
    }
    maxRow = 0;
    //std::cout << "map deleted." << std::endl;
}

char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
  //create streams
  std::ifstream inputfile;
  inputfile.open(fileName);
  if(!inputfile.is_open()) { 
      return nullptr;
  }
  else {  
    //cout << "File Opens and works" << endl;
    
    inputfile >> maxRow;
    if (inputfile.fail() || inputfile.bad()) {
        return nullptr;
    }
    inputfile >> maxCol;
    if (inputfile.fail() || inputfile.bad()) {
        return nullptr;
    }

    if (maxRow > 0 && maxCol > 0) {
    //cout << "Dimensions are positive" << endl;

    if ((INT32_MAX / maxRow) < maxCol) {
        return nullptr;
    }
    if ((INT32_MAX / maxCol) < maxRow) {
        return nullptr;
    }
    if ((INT32_MAX * 1.0 / maxRow) < maxRow) {
        return nullptr;
    }
    if ((INT32_MAX * 1.0 / maxCol) < maxCol) {
        return nullptr;
    }
    //cout << "No dimension overflow" << endl;

    const int rows = maxRow;
    const int cols = maxCol;

    int playerrow = 0;

	  inputfile >> playerrow;
    if (!inputfile.fail() && !inputfile.bad()) {
      player.row = playerrow;
    }
    else {
      return nullptr;
    }
    int playercol = 0;
    inputfile >> playercol;
    if (!inputfile.fail() && !inputfile.bad()) {
      player.col = playercol;
    }
    else {
      return nullptr;
    }
    
    if (player.row >= rows || player.row < 0) {
        return nullptr;
    }
    else if (player.col >= cols || player.col < 0) {
        return nullptr;
    }
    //cout << "player position within map" << endl;

    char** map = createMap(rows, cols);
    if (map == nullptr) {
        return nullptr;
    }
    //cout << "created map" << endl;

    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            inputfile >> map[i][j];
            if (inputfile.fail()) {
                deleteMap(map, maxRow);
                return nullptr;
            }
            switch(map[i][j]) {
                case TILE_MONSTER:
                case TILE_TREASURE:
                case TILE_DOOR:
                case TILE_EXIT:
                case TILE_AMULET:
                case TILE_PILLAR:
                case TILE_OPEN:
                //cout << i << " , " << j << " is valid" << endl;
                break;
                default:
                //cout << "invalid" << endl;
                deleteMap(map, maxRow);
                return nullptr;
            }
        }
    }
    //cout << "input file into map" << endl;
    //cout << "checked for incorrect chars" << endl;

    char test;
    inputfile >> test;
    if (!inputfile.fail()) {
        return nullptr;
    }
    //cout << "nothing left in file" << endl;

    test = ' ';
    bool way_out = false;
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
        test = map[i][j];
        if (test == TILE_DOOR || test == TILE_EXIT) {
            way_out = true;
        }
        }
    }
    if (way_out == false) {
        deleteMap(map, maxRow);
        return nullptr;
    }
    //cout << "way out" << endl;

    map[player.row][player.col] = TILE_PLAYER;
    //cout << "Final map output" << endl;
    return map;
    }
    else {
    //cout << "dimensions negative / 0" << endl;
    return nullptr;
    }
  }
  inputfile.close();
  return nullptr;
}

void getDirection(char input, int& nextRow, int& nextCol) {
    switch(input) {
        case 'a':
            nextCol -= 1;
            break;
        case 'w':
            nextRow -= 1;
            break;
        case 's':
            nextRow += 1;
            break;
        case 'd':
            nextCol += 1;
            break;
        case 'e':
            break;
        case 'q':
        default:
            nextRow = nextRow;
    }
}

char** resizeMap(char** map, int& maxRow, int& maxCol) {
    const int rows = maxRow * 2;
    const int cols = maxCol * 2;

    char** map2 = createMap(rows, cols);

    int playerrow;
    int playercol;
    
    for (int i = 0; i < maxRow; i++) {
      for (int j = 0; j < maxCol; j++) {
        if (map[i][j] == TILE_PLAYER) {
          playerrow = i;
          playercol = j;
          map[i][j] = TILE_OPEN;
        }
      }
    }
    
    //std::cout << "Found player" << std::endl; // debug
    
    //top left
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            map2[i][j] = map[i][j];
        }
    }
    //bottom left
    for (int i = maxRow; i < rows; i++) {
        for (int j = 0; j < maxCol; j++) {
            map2[i][j] = map[i - (maxRow)][j];
        }
    }
    //top right
    for (int i = 0; i < maxRow; i++) {
        for (int j = maxCol; j < cols; j++) {
            map2[i][j] = map[i][j - (maxCol)];
        }
    }
    //bottom right
    for (int i = maxRow; i < rows; i++) {
        for (int j = maxCol; j < cols; j++) {
            map2[i][j] = map[i - (maxRow)][j - (maxCol)];
        }
    }

    map2[playerrow][playercol] = TILE_PLAYER;
    /*
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        std::cout << map2[i][j] << " ";
      }
      std::cout << endl;
    }
    */
    int rows2 = maxRow;
    deleteMap(map, rows2);
    maxRow *= 2;
    maxCol *= 2;

    return map2;
}


int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    int status = STATUS_STAY;
    if (nextRow < maxRow && nextRow >= 0 && nextCol < maxCol && nextCol >= 0) {
        if (map[nextRow][nextCol] == TILE_OPEN) {
            status = STATUS_MOVE;
        }
        else if (map[nextRow][nextCol] == TILE_TREASURE) {
            player.treasure += 1;
            status = STATUS_TREASURE;
        }
        else if (map[nextRow][nextCol] == TILE_AMULET) {
            status = STATUS_AMULET;
        }
        else if (map[nextRow][nextCol] == TILE_DOOR) {
            status = STATUS_LEAVE;
        }
        else if (map[nextRow][nextCol] == TILE_EXIT && player.treasure > 0) {
            status = STATUS_ESCAPE;
        }
        else {
            nextRow = player.row;
            nextCol = player.col;
        }

        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = TILE_PLAYER;
    }
    return status;
}


bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    bool monsterAttack = false;
    int row = player.row;
    int col = player.col;
    bool is_visible = false;
    if (map == nullptr) {
        return false;
    }
    else {
      if (map[player.row][player.col] != TILE_PLAYER) {
        return false;
      }
      else {
          //up
          //cout << "Looking up..." << endl;
          row = player.row - 1;
          while (row >= 0 && map[row][player.col] != TILE_PILLAR) {
              //cout << "searching at " << row << " , " << player.col << endl;
              if (map[row][player.col] == TILE_MONSTER) {
                if (map[row + 1][player.col] == TILE_PLAYER) {
                  monsterAttack = true;
                  //map[row][player.col] = TILE_OPEN;
                  map[player.row][player.col] = TILE_MONSTER;
                  map[row][player.col] = TILE_OPEN;
                  //cout << "Attacked by monster." << endl;
                }
                else if (map[row + 1][player.col] != TILE_PILLAR) {
                  map[row][player.col] = TILE_OPEN;
                  map[row + 1][player.col] = TILE_MONSTER;
                  //cout << "Monster Spotted above at " << row << " , " << player.col << endl;
                }
              }
              else {
                //cout << "No monster present" << endl;
              }
              row--;
          }

          // down
          //cout << "Looking down..." << endl;
          row = player.row + 1;
          while (row < maxRow && map[row][player.col] != TILE_PILLAR) {
              //cout << "searching at " << row << " , " << player.col << endl;
              if (map[row][player.col] == TILE_MONSTER) {
                if (map[row - 1][player.col] == TILE_PLAYER) {
                  monsterAttack = true;
                  map[row][player.col] = TILE_OPEN;
                  map[row - 1][player.col] = TILE_MONSTER;
                  //cout << "Attacked by monster." << endl;
                }
                else if (map[row - 1][player.col] != TILE_PILLAR) {
                  map[row][player.col] = TILE_OPEN;
                  map[row - 1][player.col] = TILE_MONSTER;
                  //cout << "Monster Spotted below at " << row << " , " << player.col << endl;
                }
              }
              else {
                //cout << "No monster present" << endl;
              }
              row++;
          }

          //left 
          //cout << "Looking left..." << endl;
          row = player.row;
          col = player.col - 1;
          while (col >= 0 && map[player.row][col] != TILE_PILLAR) {
              //cout << "searching at " << player.row << " , " << col << endl;
              if (map[player.row][col] == TILE_MONSTER) {
                if (map[player.row][col + 1] == TILE_PLAYER) {
                  monsterAttack = true;
                  map[player.row][col] = TILE_OPEN;
                  map[player.row][col + 1] = TILE_MONSTER;
                  //cout << "Attacked by monster." << endl;
                }
                else if (map[player.row][col + 1] != TILE_PILLAR) {
                  map[player.row][col] = TILE_OPEN;
                  map[player.row][col + 1] = TILE_MONSTER;
                  //cout << "Monster Spotted left at " << player.row << " , " << col << endl;
                }
              }
              else {
                //cout << "No monster present" << endl;
              }
              col--;
          }

          //right
          //cout << "Looking right..." << endl;
          row = player.row;
          col = player.col + 1;
          while (col < maxCol && map[player.row][col] != TILE_PILLAR) {
              //cout << "searching at " << player.row << " , " << col << endl;
              if (map[player.row][col] == TILE_MONSTER) {
                if (map[player.row][col - 1] == TILE_PLAYER) {
                  monsterAttack = true;
                  map[player.row][col] = TILE_OPEN;
                  map[player.row][col - 1] = TILE_MONSTER;
                  //cout << "Attacked by monster." << endl;
                }
                else if (map[player.row][col - 1] != TILE_PILLAR) {
                  map[player.row][col] = TILE_OPEN;
                  map[player.row][col - 1] = TILE_MONSTER;
                  //cout << "Monster Spotted right at " << player.row << " , " << col << endl;
                }
              }
              else {
                //cout << "No monster present" << endl;
              }
              col++;
          }
      }
    }
    return monsterAttack;
}


int main() {
  int maxRow;
  int maxCol;
  string filename;
  Player player1;

  cin >> filename;
  char** map = loadLevel(filename, maxRow, maxCol, player1);
  
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  /*
  doMonsterAttack(map, maxRow, maxCol, player1);
  doPlayerMove(map, maxRow, maxCol, player1, 0, 4);

  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  doMonsterAttack(map, maxRow, maxCol, player1);
  */

/*
  char** map = loadLevel(filename, maxRow, maxCol, player1);
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;

  doMonsterAttack(map, maxRow, maxCol, player1);
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;

  doPlayerMove(map, maxRow, maxCol, player1, 1, 0);
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;

  doMonsterAttack(map, maxRow, maxCol, player1);
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;

  doPlayerMove(map, maxRow, maxCol, player1, 1, 1);
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;
  
  if (doMonsterAttack(map, maxRow, maxCol, player1) == true) {
    std::cout << "attacked by m" << std::endl;
  }
  for (int i = 0; i < maxRow; i++) {
    for (int j = 0; j < maxCol; j++) {
      std::cout << map[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "map 1 " << std::endl;
  std::cout << maxRow << " X " << maxCol << std::endl;
*/
} 