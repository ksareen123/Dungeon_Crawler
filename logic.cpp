#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
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

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char** &map, int& maxRow) {
    const int rows = maxRow;
    maxRow = 0;
    if (map == nullptr) {
        return;
    }
    else {
        for (int i = 0; i < rows; i++) {
            delete[] map[i];
        }
        delete[] map;
    }
    map = nullptr;
}

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
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

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
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

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    char** map2;
    if (map != nullptr) {
        if (maxRow > 0 && maxCol > 0) {
            const int rows = maxRow * 2;
            const int cols = maxCol * 2;
            map2 = createMap(rows, cols);
                
            if (map) {
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
                int rows2 = maxRow;
                deleteMap(map, rows2);
                maxRow *= 2;
                maxCol *= 2;
            }
        }
        else {
            return nullptr;
        }
    }
    else {
        return nullptr;
    }
    return map2;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
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

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
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

