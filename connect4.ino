#include <M5Cardputer.h>
#include <vector>
#include <ctime>

extern const uint8_t grid[];
extern const uint8_t red_arrow[];
extern const uint8_t blue_arrow[];
extern const uint8_t blue_wins[];
extern const uint8_t red_wins[];
extern const uint8_t single_player[];
extern const uint8_t two_players[];

String CurrentColor = "RED";
String ScreenStatus = "MENU";
String MenuStatus = "SINGLE";

bool checkWinFlag = false;

int k = -1;
int DBC = 300; //Debounce
int DrawDelay = 500;
int SelectedColumn = 0; 
int Victory = 0; // 1 = Blue; 2 = Red
int matrix[7][16]  = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};



void setup() {
  M5Cardputer.begin();
  M5Cardputer.Display.setRotation(1);

  if (M5Cardputer.Display.isEPD()) {
    M5Cardputer.Display.setEpdMode(epd_mode_t::epd_fastest);
    M5Cardputer.Display.invertDisplay(true);
    M5Cardputer.Display.clear(TFT_BLACK);
  }

  if (M5Cardputer.Display.width() < M5Cardputer.Display.height())
  {
    M5Cardputer.Display.setRotation(M5Cardputer.Display.getRotation() ^ 1);
  }
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

}



void loop() {  
  M5Cardputer.update();
  
  if (ScreenStatus == "MENU"){
    main_menu();
  
  }else if ((ScreenStatus == "PLAYING") && (MenuStatus == "TWO")) {
    play_against_human();

  }else if ((ScreenStatus == "PLAYING") && (MenuStatus == "SINGLE")) {
    play_against_bot();
  }


  if (Victory == 1) {
    delay(2000);
    M5Cardputer.Display.drawPng(blue_wins, ~0u, 0, 0);
    delay(1000);
    reset_game();
      
  }else if (Victory == 2){
    delay(2000);
    M5Cardputer.Display.drawPng(red_wins, ~0u, 0, 0);
    delay(1000);
    reset_game();
  }

}










// BOT ALGORITHM FUNCTIONS

int getBestMove() {

  // Verify if bot can win
  for (int col = 0; col < 16; col++) {
    if (botCanWin(col)) {
      return col;
    }
  }

  // Verify if opponent have 3 in sequence
  for (int col = 0; col < 16; col++) {
    if (canBlockOpponent(col)) {
      return col;
    }
  }

  // Verify if opponent have 2 adjacent pieces
  if (blockTwoAdjacent(2) != -1){
    return blockTwoAdjacent(2);
  }
  
  // To increase sequence
  if (IncreaseSequence() != -1){  
    return IncreaseSequence();
  } 

  // Aleatory movement
  return getRandomMove();
}


// To see if bot can win
bool botCanWin(int col) {
  for (int row = 7 - 1; row >= 0; --row) {
    if (matrix[row][col] == 0) {
      matrix[row][col] = 1; 
      bool result = checkWin(1);
      matrix[row][col] = 0; 
      if (result){
        return true;
      }
      break;
    }
  }
  return false;
}


// To see if bot can block opponent
bool canBlockOpponent(int col) {
  for (int row = 7 - 1; row >= 0; --row) {
    if (matrix[row][col] == 0) {
      matrix[row][col] = 2; 
      bool result = checkWin(2);
      matrix[row][col] = 0; 
      if (result){
        return true;
      }
      break;
    }
  }
  return false;
}


// To see if there are two enemy adjacent pieces
int blockTwoAdjacent(int player) {
  for (int row = 7 - 1; row >= 0; --row) {
    for (int col = 0; col < 16; ++col) {
      if (matrix[row][col-1] == 0 && matrix[row][col] == player && matrix[row][col + 1] == player && matrix[row][col+2] == 0) {
        return col-1;
      }
    }
  }
  return -1;
}


// To increase bot sequence
int IncreaseSequence() {
  for (int col = 0; col < 16; ++col) {
    for (int row = 0; row < 7; ++row) {
      if (matrix[row][col] == 1) {
        if (col + 1 < 16 && matrix[row][col + 1] == 0) {
          return (col + 1);
        }
        if (col - 1 >= 0 && matrix[row][col - 1] == 0) {
          return (col - 1);
        }
        if (row + 1 < 7 && matrix[row + 1][col] == 0) {
          return col;
        }
        if (row - 1 >= 0) {
          if (col - 1 >= 0 && matrix[row - 1][col - 1] == 0) {
            return (col - 1);
          }
          if (col + 1 < 16 && matrix[row - 1][col + 1] == 0) {
            return (col + 1);
          }
        }
        if (row + 1 < 7) {
          if (col - 1 >= 0 && matrix[row + 1][col - 1] == 0) {
            return (col - 1);
          }
          if (col + 1 < 16 && matrix[row + 1][col + 1] == 0) {
            return (col + 1);
          }
        }
      }
    }
  }
  return -1; 
}


// To generate a random move for the bot
int getRandomMove() {

  int leftCount = 0;
  int rightCount = 0;
  int num;

  for (int row = 0; row < 7; ++row) {
    for (int col = 0; col < 16; ++col) {
      if (matrix[row][col] != 0) {
        if (col < 8) {
          leftCount++;
        } else {
          rightCount++;
        }
      }
    }
  }


  if (leftCount > rightCount) {
    do {
      num = std::rand() % 8;
    } while (matrix[0][num] != 0);
    return num;

  }else{
    do {
      num = 8 + std::rand() % 8;
    } while (matrix[0][num] != 0);
    return num;
  }  
}


// Used to get results about simulating user actions
bool checkWin(int player) {
  checkWinFlag = true;
  victory_horizontal(player);
  victory_vertical(player);
  victory_diagonal_positive(player);
  victory_diagonal_negative(player);
  checkWinFlag = false;

  if (Victory == player) {
    Victory = 0;
    return true;
  }
  return false;
}











// INTERFACE FLUX FUNCTIONS

// Generate Main menu interface
void main_menu() {
  if(MenuStatus == "SINGLE"){
    M5Cardputer.Display.drawPng(single_player, ~0u, 0, 0);
    if(M5Cardputer.Keyboard.isKeyPressed('m')){
      MenuStatus = "TWO";
      delay(DBC);

    }

  }else if(MenuStatus == "TWO"){
    M5Cardputer.Display.drawPng(two_players, ~0u, 0, 0);
    if(M5Cardputer.Keyboard.isKeyPressed('m')){
      MenuStatus = "SINGLE";
      delay(DBC);

    }
  }

  if(M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER) && ScreenStatus == "MENU"){
    ScreenStatus = "PLAYING";
    M5Cardputer.Display.drawPng(grid, ~0u, 0, 0);
    delay(DBC);
  }

}

// Set flags to default
void reset_game() {
  Victory = 0;
  k = -1;
  SelectedColumn = 0;
  CurrentColor = "RED";
  ScreenStatus = "MENU";
  MenuStatus = "SINGLE";

  for(int lin = 0; lin < 7; lin++){
    for(int col = 0; col < 16; col++){
      matrix[lin][col] = 0;
    }
  }
}

void play_against_human() {
  cursor_two();
  victory_horizontal(1);
  victory_horizontal(2);
  victory_vertical(1);
  victory_vertical(2);
  victory_diagonal_positive(1);
  victory_diagonal_positive(2);
  victory_diagonal_negative(1);
  victory_diagonal_negative(2);
}

void play_against_bot() {
  cursor_single();
  victory_horizontal(1);
  victory_horizontal(2);
  victory_vertical(1);
  victory_vertical(2);
  victory_diagonal_positive(1);
  victory_diagonal_positive(2);
  victory_diagonal_negative(1);
  victory_diagonal_negative(2);
}

// Used to select and draw arrows on the grid
void cursor_single() {
  int FIRST_LINE = 6;

  if(CurrentColor == "RED") {
    
    if (M5Cardputer.Keyboard.isKeyPressed('/')) {
      M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
      k++;
      replot();
      draw_arrow(k,CurrentColor);
      delay(DBC);

    }else if (M5Cardputer.Keyboard.isKeyPressed(',')) {
      M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
      k--;
      replot();
      draw_arrow(k,CurrentColor);
      delay(DBC);

    }else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
      CurrentColor = "BLUE";
      M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
      SelectedColumn = k;
      k = -1;

      while (matrix[FIRST_LINE][SelectedColumn] != 0) {
        FIRST_LINE--;
      }

      if ((SelectedColumn >= 0) && (FIRST_LINE >= 0)){
        matrix[FIRST_LINE][SelectedColumn] = 2; 
        draw_circle_on_grid(FIRST_LINE, SelectedColumn, TFT_RED);
      }
      delay(DBC);

      }
  

  }else if (CurrentColor == "BLUE"){
    CurrentColor = "RED";
    M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
    SelectedColumn = getBestMove();
    

    while (matrix[FIRST_LINE][SelectedColumn] != 0) {
      FIRST_LINE--;
    }

    if (SelectedColumn <= 8){
      for (int cont = 0; cont <= SelectedColumn; cont++){
        M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
        draw_arrow(cont,"BLUE");
        delay(100);
      }
      
    }else if (SelectedColumn > 8){
      for (int cont = 16; cont >= SelectedColumn; cont--){
        M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
        draw_arrow(cont,"BLUE");
        delay(100);
      }
    }
    
    M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);

    if ((SelectedColumn >= 0) && (FIRST_LINE >= 0)){
      matrix[FIRST_LINE][SelectedColumn] = 1; 
      draw_circle_on_grid(FIRST_LINE, SelectedColumn, TFT_BLUE);
    }

  }
}


// Used to select and draw arrows on the grid
void cursor_two() {
  int FIRST_LINE = 6;

  if (M5Cardputer.Keyboard.isKeyPressed('/')) {
    M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
    k++;
    replot();
    draw_arrow(k,CurrentColor);
    delay(DBC);

  }else if (M5Cardputer.Keyboard.isKeyPressed(',')) {
    M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
    k--;
    replot();
    draw_arrow(k,CurrentColor);
    delay(DBC);

  }else if (M5Cardputer.Keyboard.isKeyPressed(KEY_ENTER)) {
    if (CurrentColor == "BLUE"){
      CurrentColor = "RED";
      M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
      SelectedColumn = k;
      k = -1;

      while (matrix[FIRST_LINE][SelectedColumn] != 0) {
        FIRST_LINE--;
      }

      if ((SelectedColumn >= 0) && (FIRST_LINE >= 0)) {
        matrix[FIRST_LINE][SelectedColumn] = 1; // 1 FOR BLUE
        draw_circle_on_grid(FIRST_LINE, SelectedColumn, TFT_BLUE);
      }
      

    }else if (CurrentColor == "RED"){
      CurrentColor = "BLUE";
      M5Cardputer.Display.fillRect(0, 0, 240, 25, BLACK);
      SelectedColumn = k;
      k = -1;

      while (matrix[FIRST_LINE][SelectedColumn] != 0) {
        FIRST_LINE--;
      }

      if ((SelectedColumn >= 0) && (FIRST_LINE >= 0)){
        matrix[FIRST_LINE][SelectedColumn] = 2; // 2 FOR RED
        draw_circle_on_grid(FIRST_LINE, SelectedColumn, TFT_RED);
      }

      
    }
    delay(DBC);
  }
}










// VERIFY VICTORY FUNCTIONS

void victory_horizontal(int player) {

  if (Victory == 0){
    std::vector<int> winCol;

    for (int current_line = 0; current_line <= 6; current_line++) {
      int maxCount = 0;  
      int currentCount = 0;

      for (int current_col = 0; current_col <= 15; current_col++){
        
        if (matrix[current_line][current_col] == player) {
            currentCount++;
            winCol.push_back(current_col);
            if (currentCount > maxCount) {
                maxCount = currentCount;
            }
        } else {
            currentCount = 0;
            winCol.clear();
        }
      }

      if (maxCount >= 4) {

        if(checkWinFlag == false){
          M5Cardputer.Speaker.tone(2000, 100);
          for(int c = 0; c < 4;c++){
            delay(DrawDelay);
            draw_circle_on_grid(current_line, winCol[c], TFT_GREEN);
          }
        }
        Victory = player; 
      } 
    }
  }
}


void victory_vertical(int player) {
  if (Victory == 0) {
    for (int current_col = 0; current_col <= 15; current_col++) {
      int currentCount = 0;
      std::vector<int> winRow;

      for (int current_line = 0; current_line <= 6; current_line++) {
        
        if (matrix[current_line][current_col] == player) {
          currentCount++;
          winRow.push_back(current_line);

          
          if (currentCount == 4) {
            if(checkWinFlag == false){
              M5Cardputer.Speaker.tone(2000, 100);
              for (int r = 0; r < 4; r++) {
                delay(DrawDelay);
                draw_circle_on_grid(winRow[r], current_col, TFT_GREEN);
              } 
            }
            Victory = player;
          }
        } else {
          currentCount = 0;
          winRow.clear();
        }
      }
    }
  }
}



void victory_diagonal_positive(int player) {
  if (Victory == 0) {
    std::vector<std::pair<int, int>> winCells;

    for (int start_row = 0; start_row <= 6 - 3; start_row++) {
      for (int start_col = 0; start_col <= 15 - 3; start_col++) {
        bool win = true;
        winCells.clear();

        for (int offset = 0; offset < 4; offset++) {
          int current_row = start_row + offset;
          int current_col = start_col + offset;

          if (matrix[current_row][current_col] != player) {
              win = false;
              break;
          }
          winCells.push_back({current_row, current_col});
        }

        if (win) {
          if(checkWinFlag == false){
            M5Cardputer.Speaker.tone(2000, 100);
            for (auto cell : winCells) {
              delay(DrawDelay);
              draw_circle_on_grid(cell.first, cell.second, TFT_GREEN);
            }
          }  
          
          Victory = player;
          return;
        }
      }
  }

    for (int start_row = 0; start_row <= 6 - 3; start_row++) {
      for (int start_col = 15; start_col >= 3; start_col--) {
        bool win = true;
        winCells.clear();

        for (int offset = 0; offset < 4; offset++) {
          int current_row = start_row + offset;
          int current_col = start_col - offset;

          if (matrix[current_row][current_col] != player) {
            win = false;
            break;
          }
          winCells.push_back({current_row, current_col});
        }

        if (win) {
          if(checkWinFlag == false){
            M5Cardputer.Speaker.tone(2000, 100);
            for (auto cell : winCells) {
              delay(DrawDelay);
              draw_circle_on_grid(cell.first, cell.second, TFT_GREEN);
            }
          }
            
          Victory = player;
          return;
        }
      }
    }
  }
}



void victory_diagonal_negative(int player) {
  if (Victory == 0) {
    std::vector<std::pair<int, int>> winCells;

    for (int start_row = 0; start_row <= 6 - 3; start_row++) {
      for (int start_col = 0; start_col <= 15 - 3; start_col++) {
        bool win = true;
        winCells.clear();

        for (int offset = 0; offset < 4; offset++) {
          int current_row = start_row + offset;
          int current_col = start_col + offset;

          if (matrix[current_row][current_col] != player) {
              win = false;
              break;
          }
          winCells.push_back({current_row, current_col});
        }

        if (win) {
          if(checkWinFlag == false){
            M5Cardputer.Speaker.tone(2000, 100);
            for (auto cell : winCells) {
              delay(DrawDelay);
              draw_circle_on_grid(cell.first, cell.second, TFT_GREEN);
            }
          }
            
          Victory = player;
          return;
        }
      }
  }

    for (int start_row = 3; start_row <= 6; start_row++) {
      for (int start_col = 0; start_col <= 15 - 3; start_col++) {
        bool win = true;
        winCells.clear();

        for (int offset = 0; offset < 4; offset++) {
          int current_row = start_row - offset;
          int current_col = start_col + offset;

          if (matrix[current_row][current_col] != player) {
              win = false;
              break;
          }
          winCells.push_back({current_row, current_col});
        }

        if (win) {
          if(checkWinFlag == false){
            M5Cardputer.Speaker.tone(2000, 100);
            for (auto cell : winCells) {
              delay(DrawDelay);
              draw_circle_on_grid(cell.first, cell.second, TFT_GREEN);
            }
          }
            
          Victory = player;
          return;
        }
      }
    }
  }
}








// INTERFACE/DRAW FUNCTIONS

void draw_circle_on_grid(int line, int column, int color){
  M5Cardputer.Display.fillCircle(22+13*column, 32+14*line, 5, color);  
}

void replot() {
  if (k == 16) {
    k = 0;
  }else if (k <= -1) {
    k = 15;
  }
}

void draw_arrow(int column, String color){
  if (color == "RED"){
    M5Cardputer.Display.drawPng(red_arrow, ~0u, 17+13*column, 8);
  }else if (color == "BLUE"){
    M5Cardputer.Display.drawPng(blue_arrow, ~0u, 17+13*column, 8);
  }
}










// BACKGROUNDS AND IMAGES

constexpr uint8_t grid[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x87,0x08,0x02,0x00,0x00,0x00,0xa7,0x10,0x43,
	0xbc,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x01,0xd3,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0xda,0xb1,
	0x0d,0xc2,0x40,0x10,0x45,0x41,0xe4,0x6e,0xa9,0x88,0x76,0x29,0x01,0xd6,0xb2,0x61,
	0xf5,0x34,0x13,0xff,0xe0,0x82,0xa7,0x8d,0xee,0x78,0x40,0xc8,0xf1,0xef,0x07,0xc0,
	0x95,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,0x45,0xd0,0xa4,0x08,0x9a,
	0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,
	0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,
	0x11,0x34,0x29,0x82,0x26,0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0x32,0x0b,0xfa,
	0x09,0x3f,0x77,0x63,0xd0,0xb0,0xdc,0x99,0xa0,0x5f,0x9f,0x98,0x99,0x5d,0x38,0x1b,
	0x71,0xa1,0x49,0x11,0x34,0x29,0x82,0x26,0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,
	0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,0xe5,0x4c,0xd0,
	0x5f,0x7e,0x8f,0x32,0x33,0xbb,0x64,0x36,0xe2,0x42,0x93,0xe2,0xb7,0x9d,0xd9,0xf6,
	0xd9,0x88,0x0b,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,0x45,0xd0,0xa4,0x08,
	0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x7e,
	0xdb,0x99,0x6d,0x9f,0x8d,0xb8,0xd0,0xa4,0xf8,0x6d,0x67,0xb6,0x7d,0x36,0xe2,0x42,
	0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,0x45,0xd0,
	0xa4,0x08,0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xdf,0x76,0x66,0xdb,
	0x67,0x23,0x2e,0x34,0x29,0x7e,0xdb,0x99,0x6d,0x9f,0x8d,0xb8,0xd0,0xa4,0x08,0x9a,
	0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,
	0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0xe2,0xb7,0x9d,0xd9,0xf6,0xd9,0x88,0x0b,
	0x4d,0x8a,0xdf,0x76,0x66,0xdb,0x67,0x23,0x2e,0x34,0x29,0x82,0x26,0x45,0xd0,0xa4,
	0x08,0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,
	0x82,0x26,0x45,0xd0,0xa4,0xf8,0x6d,0x67,0xb6,0x7d,0x36,0xe2,0x42,0x93,0xe2,0xb7,
	0x9d,0xd9,0xf6,0xd9,0x88,0x0b,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,0x45,
	0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,
	0x34,0x29,0x7e,0xdb,0x99,0x6d,0x9f,0x8d,0xb8,0xd0,0xa4,0xf8,0x6d,0x67,0xb6,0x7d,
	0x36,0xe2,0x42,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,
	0x26,0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0x22,0x68,0x52,0x04,0x4d,0x8a,0xdf,
	0x76,0x66,0xdb,0x67,0x23,0x2e,0x34,0x29,0xb3,0xa0,0x3f,0x7e,0x8f,0x82,0xcb,0xdd,
	0x18,0x34,0x2c,0x27,0x68,0x52,0x04,0x4d,0x8a,0xa0,0x49,0x11,0x34,0x29,0x82,0x26,
	0x45,0xd0,0xa4,0x08,0x9a,0x14,0x41,0x93,0xf2,0x06,0x4c,0x3f,0xe7,0xff,0xdb,0x56,
	0x2d,0x7e,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};

constexpr uint8_t blue_arrow[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x0c,0x08,0x02,0x00,0x00,0x00,0x3b,0xcb,0xd0,
	0xc9,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x00,0x24,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x60,0x40,
	0x01,0xff,0x61,0x08,0x27,0x18,0xdc,0x2a,0xfe,0x13,0x42,0x04,0x14,0x11,0x30,0x09,
	0xa7,0x23,0xf0,0x39,0x96,0xb0,0x5f,0x18,0x30,0xa5,0x01,0x27,0xa4,0x35,0xcb,0x6d,
	0x10,0x04,0xf4,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};

constexpr uint8_t red_arrow[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0x0b,0x00,0x00,0x00,0x0c,0x08,0x02,0x00,0x00,0x00,0x3b,0xcb,0xd0,
	0xc9,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x00,0x25,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x60,0x40,
	0x02,0xff,0x61,0x08,0x27,0x18,0xdc,0x2a,0xfe,0x13,0x42,0x04,0x14,0x11,0x30,0x09,
	0xa7,0x23,0xf0,0x39,0x96,0xb0,0x5f,0x18,0xb0,0x49,0x03,0x00,0x93,0x38,0x35,0xcb,
	0xeb,0x1f,0x5e,0xd5,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};


constexpr uint8_t blue_wins[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x87,0x08,0x02,0x00,0x00,0x00,0xa7,0x10,0x43,
	0xbc,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x01,0x58,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0xd9,0x41,
	0x8e,0x83,0x30,0x10,0x45,0xc1,0xdc,0xff,0xd2,0x33,0xdb,0xac,0x5a,0xd8,0x98,0xe0,
	0xfe,0xae,0x12,0xbb,0x58,0x4d,0x5a,0x3c,0xa4,0x48,0xf9,0x7c,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x99,0xbf,0x91,0x6b,0x6e,0x20,
	0xfc,0x8e,0xa0,0x89,0x22,0x68,0xa2,0x5c,0xe9,0x6f,0xa8,0x51,0x41,0xf3,0x26,0x41,
	0x13,0x45,0xd0,0x44,0x79,0x37,0xe8,0x1f,0xdc,0xdd,0x0b,0x76,0x16,0x41,0x13,0x45,
	0xd0,0x44,0xb9,0x52,0xc0,0x50,0x0a,0xaf,0x04,0xbd,0xfc,0xc5,0xa0,0x2b,0x41,0x13,
	0x45,0xd0,0x44,0x59,0x9e,0xc2,0x26,0x41,0xaf,0x7a,0x3f,0x69,0x46,0xd0,0x44,0x11,
	0x34,0x51,0x52,0x83,0xe6,0x50,0x82,0x26,0x8a,0xa0,0x89,0xf2,0xdc,0x8f,0xce,0x9b,
	0x65,0xcf,0xdd,0x7d,0xf9,0x16,0x34,0x23,0x68,0xa2,0x08,0x1a,0x00,0x00,0x00,0x00,
	0x00,0xe0,0xdb,0xcd,0x3f,0x29,0x96,0x4f,0xde,0x4d,0xc6,0x16,0x07,0x11,0x74,0x2d,
	0x63,0x8b,0x83,0x08,0xba,0x96,0xb1,0xc5,0x41,0x9e,0x0b,0x3a,0x83,0xa0,0x9b,0x11,
	0x74,0x4d,0xd0,0xcd,0x08,0xba,0x26,0xe8,0x66,0x8a,0x07,0x76,0xe5,0xa3,0xb9,0xc9,
	0x57,0x0e,0x17,0xd7,0xdc,0x5e,0xab,0xe6,0xb0,0x35,0x41,0x8f,0xce,0x61,0x6b,0x82,
	0x1e,0x9d,0xc3,0xd6,0xb6,0x0d,0xfa,0xb9,0x33,0x43,0x04,0xdd,0x8c,0xa0,0x6b,0x82,
	0x6e,0x46,0xd0,0x35,0x41,0x37,0x13,0x16,0x74,0x71,0x78,0x2e,0x4d,0x41,0x37,0x23,
	0xe8,0x9a,0xa0,0x9b,0x11,0x74,0x4d,0xd0,0xcd,0xa4,0x06,0xbd,0xea,0x8b,0xd1,0x8c,
	0xa0,0x05,0x1d,0x45,0xd0,0x82,0xce,0x34,0x57,0xf6,0xdc,0xc0,0xb9,0xc3,0x73,0x67,
	0x8a,0x6b,0xf9,0x16,0x6c,0x44,0xd0,0xf7,0xb7,0x60,0x23,0x82,0xbe,0xbf,0x05,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0xb4,0x7f,0xce,
	0x14,0x33,0x22,0x8c,0x9b,0x2a,0x2b,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,
	0x42,0x60,0x82
};

constexpr uint8_t red_wins[] = {
	0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x87,0x08,0x02,0x00,0x00,0x00,0xa7,0x10,0x43,
	0xbc,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x01,0x5f,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0xd9,0x31,
	0x8e,0x43,0x21,0x0c,0x40,0xc1,0xdc,0xff,0xd2,0xbb,0x7d,0x0a,0xeb,0x03,0xfe,0x8a,
	0x6d,0x66,0x44,0x17,0xe4,0x80,0xf2,0x0a,0xa4,0x7c,0x3e,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xbe,0xbf,0x95,0x55,0x44,0xd9,0x83,
	0xf1,0x7b,0x82,0x66,0x14,0x41,0x33,0xca,0x93,0x38,0xaa,0x05,0x54,0xed,0x3c,0x14,
	0x22,0x68,0x46,0x11,0x34,0xa3,0xa4,0x07,0x9d,0xf5,0x16,0xef,0xf8,0xb8,0xe7,0xf7,
	0x04,0xcd,0x28,0x82,0x66,0x94,0xac,0xa0,0xab,0xed,0xe1,0x52,0xd5,0x42,0x14,0x34,
	0x47,0xaa,0x85,0x28,0x68,0x8e,0x64,0x3d,0x55,0x97,0xe6,0x08,0x9a,0xb7,0x08,0x9a,
	0x51,0x04,0xcd,0x28,0x41,0x1c,0x87,0x41,0xa7,0x9f,0x27,0xfd,0xbb,0x18,0x48,0xd0,
	0x8c,0x22,0x68,0x46,0x49,0x0f,0x68,0xef,0xe9,0xbc,0x37,0x47,0xd0,0x7c,0x13,0x34,
	0xa3,0x08,0x1a,0x00,0x00,0x00,0x00,0x00,0xb8,0xd0,0xe1,0xff,0x17,0xe9,0x93,0xab,
	0x99,0x71,0x8b,0x8b,0x08,0x3a,0x36,0xe3,0x16,0x17,0x11,0x74,0x6c,0xc6,0x2d,0x2e,
	0xf2,0x5e,0xd0,0x33,0x08,0xba,0x19,0x41,0xc7,0x04,0xdd,0x8c,0xa0,0x63,0x82,0x6e,
	0x26,0xf8,0xc1,0x9e,0x7c,0xb4,0x37,0xf9,0xc9,0xe6,0x60,0xed,0xdd,0x2b,0x6b,0x0e,
	0xa5,0x09,0x7a,0x75,0x0e,0xa5,0x09,0x7a,0x75,0x0e,0xa5,0x95,0x0d,0xfa,0xbd,0x3d,
	0x4b,0x04,0xdd,0x8c,0xa0,0x63,0x82,0x6e,0x46,0xd0,0x31,0x41,0x37,0x33,0x2c,0xe8,
	0x60,0xf3,0x5e,0x9a,0x82,0x6e,0x46,0xd0,0x31,0x41,0x37,0x23,0xe8,0x98,0xa0,0x9b,
	0x99,0x1a,0x74,0xd6,0xc1,0x68,0x46,0xd0,0x82,0x1e,0x45,0xd0,0x82,0x9e,0x69,0xaf,
	0xec,0xbd,0x81,0x7b,0x9b,0xf7,0xf6,0x04,0x2b,0xfd,0x16,0x14,0x22,0xe8,0xf3,0x5b,
	0x50,0x88,0xa0,0xcf,0x6f,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x30,0xdc,0x3f,0xb1,0x4c,0xe2,0x64,0x52,0x0a,0xef,0xfd,0x00,0x00,
	0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};

constexpr uint8_t single_player[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x87,0x08,0x02,0x00,0x00,0x00,0xa7,0x10,0x43,
	0xbc,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x03,0xe1,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0xdc,0xe1,
	0xb1,0xe2,0x36,0x14,0x06,0xd0,0x57,0x50,0x8a,0x48,0x35,0x29,0x24,0x05,0xa5,0x86,
	0x74,0xb0,0x25,0x6d,0xf2,0x23,0x43,0xbc,0x48,0xba,0xba,0x92,0x85,0x9f,0x0c,0xe7,
	0x0c,0xb3,0x03,0xb6,0x7c,0x25,0xcc,0x87,0xac,0xc7,0xc0,0x7e,0x7d,0x01,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x6c,0xee,0xe7,0x5f,0xfd,0x1b,0x1f,0xee,0x4e,0xd9,
	0x10,0x68,0xba,0xee,0x94,0x0d,0x81,0xa6,0xeb,0x06,0xd9,0x18,0x1a,0xd9,0x76,0xa3,
	0xff,0x60,0x57,0x46,0xaa,0xec,0x42,0xa0,0x59,0x4c,0xa0,0xeb,0x04,0xfa,0xa6,0x04,
	0xba,0x6e,0xf9,0xc8,0x4e,0xae,0xc5,0x33,0x87,0x5f,0x59,0x67,0xd5,0xf3,0xda,0xed,
	0xfc,0x0c,0x0d,0x35,0x33,0x8c,0x5d,0x08,0xb4,0x40,0x77,0xfb,0xca,0x0c,0x63,0x17,
	0x02,0x2d,0xd0,0xdd,0xbe,0x32,0xc3,0xd8,0xc5,0xaa,0x91,0x65,0xea,0x68,0x33,0x9a,
	0xda,0x4c,0x9b,0xe5,0x91,0x5a,0xfe,0x74,0x2e,0x25,0xd0,0xfb,0xb4,0x29,0x1b,0x67,
	0xda,0x08,0xf4,0x2f,0x04,0x7a,0x9f,0x36,0x65,0xe3,0x4c,0x1b,0x81,0xfe,0x85,0x40,
	0x8f,0xb6,0xc9,0xdc,0xe6,0xfa,0x1a,0x72,0x65,0xa0,0xe7,0x9e,0xf2,0xf7,0x10,0x68,
	0x81,0xee,0x56,0x16,0xe8,0x1d,0x83,0xb8,0xaa,0x8d,0x40,0xbf,0x4f,0xa0,0xb7,0x0d,
	0xd9,0xf7,0xb6,0xc9,0x58,0x75,0x9e,0x97,0x0f,0x6c,0x8e,0x40,0xbf,0x73,0x9b,0x0c,
	0x81,0xbe,0x88,0x40,0x0b,0xf4,0x84,0x7d,0x03,0xfd,0x50,0x0e,0x71,0x6e,0xd0,0x27,
	0xeb,0x6c,0x1b,0xd6,0x4d,0xce,0xcf,0x50,0xc1,0xd7,0x11,0x68,0x81,0x5e,0x5c,0x27,
	0x53,0xf0,0x75,0x04,0x5a,0xa0,0x17,0xd7,0xc9,0x14,0x7c,0x9d,0x1b,0x04,0x1a,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x52,0xbf,
	0xf2,0x77,0xf3,0xdf,0x06,0xdc,0x86,0x97,0x8a,0xb7,0x22,0xd0,0xbc,0x95,0x63,0xa0,
	0x7f,0xff,0xed,0xff,0x2b,0xec,0xdf,0x7f,0x3e,0x6f,0x3f,0x6e,0x19,0x75,0xbe,0xc2,
	0xeb,0xaa,0xf1,0x56,0x1e,0x81,0x7e,0x4a,0xc9,0xbf,0x0f,0x17,0x26,0x46,0xa0,0xb9,
	0x48,0x2b,0xd0,0x47,0x65,0xd6,0x8f,0x7f,0x2d,0x95,0x73,0x79,0xb9,0xab,0x2c,0xde,
	0xba,0x1a,0x74,0x4b,0xe5,0xab,0x1d,0x9b,0x05,0x4f,0x21,0xd3,0x9e,0xdb,0x98,0x58,
	0x72,0xb4,0x5e,0xf8,0x6e,0x80,0x86,0xa2,0x93,0xaf,0x96,0x39,0x6a,0xe8,0xfe,0xe3,
	0x3c,0x08,0xf4,0xfd,0x54,0xff,0x28,0x9c,0x8b,0xe3,0x68,0xa0,0x5b,0x13,0x70,0xdc,
	0x4b,0xf9,0xb0,0x2c,0x98,0x19,0x5b,0xab,0xbd,0x1c,0xdf,0xdb,0x71,0xc9,0x91,0x49,
	0xf0,0x57,0x22,0x6a,0xdd,0x25,0x47,0xe6,0x82,0x9e,0x5f,0x72,0x8c,0x8e,0x27,0x73,
	0x4d,0xe0,0xae,0x16,0x2e,0x39,0x8e,0x5a,0x93,0x68,0xbc,0x8a,0x8d,0x2b,0x04,0x7b,
	0x83,0xf4,0xe7,0x7b,0x17,0xe8,0x77,0x50,0x5d,0x72,0x4c,0xeb,0xc6,0x74,0xb4,0xd4,
	0x92,0x22,0x16,0x12,0x1f,0x64,0x6d,0xa0,0xe1,0x9b,0x09,0x34,0x00,0x00,0x00,0x00,
	0x00,0x00,0xd0,0xd4,0xfd,0xf6,0xcf,0xc2,0x8e,0x7c,0xe9,0x82,0x8b,0x5c,0x90,0x36,
	0x81,0xe6,0x3a,0x4f,0x69,0x2b,0xc3,0x57,0x7e,0xa9,0x3a,0xfe,0x76,0x68,0xf5,0xab,
	0xaa,0xd5,0x6f,0xee,0x57,0x7f,0xec,0x18,0x77,0xf7,0xd8,0xdb,0xfa,0xee,0xa8,0x77,
	0xce,0xa7,0xcb,0x07,0xba,0x15,0xbb,0xea,0xb1,0xdd,0xc6,0xdd,0xca,0xc1,0xc6,0xee,
	0x98,0xf9,0x5c,0xad,0x70,0x04,0x77,0xca,0xa3,0xca,0x8d,0x41,0xe3,0x72,0x22,0x6f,
	0x65,0x77,0xf4,0x97,0x29,0x66,0x68,0x16,0x07,0xba,0xbb,0xe4,0xe8,0xae,0x2e,0xaa,
	0x29,0x6f,0x8d,0xb6,0xfb,0x74,0xf8,0x38,0xd5,0x35,0xc6,0xf1,0x97,0xd2,0xf1,0xf2,
	0x37,0x2e,0x55,0xdd,0x15,0xff,0xe4,0x24,0xb9,0x86,0x2e,0x07,0x6c,0x86,0x66,0xb1,
	0x33,0xb3,0xa3,0x35,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x55,0x3f,0x7f,0xfc,0xf1,0xb8,0x5d,0xd9,0x69,0x77,0xcb,
	0x65,0x26,0xba,0x7e,0xe9,0x68,0xbf,0xf1,0x54,0xbc,0x83,0xe3,0xe9,0xbb,0xec,0x54,
	0xe6,0x03,0x7d,0xfd,0xab,0x9b,0xe9,0x51,0xa0,0xf7,0x55,0x0d,0xf4,0xf1,0xce,0x53,
	0x83,0x72,0x2e,0xcf,0x6f,0x7c,0xda,0xdb,0x7a,0x58,0x16,0xe9,0x8e,0x21,0xee,0x3a,
	0x7e,0xff,0x54,0x8b,0xc7,0x3d,0x76,0x2f,0x6b,0xc7,0xbd,0xa3,0x67,0x6c,0xee,0x84,
	0xf3,0x9f,0x20,0xd0,0xe5,0x79,0x6c,0xb5,0x0c,0x6a,0x06,0x9d,0x96,0xff,0x76,0x4b,
	0x75,0xaf,0x27,0xc9,0x8d,0x41,0x9d,0xd1,0x67,0x1d,0xf7,0x18,0xdc,0x39,0xbf,0x91,
	0x8a,0x60,0x62,0x08,0x5a,0x9e,0x9c,0x45,0xe2,0x97,0xb9,0xda,0xb2,0x7c,0x98,0x99,
	0x1d,0x33,0xdd,0xe5,0x03,0x9d,0x1f,0xc6,0xaa,0x40,0xcf,0x5d,0xa3,0x3e,0xda,0x99,
	0x79,0x6e,0x61,0xa9,0x85,0x81,0x2e,0x1b,0xac,0x0a,0xf4,0xc4,0x0c,0x9d,0x3c,0x76,
	0x6e,0x32,0x96,0xe9,0x8a,0xa1,0xec,0x06,0xb3,0x45,0x77,0x63,0x5c,0x3f,0x78,0x6d,
	0x86,0xe6,0xa7,0x56,0xd7,0x8f,0x2d,0xd5,0x5d,0xa3,0x3d,0x9e,0x09,0x74,0xeb,0x29,
	0x24,0x27,0xe3,0xee,0xb9,0x05,0x00,0x60,0x17,0x43,0x2b,0xc8,0x56,0x85,0x0f,0x59,
	0xff,0x05,0x0b,0x6e,0x76,0x91,0x7c,0x85,0xe6,0x76,0x9d,0x6c,0x7c,0x41,0x9d,0x9d,
	0x7b,0x64,0x46,0x66,0x86,0x8e,0x27,0xe0,0xd6,0xc6,0xd1,0x4f,0x4e,0xca,0x82,0x41,
	0x8f,0x13,0x1f,0xe2,0xc6,0x1f,0x41,0xcc,0x8d,0x3c,0x1e,0xf3,0xf1,0x90,0xb7,0xbf,
	0x7c,0xed,0xa2,0xb5,0x60,0xe8,0x7e,0x04,0x56,0x56,0x68,0xb5,0x4f,0x96,0x3d,0xe6,
	0x20,0xee,0x31,0x59,0x27,0xd9,0x51,0x66,0x63,0xb5,0x6c,0xb9,0xb7,0x35,0xaa,0x6a,
	0x71,0x5e,0x62,0x22,0x79,0x41,0x85,0x89,0xb2,0xe5,0xdb,0x29,0x33,0x99,0x9d,0x09,
	0x74,0x7c,0xd4,0x74,0xa0,0x83,0x06,0x02,0x7d,0x9d,0x6f,0x0f,0x74,0xb2,0x60,0xdc,
	0x66,0x93,0x19,0x3a,0x1e,0x64,0xab,0x19,0x2b,0x0d,0xbd,0x7e,0xdd,0x15,0x6d,0x37,
	0x16,0x65,0x9d,0xee,0x6a,0x27,0x18,0x79,0x5c,0xa7,0x35,0xce,0x78,0xa3,0x40,0x03,
	0x00,0x00,0x00,0x00,0x00,0x30,0xe7,0x1f,0xa8,0x6c,0xfe,0x36,0x80,0x31,0xe5,0x33,
	0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82
};


constexpr uint8_t two_players[] = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,
	0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x87,0x08,0x02,0x00,0x00,0x00,0xa7,0x10,0x43,
	0xbc,0x00,0x00,0x00,0x01,0x73,0x52,0x47,0x42,0x01,0xd9,0xc9,0x2c,0x7f,0x00,0x00,
	0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x0b,0x13,0x00,0x00,0x0b,0x13,0x01,0x00,
	0x9a,0x9c,0x18,0x00,0x00,0x03,0xd7,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0xdd,0xe1,
	0x91,0xd3,0x30,0x10,0x06,0x50,0x0a,0xa2,0x08,0xaa,0xa1,0x10,0x0a,0xa2,0x06,0x3a,
	0xa0,0xa4,0xe3,0xc7,0x31,0x19,0x9f,0x65,0xad,0xd6,0xb2,0xe2,0x28,0xf6,0x7b,0x93,
	0x61,0x72,0xce,0x7a,0xa5,0x38,0x5f,0x14,0x11,0x66,0x8e,0x6f,0xdf,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x26,0xf7,0xf1,0xbb,0x7d,0xe3,0xe6,0xde,0x29,0x1b,
	0x02,0x4d,0xd3,0x3b,0x65,0x43,0xa0,0x69,0x7a,0x83,0x6c,0xec,0x9a,0xd9,0x74,0xb3,
	0xbf,0xb1,0x33,0x23,0x55,0x0e,0x21,0xd0,0x0c,0x26,0xd0,0xdb,0x04,0xfa,0x4d,0x09,
	0xf4,0xb6,0xe1,0x33,0x3b,0xb8,0x17,0xcf,0x9c,0x7e,0x66,0x9f,0x51,0xcf,0x6b,0xb6,
	0xeb,0xb3,0x6b,0xaa,0x99,0x69,0xcc,0x42,0xa0,0x05,0xba,0x39,0x56,0x66,0x1a,0xb3,
	0x10,0x68,0x81,0x6e,0x8e,0x95,0x99,0xc6,0x2c,0x46,0xcd,0x2c,0xd3,0x47,0xcd,0xde,
	0xd4,0x66,0x6a,0x86,0x47,0x6a,0xf8,0xd3,0x39,0x95,0x40,0xcf,0x53,0x53,0x16,0x67,
	0x6a,0x04,0xfa,0x0b,0x81,0x9e,0xa7,0xa6,0x2c,0xce,0xd4,0x08,0xf4,0x17,0x02,0xbd,
	0xb7,0x26,0x73,0xeb,0x1b,0x6b,0x97,0x33,0x03,0xdd,0xf7,0x94,0x5f,0x43,0xa0,0x05,
	0xba,0xd9,0x59,0xa0,0x67,0x0c,0xe2,0xa8,0x1a,0x81,0xbe,0x4e,0xa0,0xa7,0x0d,0xd9,
	0x6b,0x6b,0x32,0x46,0x5d,0xe7,0xe1,0x13,0xeb,0x23,0xd0,0x57,0xae,0xc9,0x10,0xe8,
	0x93,0x08,0xb4,0x40,0x77,0x98,0x37,0xd0,0x0f,0xe5,0x14,0xfb,0x26,0x7d,0xb0,0xcf,
	0xb4,0x61,0x9d,0xe4,0xfa,0xec,0x6a,0xf8,0x3c,0x02,0x2d,0xd0,0x83,0xfb,0x64,0x1a,
	0x3e,0x8f,0x40,0x0b,0xf4,0xe0,0x3e,0x99,0x86,0xcf,0xf3,0x06,0x81,0x06,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xd7,
	0xfb,0xf1,0xfd,0xff,0xef,0xe9,0xf9,0xf3,0xeb,0xcb,0xc1,0xe5,0x8f,0xbb,0xba,0xf5,
	0x9d,0x38,0xbc,0x09,0x77,0xb4,0x8c,0xce,0x3c,0x59,0x14,0x68,0x3a,0xd5,0xa2,0xb3,
	0x0a,0xfa,0xe3,0xb7,0xad,0xad,0x56,0xf1,0xf2,0xf8,0xaa,0xe1,0xe6,0xf2,0x1f,0x77,
	0xc8,0x34,0xa9,0xbd,0x0f,0x83,0xca,0xcf,0x87,0xbc,0x4f,0xae,0xaf,0xb9,0xe5,0xd8,
	0xbc,0xdf,0x2c,0x08,0xee,0xe7,0x47,0xa9,0xd5,0xe7,0xa7,0xf4,0x79,0xc7,0x92,0x7f,
	0x47,0xbb,0x72,0x99,0x2c,0x5e,0xfe,0x2e,0xcd,0xee,0x40,0x97,0x0b,0x79,0x6d,0x4a,
	0xe5,0x70,0x16,0xe6,0x7b,0x39,0xb2,0xd0,0x36,0xb7,0x1c,0xcd,0x75,0x31,0xb3,0xe5,
	0xc8,0x8f,0xde,0xfc,0x10,0xe0,0x16,0xfa,0xb6,0x1c,0xab,0x0e,0xc1,0xb9,0xf1,0x1e,
	0xba,0x96,0xb6,0xcc,0x26,0x3e,0x33,0x9c,0x40,0x93,0x15,0x87,0x35,0xd9,0xe1,0xc8,
	0xb9,0x07,0x47,0x07,0x00,0x00,0x00,0x00,0x00,0x68,0xca,0xfc,0xbf,0xab,0x77,0xb8,
	0x71,0x11,0x5e,0x4b,0x2e,0x45,0xa0,0xb9,0x94,0x65,0xa0,0xff,0xfc,0x5a,0x7f,0x10,
	0xff,0xf8,0xfe,0xdc,0xd1,0x3f,0x47,0x7c,0xf6,0x28,0xdc,0x48,0xb9,0x42,0x9f,0x19,
	0x32,0x81,0x66,0xb0,0x4c,0xa0,0x37,0x8f,0x3c,0x7e,0x5c,0xae,0xeb,0x9b,0xd1,0x5c,
	0x2d,0xfc,0xcd,0xce,0x65,0xe5,0xb2,0x6c,0x75,0xca,0x90,0x7a,0xae,0xe3,0x60,0xa0,
	0x83,0xe8,0x6c,0x9e,0xbe,0x19,0xaf,0xe0,0x6d,0x10,0x8f,0xd2,0xbc,0xff,0x88,0x6f,
	0x3c,0x43,0xae,0x63,0x57,0xa0,0x97,0x77,0x9e,0x14,0xe8,0xcd,0xe5,0x3c,0x0e,0x6e,
	0xad,0x3e,0x98,0x8c,0x15,0xfa,0xb2,0x4e,0x0b,0x74,0x66,0xcb,0x91,0xdc,0x2a,0xec,
	0x7a,0x0b,0xd5,0x58,0xad,0xaf,0x29,0xf9,0x97,0xc2,0x32,0xd3,0xab,0x87,0x9a,0x7b,
	0xe8,0x5a,0x74,0x32,0xc1,0x4d,0xd6,0x37,0x77,0x32,0xcd,0xe6,0xbc,0xbd,0x13,0xbe,
	0x87,0x1e,0xb2,0x16,0xda,0x2a,0x90,0xe2,0x1f,0x56,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x66,0x3e,0xfe,0xfe,0x7c,0xdc,
	0xce,0x1c,0xb4,0x79,0xe4,0x34,0x1d,0x43,0x3f,0x75,0xb6,0x2f,0xbc,0x14,0x57,0xb0,
	0xbc,0x7c,0xa7,0x5d,0xca,0x7c,0xa0,0xcf,0x7f,0x75,0x33,0x23,0x0a,0xf4,0xbc,0x36,
	0x03,0xbd,0xbc,0xb3,0x2a,0x28,0xd7,0xf2,0xfc,0xc1,0xd5,0xa3,0xb5,0x1f,0xcb,0x26,
	0xcd,0x39,0xc4,0x43,0xc7,0xef,0x9f,0xcd,0xe6,0xf1,0x88,0xcd,0x8f,0xb5,0xe5,0xa3,
	0x7b,0xaf,0x58,0xdf,0x05,0xe7,0xbf,0x20,0xd0,0xe5,0x75,0xac,0x55,0x06,0x3d,0x83,
	0x41,0xcb,0x3f,0x9b,0xad,0x9a,0x9f,0x27,0xc9,0x83,0x41,0x9f,0xbd,0xcf,0x3a,0x1e,
	0x31,0xb8,0x73,0xfc,0x20,0x1b,0x82,0x85,0x21,0xa8,0x3c,0xb8,0x8a,0xc4,0x2f,0xf3,
	0x66,0x65,0xf9,0x63,0x66,0x75,0xcc,0x0c,0x97,0x0f,0x74,0x7e,0x1a,0xa3,0x02,0xdd,
	0xf7,0x19,0x75,0x6b,0x47,0xd6,0xb9,0x81,0xad,0x06,0x06,0xba,0x2c,0x18,0x15,0xe8,
	0x8e,0x15,0x3a,0x79,0x6e,0xdf,0x62,0x2c,0xd3,0x1b,0x76,0x65,0x37,0x58,0x2d,0x9a,
	0x07,0xe3,0xfe,0xc1,0x6b,0xb3,0x6b,0x7d,0xaa,0x0d,0xfd,0x38,0xb2,0xf9,0xd0,0xde,
	0x11,0x8f,0x04,0xba,0xf6,0x14,0x92,0x8b,0x71,0xf3,0xda,0x02,0x00,0x30,0x8b,0x5d,
	0x3b,0xc8,0x5a,0x87,0x9b,0xec,0xff,0x82,0x0d,0x37,0xb3,0x48,0xbe,0x42,0x7d,0x0f,
	0x1d,0x2c,0x3e,0xa1,0xcf,0xcc,0x23,0xd2,0x23,0xb3,0x42,0xc7,0x0b,0x70,0xed,0xe0,
	0xde,0x6f,0x4e,0xca,0x86,0xc1,0x88,0x1d,0x5f,0xe2,0xc6,0x5f,0x41,0xf4,0xcd,0x3c,
	0x9e,0xf3,0xf2,0x94,0xcb,0x7f,0x7c,0xcd,0xa2,0xb6,0x61,0x68,0x7e,0x05,0x56,0x76,
	0xa8,0xd5,0x27,0xdb,0x2e,0x73,0x10,0x8f,0x98,0xec,0x93,0x1c,0x28,0x73,0x70,0xb3,
	0x6d,0xf9,0x68,0x6d,0x56,0x9b,0xcd,0x79,0x8a,0x8e,0xe4,0x05,0x1d,0x3a,0xda,0x96,
	0x6f,0xa7,0xcc,0x62,0x76,0x24,0xd0,0xf1,0x59,0xdd,0x81,0x0e,0x0a,0x04,0xfa,0x3c,
	0x2f,0x0f,0x74,0xb2,0x61,0x5c,0x33,0xc9,0x0a,0x1d,0x4f,0xb2,0x56,0xc6,0x48,0xbb,
	0x5e,0xbf,0xe6,0x8e,0xb6,0x19,0x8b,0xb2,0x4f,0x73,0xb7,0x13,0xcc,0x3c,0xee,0x53,
	0x9b,0x67,0x7c,0x50,0xa0,0x01,0x00,0x00,0x00,0x00,0x00,0xe8,0xf3,0x0f,0x69,0xe0,
	0xcd,0x9d,0x0f,0xe3,0xb9,0x3e,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,
	0x60,0x82
};
