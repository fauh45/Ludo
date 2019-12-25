#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// #include <stdbool.h>

/* OS Detection to make sure screen clearing, sleep, and curses function works */
#if defined(__linux__) || defined(unix)
#include <unistd.h>

// Ncurses for Linux user
#include <ncurses.h>
char *os = "linux";
#else
#ifdef _WIN32
#include <windows.h>

// PDCurses (port of ncurses) for windows user
#include "PDCurses/curses.h"
char *os = "windows";
#else
#ifdef __APPLE__
#include <unistd.h>

// Ncurses for the macs as well
#include <ncurses.h>
char *os = "macos";
#else
#include <unistd.h>
#include <ncurses.h>
char *os = "unknown";
#endif
#endif
#endif

// Colour for boards, defined for easier reading
#define BOARD_BLUE 1   // White foreground, Blue background
#define BOARD_RED 2    // White foreground, Red background
#define BOARD_GREEN 3  // White foreground, Green background
#define BOARD_YELLOW 4 // White foreground, Yellow background
#define BOARD_WHITE 5  // Black foreground, White background
#define BOARD_BLACK 6  // White foreground, Black background

/* Variable Declaration */
typedef struct
{
    char col;   //Color of the tokens, corresponding to the player colour
    int ind;    //Index of the tokens (1 - 4) for each player
    int pos;    //Position of the tokens (global position)
    int relpos; //Realtive Position from the start position
    bool safe;  //Is the token in safezone
} Tokens;

typedef struct
{
    char col;      //Color of the player
    bool comp;     //Is the player a computer or not
    char comptype; // Type of computers
    int move;      //How much the player has move
} Player;

WINDOW *board[15][15]; // Ludo board (graphically)
WINDOW *options;       // The box mmenu below of the board for the player to choose many things

/*
    Player array consist of all of the player
    The array is sorted according to colour
    red -> index 0
    green -> index 1
    yellow -> index 2
    blue -> index 3
*/
Player players[4]; // List of players

/*
    Token arrays for each colours

    char col value per colour :
    Red -> r
    Green -> g
    Yellow -> y
    Blue -> b
*/
Tokens red[4];
Tokens green[4];
Tokens yellow[4];
Tokens blue[4];

// Number of bots
int numberOfBots;

/* Function Prototype */

/*
   Initial State : Screen contains other elements
   Final State : Screen are clear
   Note : Clear using system
   Author : Muhammad Fauzan L.
*/
void ClearScreen();

/*
   Initial State : Not waiting
   Final State : Waiting is complete
   Author : Muhammad Fauzan L.
*/
void WaitForSecond(int time);

/*
   Input : None
   Output : Randomized number from 1 to 6 representing dice rolls
   Author : Muhammad Fauzan L.
*/
int RollADice();

/*
   Input : None
   Output : True if Console have the capabilities of colour, otherwise False
   Author : Muhammad Fauzan L.
*/
bool hasColour();

/*
   Input : 
    @line how much rows the new window have
    @collumns how much collumns the new window have
    @starty the starting row position of the new window
    @startx the starting collumns position of the new window 
            (starting position are the top leftmost position of new window)
   Output : New window in WINDOW data type
   Author : Muhammad Fauzan L.
*/
WINDOW *newWindow(int line, int collumns, int starty, int startx);

/*
   Initial State : No board in the screen
   Final State : Board in the screen
   Author : Muhammad Fauzan L.
*/
void showBoard();

/*
   Initial State : Board is in the screen
   Final State : Board no longer in the screen
   Author : Muhammad Fauzan L.
*/
void destroyBoard();

/*
    Initial State : Logo is not shown
    Final State : Logo is in the screen
    Author : Muhammad Fauzan L.
*/
void showLogo();

/*
    Initial State : Menu is not shown
    Final State : Menu is shown and user have entered a choice
    Input and Outout :
    @choice an int with the value of choice, 0 means new game, 1 means resume, 2 means highscore, and 3 means exit
    Author : Muhammad Fauzan L.
*/
void showMainMenu(int *choice);

/*
    Input : None
    Output : The User choice on showMainMenu and Clear screen
    Author : Muhammad Fauzan L.
*/
int getUserChoiceinMenu();

/*
    Initial State : Cleared screen
    Final State : New Game menu is shown and the user have enetered a choice
    Input and Output :
    @choice an array with lenght of 3, consisting of bot id, 0 for Jörgen, 1 for Hans, and 2 for Müller
            value -1 means the slot must be empty
    Author : Muhammad Fauzan L.
*/
void showNewGameMenu(int choice[3]);

/*
    Input : 
    @win the target window to get the middle horizontal position
    @width the width of object
    Output : leftmost position of the object such that the object is in the middle of said screen
    Author : Muhammad Fauzan L.
*/
int getMiddleX(WINDOW *win, int len);

/*
    Input : File name
    Output : True if file exist and false if it doesn't
    Author : Muhammad Fauzan L.
*/
bool isFileExist(char fileName[]);

/*
    Initial State : Bot data are empty (value unknown)
    Input : 
    @botIndexes integer contains bot index from 0 to 2
                0 for Jörgen, 1 for Hans, and 2 for Müller
    @colour integer containing the colour index data, 0 -> red, 1 -> green, 2 -> yellow, 3 -> blue.
    Final State : Bot player data are initialized
    Author : Muhammad Fauzan L.
*/
void initBotPlayerData(int botIndexes, int colour);

/*
    Initial State : Human player data are empty (value unknown)
    Input : 
    @colour integer containing the colour index data, 0 -> red, 1 -> green, 2 -> yellow, 3 -> blue.
    Final State : Human player data are initialized
    Author : Muhammad Fauzan L.
*/
void initHumanPlayerData(int colour);

/*
    Initial State : Player data (bot and human) are empty (value unknown)
    Input :
    @botIndexes integer array with maximum lenght of 3 contains bot index from -1 to 2
                0 for Jörgen, 1 for Hans, and 2 for Müller. value -1 means there's no player there
    Final State : Player data (bot and human) are initialized with inputted data
    Author : Muhammad Fauzan L.
*/
void initPlayerData(int botIndexes[3]);

/*
    Input : 
    @token the current token
    @index the place where the current tokens wants to move
    Output : token data of opponents if there's opponents on the index,
             otherwise token data with colour 'n'
    Author : Muhammad Fauzan L.
*/
Tokens isThereOpponents(Tokens token, int index);

/*
    Initial State : Token in initial position
    Input :
    @diceNum the dice number that shown up
    @whosTurn [pemain yang sedang giliran]
    Final State : Token in the desired position, back to home base if failed the suit, or can't move
    Author : Marissa Nur Amalia
*/
void moveToken();

/*
    Initial State : [token ingin dicek posibilitas geraknya (?)]
    Input :
    @diceNum the dice number that shown up
    @pos the token's position that want to check
    @safe is the token in the safe zone or not
    Output : the possibility of token to move, to out from home, or can't move
    Author : Marissa Nur Amalia
*/
char possibleMove(int diceNum, int pos, bool safe);

/*
    Initial State : The input number of choice is want to be checked
    Input :
    @choice choice of tokens
    Output : is the choice valid or not
    Author : Marissa Nur Amalia
*/
bool validateInputToken(int choosenToken);

/*
    Initial State : Token in initial position
    Input :
    @diceNum the dice number that shown up
    @whosTurn [pemain yang sedang giliran]
    @numOfToken the index number of token that want to move
    Final State : the token is in desired position
    Author : Marissa Nur Amalia
*/
void moveForward(int diceNum, int whosTurn, int numOfToken);

/*
    Initial State : Token in initial position
    Input :
    @numOfToken the index number of token that want to move
    @whosTurn [pemain yang sedang giliran]
    Final State : Token is back to home
    Author : Marissa Nur Amalia
*/
void toHomeBase(int numOfToken, int whosTurn);

/*
    Initial State : Token is in home
    Input :
    @diceNum the dice number that shown up
    @whosTurn [pemain yang sedang giliran]
    Final State : Token is out home
    Author : Marissa Nur Amalia
*/
void outFromHomeBase(int numOfToken, int whosTurn);

/*
    Initial State : Token is in home
    Input :
    @diceNum the dice number that shown up
    @whosTurn [pemain yang sedang giliran]
    @numOfToken
    Final State : Token is in safe zone
    Author : Marissa Nur Amalia
*/
void moveToSafeZone(int numOfToken, int whosTurn, int diceNum);

/*
    Input :
    @count the number of the players take turn
    Output : players that takes turn
    Author : Marissa Nur Amalia
*/
int whosTurn(int count);

/*
    Input : 
    @token the current token
    @diceroll number from 1 to 6 of the dice roll
    Output : true if the token must be transitioned into safezone, otherwise false
    Author : Muhammad Fauzan L.
*/
bool isTransitionToSafezone(Tokens token, int diceroll);

/*
    Input : 
    @token1 the first token
    @token2 the second token
    Output : Distance between first token and second token
    Author : Muhammad Fauzan L.
*/
int distanceBetween(Tokens token1, Tokens token2);

/*
    Initial State : Option box is not shown
    Final State : Option box is shown
    Author : Muhammad Fauzan L.
*/
void showOptionBox();

/*
    Initial State : Option box is shown
    Final State : Option box is not shown and destroyed
    Author : Muhammad Fauzan L.
*/
void destroyOptionBox();

/*
    Initial State : Option box contains other content
    Final State : Option box is clear of any content except the border
    Author : Muhammad Fauzan L.
*/
void clearOptionBox();

/*
    Input :
    @player1 player one suit choice
    @player2 player two suit choice
    suit choice consist of a number from 1 to 3, where
    1 -> Kertas
    2 -> Gunting
    3 -> Batu
    Output : 1 if player one won, 2 if player 2 won, and 0 if it's a draw
    Author : Muhammad Fauzan L.
*/
int suitCheck(int player1, int player2);

/*
    Initial State : Suit menu is not shown
    Final State : User has inputted a choice
    Output : 
    @input 1 to 4 of suit choice as described in suitcheck
    Author : Muhammad Fauzan L.
*/
void suitMenu(int *choice);

/*
    Input : None
    Output : Random number from 1 to 3, symbolizing the suit
    Author : Muhammad Fauzan L.
*/
int suitRandom();

/*
    Initial State : The inputted string are not shown in option box
    Final State : The inputted string shown in option box
    Input :
    @input[] the string that is going to be shown at the option box
    @x row position
    @y position in collumns
    Author : Muhammad Fauzan L.
*/
void printToOptionBox(char input[], int x, int y);

/*
    Initial State : Position of token in board is known
    Final State : The coresponding coordinate in board is outputed
    Input :
    @token the token that position that's going to be searched
    Output :
    @x position in rows
    @y position in collumns
    Author : Muhammad Fauzan L.
*/
void positionToCoordinate(Tokens token, int *x, int *y);

/*
    Initial State : Tokens are not shown on the board
    Final State : Tokens are shown on the board
    Author : Muhammad Fauzan L.
*/
void printTokens();

/*
    Input :
    @index index of the tokens
    Output : A, B, C, or D depending the index of the tokens
    Author : Muhammad Fauzan L.
*/
char tokenShown(int index);

int main()
{
    // Curses mode intialization
    initscr();

    // Check for console capabilities
    if (!has_colors())
    {
        printw("No color support on console");
        exit(1);
    }

    // Starts out colour mode of curses
    start_color();

    // Initalize all the color pairs for the board
    init_pair(BOARD_BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(BOARD_RED, COLOR_WHITE, COLOR_RED);
    init_pair(BOARD_GREEN, COLOR_WHITE, COLOR_GREEN);
    init_pair(BOARD_YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(BOARD_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BOARD_BLACK, COLOR_WHITE, COLOR_BLACK);

    switch (getUserChoiceinMenu())
    {
    case 0:
        /*
            Play new game here
            While not gameover, showboard and option box, check who's turn, play
        */
        break;

    case 1:
        /*
            Play resumed game here
            Check if file exist, check if file empty, load save game into variable, play as usual
        */
        break;

    case 2:
        /*
            Show highscore here
        */
        break;

    case 3:
        /*
            Exit the game here
            Just do nothing as the endwin and return 0 is right down of this selection
        */
        break;
    }

    // Curses mode end
    endwin();
    return 0;
}

/* Function Body */

WINDOW *newWindow(int line, int collumns, int starty, int startx)
{
    WINDOW *win;

    win = newwin(line, collumns, startx, starty);
    // wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);

    wrefresh(win);
    return win;
}

void showBoard()
{
    int i;            // Rows of Board
    int j;            // Collumn of Board
    int x = 1, y = 1; // Initial position of board, top left of board

    // Create new window for each 14 * 14 board
    // With the size each of the board, 2 rows and 4 collumns
    // Starting from (1, 1)
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 15; j++)
        {
            board[i][j] = newWindow(2, 4, x, y);
            x += 4;
            refresh();
        }
        x = 1;
        y += 2;
    }

    // Blue Corner
    for (i = 0; i <= 5; i++)
    {
        for (j = 0; j <= 5; j++)
        {
            if (i == 0 || i == 5)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else if (j == 0 || j == 5)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLUE));
                wrefresh(board[i][j]);
            }
        }
    }

    // Red Corner
    for (i = 0; i <= 5; i++)
    {
        for (j = 9; j <= 14; j++)
        {
            if (i == 0 || i == 5)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else if (j == 9 || j == 14)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_RED));
                wrefresh(board[i][j]);
            }
        }
    }

    // Green Corner
    for (i = 9; i <= 14; i++)
    {
        for (j = 9; j <= 14; j++)
        {
            if (i == 9 || i == 14)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else if (j == 9 || j == 14)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_GREEN));
                wrefresh(board[i][j]);
            }
        }
    }

    // Yellow Corner
    for (i = 9; i <= 14; i++)
    {
        for (j = 0; j <= 5; j++)
        {
            if (i == 9 || i == 14)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else if (j == 0 || j == 5)
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));
                wrefresh(board[i][j]);
            }
            else
            {
                wbkgd(board[i][j], COLOR_PAIR(BOARD_YELLOW));
                wrefresh(board[i][j]);
            }
        }
    }

    // Line on j = 6
    for (i = 0; i <= 14; i++)
    {
        if (i != 6 && i != 7 && i != 8)
        {
            wbkgd(board[i][6], COLOR_PAIR(BOARD_WHITE));
            wrefresh(board[i][6]);
        }
    }

    // Line on j = 8
    for (i = 0; i <= 14; i++)
    {
        if (i != 6 && i != 7 && i != 8)
        {
            wbkgd(board[i][8], COLOR_PAIR(BOARD_WHITE));
            wrefresh(board[i][8]);
        }
    }

    // Two of single blocks on j = 7
    wbkgd(board[0][7], COLOR_PAIR(BOARD_WHITE));
    wrefresh(board[0][7]);

    wbkgd(board[14][7], COLOR_PAIR(BOARD_WHITE));
    wrefresh(board[14][7]);

    // Line on i = 6
    for (j = 0; j <= 14; j++)
    {
        if (j != 6 && j != 7 && j != 8)
        {
            wbkgd(board[6][j], COLOR_PAIR(BOARD_WHITE));
            wrefresh(board[6][j]);
        }
    }

    // Line on i = 8
    for (j = 0; j <= 14; j++)
    {
        if (j != 6 && j != 7 && j != 8)
        {
            wbkgd(board[8][j], COLOR_PAIR(BOARD_WHITE));
            wrefresh(board[8][j]);
        }
    }

    // Two of single blocks on i = 7
    wbkgd(board[7][0], COLOR_PAIR(BOARD_WHITE));
    wrefresh(board[7][0]);

    wbkgd(board[7][14], COLOR_PAIR(BOARD_WHITE));
    wrefresh(board[7][14]);

    // Blue Safezone
    /* 
        For some unknown reason using loop doesnt work on this particular horizontal line    
    */
    // for (j = 1; j <= 6; j++)
    // {
    //     printw("%d", j);
    //     wbkgd(board[7][i], COLOR_PAIR(1));
    //     wrefresh(board[7][i]);
    // }

    wbkgd(board[7][1], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][1]);

    wbkgd(board[7][2], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][2]);

    wbkgd(board[7][3], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][3]);

    wbkgd(board[7][4], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][4]);

    wbkgd(board[7][5], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][5]);

    wbkgd(board[7][6], COLOR_PAIR(BOARD_BLUE));
    wrefresh(board[7][6]);

    // Red Safezone
    for (i = 1; i <= 6; i++)
    {
        wbkgd(board[i][7], COLOR_PAIR(BOARD_RED));
        wrefresh(board[i][7]);
    }

    // Green Safezone
    /* 
        For some unknown reason using loop doesnt work on this particular horizontal line    
    */
    // for (j = 8; j <= 13; j++)
    // {
    //     wbkgd(board[7][i], COLOR_PAIR(3));
    //     wrefresh(board[7][i]);
    // }

    wbkgd(board[7][8], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][8]);

    wbkgd(board[7][9], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][9]);

    wbkgd(board[7][10], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][10]);

    wbkgd(board[7][11], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][11]);

    wbkgd(board[7][12], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][12]);

    wbkgd(board[7][13], COLOR_PAIR(BOARD_GREEN));
    wrefresh(board[7][13]);

    // Yellow Safezone
    for (i = 8; i <= 13; i++)
    {
        wbkgd(board[i][7], COLOR_PAIR(BOARD_YELLOW));
        wrefresh(board[i][7]);
    }
}

void destroyBoard()
{
    int i; // Rows of Board
    int j; // Collumns of Board

    for (i = 0; i <= 14; i++)
    {
        for (j = 0; j <= 14; j++)
        {
            // Restore back the window to black
            wbkgd(board[i][j], COLOR_PAIR(BOARD_BLACK));

            // Clear out whatever inside the window
            werase(board[i][j]);

            // Update it
            wrefresh(board[i][j]);

            // Delete as it's not used anymore
            delwin(board[i][j]);
        }
    }
}

bool hasColour()
{
    return has_colors();
}

int getMiddleX(WINDOW *win, int len)
{
    int x;
    int y;
    getmaxyx(win, y, x);

    return (x / 2) - (len / 2);
}

void showLogo()
{
    WINDOW *logo;
    FILE *logo_file;
    int i;
    char logo_data[125];

    // As logo is an ASCII art it's easier to print from a file
    // Open it as read-only
    logo_file = fopen("logo.txt", "r");

    // File not found or cannot be openned
    if (logo_file == NULL)
    {
        erase();

        printw("Logo file not found / permission problems");
        getch();

        // Return back the console properties
        echo();
        curs_set(1);
        exit(3);
    }

    // New widow to put the logo
    // Horizontally center
    logo = newWindow(10, 41, getMiddleX(stdscr, 35), 1);

    // Show the logo
    for (i = 0; i < 9; i++)
    {
        if (fgets(logo_data, 125, logo_file) != NULL)
        {
            mvwprintw(logo, i, 0, logo_data);
        }
        else
        {
            // If somehow the file reaches End-Of-File
            mvwprintw(logo, i, 0, "EOF!");
        }
    }

    // Close the file and refresh the screen
    fclose(logo_file);
    wrefresh(logo);

    // Delete the window as it's not used anymore
    delwin(logo);
}

void showMainMenu(int *choice)
{
    WINDOW *mainmenu;
    int i, highlight = 0, position;

    // Options for the user
    char options[4][10] = {"New Game", "Resume", "Highscore", "Exit"};
    char item[9];
    char ch;

    // Show logo before the menu
    showLogo();

    // Create new window for the menu
    mainmenu = newWindow(10, 57, getMiddleX(stdscr, 57), 12);
    box(mainmenu, 0, 0);

    // Enable keypad mode for key up and down
    keypad(mainmenu, true);

    // Make sure what has been entered is not shown
    noecho();

    // Hide the cursor
    curs_set(0);

    // Loop until has been inputed
    while (1)
    {
        // Show the options
        for (i = 0; i < 4; i++)
        {
            position = getMiddleX(mainmenu, strlen(options[i]));

            // Show the highlighted options with highlights
            if (i == highlight)
                wattron(mainmenu, A_REVERSE);

            mvwprintw(mainmenu, i + 1, position, options[i]);

            if (i == highlight)
                wattroff(mainmenu, A_REVERSE);
        }

        // Get user input
        ch = wgetch(mainmenu);

        if (ch == (char)KEY_UP)
        {
            highlight--;
            highlight = (highlight < 0) ? 3 : highlight;
        }
        else if (ch == (char)KEY_DOWN)
        {
            highlight++;
            highlight = (highlight > 3) ? 0 : highlight;
        }
        else if (ch == 10) // Somehow the usage of KEY_ENTER doesn't work, so char 10 is used instead
        {
            *choice = highlight;
            break;
        }
    }

    // After user input clear out the border and window as
    // it is not used anymore
    wborder(mainmenu, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    delwin(mainmenu);
}

int getUserChoiceinMenu()
{
    int choice;

    // Show the user the menu and get the choice from it
    showMainMenu(&choice);

    // Clean out the window
    erase();

    // printw("%d", choice);
    refresh();

    return choice;
}

void showNewGameMenu(int choice[3])
{
    WINDOW *botchoice;

    int i, j, highlight = 0, position;

    // Options for bots that user can choose
    char bot_options[4][10] = {"Jörgen", "Hans", "Müller"};

    // Temporary storage for input
    char item[9];
    char ch;

    // Initialize the variables
    for (i = 0; i < 4; i++)
    {
        choice[i] = -1;
    }
    numberOfBots = 0;

    // Show the logo beforehand
    showLogo();

    // Create new window
    botchoice = newWindow(10, 57, getMiddleX(stdscr, 57), 12);
    box(botchoice, 0, 0);

    // Un-hide the cursor
    curs_set(1);

    // Make sure the user can see what they have inputted before entering it
    echo();

    wrefresh(botchoice);

    while (1)
    {
        mvwprintw(botchoice, 3, 4, "How many bots do you want to play with (1 - 3):");
        wmove(botchoice, 3, strlen("How many bots do you want to play with (1 - 3):") + 4);

        // Get the user input
        wscanw(botchoice, "%d", &numberOfBots);

        // Check for validity of input
        if (numberOfBots > 0 && numberOfBots < 4)
        {
            break;
        }

        wrefresh(botchoice);
    }

    // Cleared out the box for next stage of menu
    werase(botchoice);

    // Re-box the window as erase also remove the box from the window
    box(botchoice, 0, 0);

    // Enable usage of keypad
    keypad(botchoice, true);

    // Don't show the user input
    noecho();

    // Hide the cursor
    curs_set(0);

    // Loops the input stage as much as the user has inputted
    for (j = 0; j < numberOfBots; j++)
    {
        // Reset the highlight index each time
        highlight = 0;

        while (1)
        {
            // Show the current bot number
            mvwprintw(botchoice, 1, getMiddleX(botchoice, strlen("Bot no. 1")), "Bot no. %d", j + 1);

            // Shows the bot options
            for (i = 0; i < 3; i++)
            {
                // Get the horizontal center for each string
                position = getMiddleX(botchoice, strlen(bot_options[i]));

                // Highlight it for the selected
                if (i == highlight)
                    wattron(botchoice, A_REVERSE);

                mvwprintw(botchoice, i + 2, position, bot_options[i]);

                if (i == highlight)
                    wattroff(botchoice, A_REVERSE);
            }

            // Get the user actions
            ch = wgetch(botchoice);

            if (ch == (char)KEY_UP)
            {
                highlight--;
                highlight = (highlight < 0) ? 2 : highlight;
            }
            else if (ch == (char)KEY_DOWN)
            {
                highlight++;
                highlight = (highlight > 2) ? 0 : highlight;
            }
            else if (ch == 10)
            {
                choice[j] = highlight;
                break;
            }
        }
    }

    // Remove borders of the box
    wborder(botchoice, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    // Remove the window as it's not used anymore
    delwin(botchoice);
}

void initHumanPlayerData(int colour)
{
    int i;
    char col; // Colour of the the player

    switch (colour)
    {
    case 0:
        col = 'r';
        break;

    case 1:
        col = 'g';
        break;

    case 2:
        col = 'y';
        break;

    case 3:
        col = 'b';
        break;

    default:
        break;
    }

    players[colour].col = col;
    players[colour].comp = false;
    players[colour].move = 0;

    for (i = 0; i < 4; i++)
    {
        switch (col)
        {
        case 'r':
            red[i].col = 'r';
            red[i].ind = i;
            red[i].pos = 0;
            red[i].relpos = 0;
            red[i].safe = false;

            break;

        case 'g':
            green[i].col = 'g';
            green[i].ind = i;
            green[i].pos = 0;
            green[i].relpos = 0;
            green[i].safe = false;

            break;

        case 'y':
            yellow[i].col = 'y';
            yellow[i].ind = i;
            yellow[i].pos = 0;
            yellow[i].relpos = 0;
            yellow[i].safe = false;

            break;

        case 'b':
            blue[i].col = 'b';
            blue[i].ind = i;
            blue[i].pos = 0;
            blue[i].relpos = 0;
            blue[i].safe = false;

            break;

        default:
            break;
        }
    }
}

void initBotPlayerData(int botIndexes, int colour)
{
    int i;
    char col;
    char botIndex;

    switch (colour)
    {
    case 0:
        col = 'r';
        break;

    case 1:
        col = 'g';
        break;

    case 2:
        col = 'y';
        break;

    case 3:
        col = 'b';
        break;

    default:
        break;
    }

    switch (botIndexes)
    {
    case 0:
        botIndex = 'j';
        break;

    case 1:
        botIndex = 'h';
        break;

    case 2:
        botIndex = 'm';
        break;

    default:
        break;
    }

    players[colour].col = col;
    players[colour].comp = true;
    players[colour].comptype = botIndex;
    players[colour].move = 0;

    for (i = 0; i < 4; i++)
    {
        switch (col)
        {
        case 'r':
            red[i].col = 'r';
            red[i].ind = i;
            red[i].pos = 0;
            red[i].relpos = 0;
            red[i].safe = false;

            break;

        case 'g':
            green[i].col = 'g';
            green[i].ind = i;
            green[i].pos = 0;
            green[i].relpos = 0;
            green[i].safe = false;

            break;

        case 'y':
            yellow[i].col = 'y';
            yellow[i].ind = i;
            yellow[i].pos = 0;
            yellow[i].relpos = 0;
            yellow[i].safe = false;

            break;

        case 'b':
            blue[i].col = 'b';
            blue[i].ind = i;
            blue[i].pos = 0;
            blue[i].safe = false;

            break;

        default:
            break;
        }
    }
}

void initPlayerData(int botIndexes[3])
{
    int randTemp[4]; // Used for temporary storage of randomized number
    int i, j;        // Looping variable
    int temp;

    i = 0;
    srand(time(NULL));

    // Make sure non-player are known, intializing null players
    for (i = 0; i < 4; i++)
    {
        players[i].col = 'n';
    }

    // Get all uniqe random number
    while (i < 4)
    {
        temp = rand() % 4;

        for (j = 0; j < i; j++)
        {
            if (randTemp[j] == temp)
            {
                break;
            }
        }

        if (j == i)
        {
            randTemp[i++] = temp;
        }
    }

    initHumanPlayerData(randTemp[0]);

    for (i = 0, j = 1; i < numberOfBots && j < 4; i++, j++)
    {
        initBotPlayerData(botIndexes[i], randTemp[j]);
    }
}

Tokens isThereOpponents(Tokens token, int index)
{
    int i;

    // Loops every array of token colour
    for (i = 0; i < 4; i++)
    {
        // Oppponents is there only if they're not the same colour, have the same positions
        // and not in the safezone
        if (red[i].col != token.col && red[i].pos == index && !red[i].safe)
        {
            return red[i];
        }
        else if (green[i].col != token.col && green[i].pos == index && !green[i].safe)
        {
            return green[i];
        }
        else if (blue[i].col != token.col && blue[i].pos == index && !blue[i].safe)
        {
            return blue[i];
        }
        else if (yellow[i].col != token.col && yellow[i].pos == index && !yellow[i].safe)
        {
            return yellow[i];
        }
    }

    // If none were found return with colour n (null)
    token.col = 'n';
    return token;
}

bool isTransitionToSafezone(Tokens token, int diceroll)
{
    return (token.relpos + diceroll) > 52;
}

//int distanceBetween(Tokens token1, Tokens token2)
//{
//    int distance = 0; // Distance between token1 and 2
//    int token1pos = token1.pos; // Temporary variable for positions
//    int token2pos = token2.pos;
//
//    /* Add calculation here */
//}

void ClearScreen()
{
#ifdef _WIN32
    System("cls");
#else
    system("clear");
#endif
}

void WaitForSecond(int time)
{
// Check for platform as sleep function are different
// in different OS
#ifdef _WIN32
    // Windows uses ms instead seconds
    Sleep(time * 1000);
#else
    sleep(time);
#endif
}

bool isFileExist(char fileName[])
{
// Different syntax of access on widows
#ifdef _WIN32
    return (_access(fileName, F_OK) != -1);
#else
    return (access(fileName, F_OK) != -1);
#endif
}

int RollADice()
{
    srand(time(NULL));

    return rand() % 6 + 1;
}

int whosTurn(int count)
{
    return (count % numberOfBots + 1) + 1;
}

void moveToken(int diceNum, int whosTurn)
{
    int dadu, pos[4];
    char posmov[4];
    Tokens temp[4], opponents;

    switch (whosTurn)
    {
    case 1:
        for (int i = 0; i < 4; i++)
        {
            temp[i] = red[i];
        }
        break;
    case 2:
        for (int i = 0; i < 4; i++)
        {
            temp[i] = green[i];
        }
        break;
    case 3:
        for (int i = 0; i < 4; i++)
        {
            temp[i] = yellow[i];
        }
        break;
    case 4:
        for (int i = 0; i < 4; i++)
        {
            temp[i] = blue[i];
        }
        break;

    default:
        break;
    }

    printf("Tokens that can be move : ");
    int j = 0;
    for (int i = 0; i < 4; i++)
    {
        posmov[i] = possibleMove(dadu, temp[i].pos, temp[i].safe);
        if (posmov[i] == 's')
        {
            j++;
            continue;
        }
        else
            printf("%d ", i + 1);
        if (j == 4)
        {
            printf("There's no tokens that can be move");
            //end the turn
        }
    }
    printf("\n");

    //check if the player comp or not
    if (!players[whosTurn].comp)
    {
        int numOfToken;
        printf("Choose the token : ");
        scanf("%d", &numOfToken);
        while (!validateInputToken(numOfToken) || ((posmov[numOfToken - 1] == 's')))
        {
            printf("The input isn't valid, please reenter the tokens!!\n");
            printf("Choose the token : ");
            scanf("%d", &numOfToken);
        }
        numOfToken -= 1;
        opponents = isThereOpponents(temp[numOfToken], temp[numOfToken].pos + diceNum);
        if (posmov[numOfToken] == 'm')
        {
            //check if the token can move to safe zone or not
            if (isTransitionToSafezone(temp[numOfToken], diceNum))
            {
                moveToSafeZone(numOfToken, whosTurn, diceNum);
                printf("Token %d move %d step to safe zone \n", numOfToken, dadu);
            }
            else
            {
                //check if there is opponents with modul isThere opponents
                if ((opponents.col != temp[numOfToken].col) && (opponents.col != 'n'))
                {
                    //suitMenu()
                    //suitCheck()
                    //if win then token moveForward opponents moveToHome and vice versa
                }
                else
                {
                    moveForward(diceNum, whosTurn, numOfToken);
                    printf("Token %d move %d step to board no-%d\n", numOfToken, dadu, temp[numOfToken].pos + dadu);
                }
            }
        }
        else if (posmov[numOfToken] == 'o')
        {
            printf("Token %d enter the board", numOfToken);

            //check if there is opponents with modul isThere opponents
            if ((opponents.col != temp[numOfToken].col) && (opponents.col != 'n'))
            {
                //suitMenu()
                //suitCheck()
                //if win then token don't have to back to home
            }
            //end the turn
        }
    }
    else
    {
        //the possible move passed to bot that take the turn
    }
}

char possibleMove(int diceNum, int x, bool safe)
{
    //There's only three possibilities for token, m for move, o for out from base, s for stuck
    if ((diceNum == 6 && x == 0) && !safe)
    {
        return 'o'; //if the token is in the home and can move to board
    }
    else if ((diceNum != 6 && x == 0) && !safe)
    {
        return 's'; //if the token is in the home and can't move to board
    }
    else if ((x != 0) && !safe)
    {
        return 'm'; //if the token is in the board
    }
    else if ((diceNum > (6 - x)) && safe)
    {
        return 's'; //if the token is in the safe zone and the dice number is bigger than required step to finish
    }
    else
    {
        return 'm'; //if the token is in the safe zone and the dice number isn't bigger than required step to finish
    }
    return '0';
}

bool validateInputToken(int x)
{
    if (x > 0 && x < 5)
    {
        return true;
    }
    else
        return false;
}

void moveForward(int diceNum, int whosTurn, int numOfToken)
{
    switch (whosTurn)
    {
    case 1:
        red[numOfToken].pos += diceNum;
        red[numOfToken].relpos += diceNum;
        if (red[numOfToken].pos > 52)
        {
            red[numOfToken].pos -= 52;
        }
        break;
    case 2:
        green[numOfToken].pos += diceNum;
        green[numOfToken].relpos += diceNum;
        if (green[numOfToken].pos > 52)
        {
            green[numOfToken].pos -= 52;
        }
        break;
    case 3:
        yellow[numOfToken].pos += diceNum;
        yellow[numOfToken].relpos += diceNum;
        if (yellow[numOfToken].pos > 52)
        {
            yellow[numOfToken].pos -= 52;
        }
        break;
    case 4:
        blue[numOfToken].pos += diceNum;
        blue[numOfToken].relpos += diceNum;
        if (blue[numOfToken].pos > 52)
        {
            blue[numOfToken].pos -= 52;
        }
        break;
    default:
        break;
    }
}

void toHomeBase(int numOfToken, int whosTurn)
{
    switch (whosTurn)
    {
    case 1:
        red[numOfToken].pos = 0;
        red[numOfToken].relpos = 0;
        break;
    case 2:
        green[numOfToken].pos = 0;
        green[numOfToken].relpos = 0;
        break;
    case 3:
        yellow[numOfToken].pos = 0;
        yellow[numOfToken].relpos = 0;
        break;
    case 4:
        blue[numOfToken].pos = 0;
        blue[numOfToken].relpos = 0;
        break;
    default:
        break;
    }
}

void outFromHomeBase(int numOfToken, int whosTurn)
{
    switch (whosTurn)
    {
    case 1:
        red[numOfToken].pos = 1;
        red[numOfToken].relpos = 1;
        break;
    case 2:
        green[numOfToken].pos = 14;
        green[numOfToken].relpos = 1;
        break;
    case 3:
        yellow[numOfToken].pos = 28;
        yellow[numOfToken].relpos = 1;
        break;
    case 4:
        blue[numOfToken].pos = 41;
        blue[numOfToken].relpos = 1;
        break;
    default:
        break;
    }
}

void moveToSafeZone(int numOfToken, int whosTurn, int diceNum)
{
    switch (whosTurn)
    {
    case 1:
        red[numOfToken].pos = (red[numOfToken].pos + diceNum) - 52;
        red[numOfToken].relpos = red[numOfToken].pos;
        red[numOfToken].safe = true;
        break;
    case 2:
        green[numOfToken].pos = (green[numOfToken].pos + diceNum) - 12;
        green[numOfToken].relpos = green[numOfToken].pos;
        green[numOfToken].safe = true;
        break;
    case 3:
        yellow[numOfToken].pos = (yellow[numOfToken].pos + diceNum) - 25;
        yellow[numOfToken].relpos = yellow[numOfToken].pos;
        yellow[numOfToken].safe = true;
        break;
    case 4:
        blue[numOfToken].pos = (blue[numOfToken].pos + diceNum) - 39;
        blue[numOfToken].relpos = blue[numOfToken].pos;
        blue[numOfToken].safe = true;
        break;
    default:
        break;
    }
}

void showOptionBox()
{
    // Initialized the box under the board
    options = newWindow(7, 58, 1, 31);

    // Create the border of the box
    wborder(options, 0, 0, 0, 0, 0, 0, 0, 0);
    wrefresh(options);
}

void destroyOptionBox()
{
    // Clean up the border of the option box
    wborder(options, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    // Remove the content
    werase(options);

    // Refresh it
    wrefresh(options);

    // Remove it
    delwin(options);
}

void clearOptionBox()
{
    // Clear out the box
    werase(options);

    // Redraw the box as erasing it also remove the border
    wborder(options, 0, 0, 0, 0, 0, 0, 0, 0);

    // Refresh the box
    wrefresh(options);
}

int suitCheck(int player1, int player2)
{
    // If they choose the same option then it's a draw
    if (player1 == player2)
    {
        return 0;
    }
    // Kertas vs Gunting, gunting won
    else if (player1 == 1 && player2 == 2)
        return 2;
    // Kertas vs Batu, batu won
    else if (player1 == 1 && player2 == 3)
        return 1;
    // Gunting vs Kertas, gunting won
    else if (player1 == 2 && player2 == 1)
        return 1;
    // Gunting vs Batu, batu won
    else if (player1 == 2 && player2 == 3)
        return 2;
    // Batu vs Kertas, kertas won
    else if (player1 == 3 && player2 == 1)
        return 1;
    // Batu vs Gunting, batu won
    else if (player1 == 3 && player2 == 2)
        return 1;
}

void suitMenu(int *choice)
{
    // Make sure the option box is clear of content
    clearOptionBox();

    // Positioning and looping
    int i, highlight = 0, position;

    // Options for the user
    char option[3][10] = {"Kertas", "Gunting", "Batu"};
    char item[9];
    char ch;

    // Enable keypad mode for key up and down
    keypad(options, true);

    // Make sure what has been entered is not shown
    noecho();

    // Hide the cursor
    curs_set(0);

    // Show label to informed the user
    mvwprintw(options, 1, 1, "Pilih opsi untuk suit");
    position = strlen("Pilih opsi untuk suit");

    // Loop until has been inputed
    while (1)
    {
        // Show the options
        for (i = 0; i < 4; i++)
        {
            // Show the highlighted options with highlights
            if (i == highlight)
                wattron(options, A_REVERSE);

            mvwprintw(options, i + 1, position + 1, option[i]);

            if (i == highlight)
                wattroff(options, A_REVERSE);
        }

        // Get user input
        ch = wgetch(options);

        if (ch == (char)KEY_UP)
        {
            highlight--;
            highlight = (highlight < 0) ? 2 : highlight;
        }
        else if (ch == (char)KEY_DOWN)
        {
            highlight++;
            highlight = (highlight > 2) ? 0 : highlight;
        }
        else if (ch == 10) // Somehow the usage of KEY_ENTER doesn't work, so char 10 is used instead
        {
            *choice = highlight + 1;
            break;
        }
    }
}

int suitRandom()
{
    srand(time(NULL));

    return rand() % 3 + 1;
}

void printToOptionBox(char input[], int x, int y)
{
    // Print it
    mvwprintw(options, x, y, input);

    // Refresh the box
    wrefresh(options);
}

void positionToCoordinate(Tokens token, int *x, int *y)
{
    if (token.safe)
    {
        switch (token.col)
        {
        case 'r':
            // Red Safezone are in one collumn
            *y = 7;
            // The rows are the same as the position in the safezone
            *x = token.pos;
            break;

        case 'g':
            // Green Safezone are in one row
            *x = 7;
            // The safezone are in the right side of the board
            *y = 14 - token.pos;
            break;

        case 'y':
            // Yellow Safezone are in one collumn
            *y = 7;
            // The rows are int the bottom part of the board
            *x = 14 - token.pos;
            break;

        case 'b':
            // Blue Safezone are in one row
            *x = 7;
            // The collumn are the same as the position in the safezone
            *y = token.pos;
            break;
        }
    }
    else
    {
        // Search for the position in the rows
        switch (token.pos)
        {
        case 51:
        case 52:
        case 53:
            *x = 0;
            break;

        case 50:
        case 1:
            *x = 1;
            break;

        case 49:
        case 2:
            *x = 2;
            break;

        case 48:
        case 3:
            *x = 3;
            break;

        case 47:
        case 4:
            *x = 4;
            break;

        case 46:
        case 5:
            *x = 5;
            break;

        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            *x = 6;
            break;

        case 39:
        case 12:
            *x = 7;
            break;

        case 38:
        case 37:
        case 36:
        case 35:
        case 34:
        case 33:
        case 18:
        case 17:
        case 16:
        case 15:
        case 14:
        case 13:
            *x = 8;
            break;

        case 32:
        case 19:
            *x = 9;
            break;

        case 31:
        case 20:
            *x = 10;
            break;

        case 30:
        case 21:
            *x = 11;
            break;

        case 29:
        case 22:
            *x = 12;
            break;

        case 28:
        case 23:
            *x = 13;
            break;

        case 27:
        case 25:
        case 24:
            *x = 14;
            break;
        }

        // Search for the collumns
        switch (token.pos)
        {
        case 40:
        case 39:
        case 38:
            *y = 0;
            break;

        case 41:
        case 37:
            *y = 1;
            break;

        case 42:
        case 36:
            *y = 2;
            break;

        case 43:
        case 35:
            *y = 3;
            break;

        case 44:
        case 34:
            *y = 4;
            break;

        case 45:
        case 33:
            *y = 5;
            break;

        case 51:
        case 50:
        case 49:
        case 48:
        case 47:
        case 46:
        case 32:
        case 31:
        case 30:
        case 29:
        case 28:
        case 27:
            *y = 6;
            break;

        case 52:
        case 25:
            *y = 7;
            break;

        case 53:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
            *y = 8;
            break;

        case 6:
        case 18:
            *y = 9;
            break;

        case 7:
        case 17:
            *y = 10;
            break;

        case 8:
        case 16:
            *y = 11;
            break;

        case 9:
        case 15:
            *y = 12;
            break;

        case 10:
        case 14:
            *y = 13;
            break;

        case 11:
        case 12:
        case 13:
            *y = 14;
            break;
        }
    }
}

void printTokens()
{
    int tempHome[4];    // Counting how many tokens are in one homebase
                        // The content are sorted by colour with the 
                        // order of red, green, yellow, and blue
    int i, j; // Looping
    int x, y; // Temporary storage for the coordinate in the board
    char tempShown; // Temporary place for the the shown string

    // Initialize the array
    for ( i = 0; i < 53; i++)
    {
        if (i < 4)
        {
            tempHome[i] = 0;
        }
    }
    
    // Red tokens
    for ( i = 0; i < 4; i++)
    {
        // Tokens that are still in homebase
        if (red[i].pos == 0)
        {
            switch (tempHome[0])
            {
            case 0:
                mvwprintw(board[2][11], 0, 0, tokenShown(i));
                wrefresh(board[2][11]);
                break;
            
            case 1:
                mvwprintw(board[2][12], 0, 0, tokenShown(i));
                wrefresh(board[2][12]);
                break;

            case 2:
                mvwprintw(board[3][11], 0, 0, tokenShown(i));
                wrefresh(board[3][11]);
                break;

            case 3:
                mvwprintw(board[3][12], 0, 0, tokenShown(i));
                wrefresh(board[3][12]);
                break;

            default:
                break;
            }

            tempHome[0]++;
        }
        else
        {
            // Get coordinates
            positionToCoordinate(red[i], &x, &y);
            // Show the token
            wprintw(board[x][y], tokenShown(i));
            // Show the colour of the token
            wbkgd(board[x][y], COLOR_PAIR(BOARD_RED));
            // Refersh the board
            wrefresh(board[x][y]);
        }
        
    }
    
    // Green tokens
    for ( i = 0; i < 4; i++)
    {
        // Tokens that are still in homebase
        if (green[i].pos == 0)
        {
            switch (tempHome[1])
            {
            case 0:
                mvwprintw(board[11][11], 0, 0, tokenShown(i));
                wrefresh(board[11][11]);
                break;
            
            case 1:
                mvwprintw(board[11][12], 0, 0, tokenShown(i));
                wrefresh(board[11][12]);
                break;

            case 2:
                mvwprintw(board[12][11], 0, 0, tokenShown(i));
                wrefresh(board[12][11]);
                break;

            case 3:
                mvwprintw(board[12][12], 0, 0, tokenShown(i));
                wrefresh(board[12][12]);
                break;

            default:
                break;
            }

            tempHome[1]++;
        }
        else
        {
            // Get coordinates
            positionToCoordinate(green[i], &x, &y);
            // Show the token
            wprintw(board[x][y], tokenShown(i));
            // Show the colour of the token
            wbkgd(board[x][y], COLOR_PAIR(BOARD_GREEN));
            // Refersh the board
            wrefresh(board[x][y]);
        }
        
    }

    // Yellow tokens
    for ( i = 0; i < 4; i++)
    {
        // Tokens that are still in homebase
        if (yellow[i].pos == 0)
        {
            switch (tempHome[2])
            {
            case 0:
                mvwprintw(board[11][2], 0, 0, tokenShown(i));
                wrefresh(board[11][2]);
                break;
            
            case 1:
                mvwprintw(board[11][3], 0, 0, tokenShown(i));
                wrefresh(board[11][3]);
                break;

            case 2:
                mvwprintw(board[12][2], 0, 0, tokenShown(i));
                wrefresh(board[12][2]);
                break;

            case 3:
                mvwprintw(board[12][3], 0, 0, tokenShown(i));
                wrefresh(board[12][3]);
                break;

            default:
                break;
            }

            tempHome[2]++;
        }
        else
        {
            // Get coordinates
            positionToCoordinate(yellow[i], &x, &y);
            // Show the token
            wprintw(board[x][y], tokenShown(i));
            // Show the colour of the token
            wbkgd(board[x][y], COLOR_PAIR(BOARD_YELLOW));
            // Refersh the board
            wrefresh(board[x][y]);
        }
    }

    // Blue tokens
    for ( i = 0; i < 4; i++)
    {
        // Tokens that are still in homebase
        if (blue[i].pos == 0)
        {
            switch (tempHome[3])
            {
            case 0:
                mvwprintw(board[2][2], 0, 0, tokenShown(i));
                wrefresh(board[2][2]);
                break;
            
            case 1:
                mvwprintw(board[2][3], 0, 0, tokenShown(i));
                wrefresh(board[2][3]);
                break;

            case 2:
                mvwprintw(board[3][2], 0, 0, tokenShown(i));
                wrefresh(board[3][2]);
                break;

            case 3:
                mvwprintw(board[3][3], 0, 0, tokenShown(i));
                wrefresh(board[3][3]);
                break;

            default:
                break;
            }

            tempHome[3]++;
        }
        else
        {
            // Get coordinates
            positionToCoordinate(blue[i], &x, &y);
            // Show the token
            wprintw(board[x][y], tokenShown(i));
            // Show the colour of the token
            wbkgd(board[x][y], COLOR_PAIR(BOARD_BLUE));
            // Refersh the board
            wrefresh(board[x][y]);
        }
    }
}

char tokenShown(int index)
{
    switch (index)
    {
    case 0:
        return 'A';
        break;
    
    case 1:
        return 'B';
        break;

    case 2:
        return 'C';
        break;

    case 3:
        return 'D';
        break;

    default:
        return 'n';
        break;
    }
}