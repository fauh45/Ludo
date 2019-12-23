#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
// #include <stdbool.h>

/* OS Detection to make sure screen clearing and sleep function works */
#if defined(__linux__) || defined(unix)
    #include <unistd.h>

    // Ncurses for Linux user
    #include <ncurses.h>
    char* os = "linux";
#else
#ifdef _WIN32
    #include <windows.h>

    // PDCurses (port of ncurses) for windows user
    #include "PDCurses/curses.h"
    char* os = "windows";
#else
#ifdef __APPLE__
    #include <unistd.h>

    // Ncurses for the macs as well
    #include <ncurses.h>
    char* os = "macos";
#else
    #include <unistd.h>
    #include <ncurses.h>
    char* os = "unknown";
#endif
#endif
#endif

// Colour for boards, defined for easier reading
#define BOARD_BLUE 1    // White foreground, Blue background
#define BOARD_RED 2     // White foreground, Red background
#define BOARD_GREEN 3   // White foreground, Green background
#define BOARD_YELLOW 4  // White foreground, Yellow background
#define BOARD_WHITE 5   // Black foreground, White background
#define BOARD_BLACK 6   // White foreground, Black background

/* Variable Declaration */
typedef struct
{
    char col; //Color of the tokens, corresponding to the player colour
    int ind; //Index of the tokens (1 - 4) for each player
    int pos; //Position of the tokens (global position)
    int relpos; //Realtive Position from the start position
    bool safe; //Is the token in safezone
} Tokens;

typedef struct
{
    char col; //Color of the player
    bool comp; //Is the player a computer or not
    char comptype; // Type of computers
    int move; //How much the player has move
} Player;

WINDOW *board[15][15]; // Ludo board (graphically)

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
    @index index for player data array, from 1-3, as human player always in the first index
    Final State : Bot player data are initialized
    Author : Muhammad Fauzan L.
*/
void initBotPlayerData(int botIndexes, int colour, int index);

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
    @token the current token
    @index the place where the current tokens wants to move
    @diceroll number from 1 to 6 of the dice roll
    Final State : Token in the desired position or back to home base if failed the suit
    Author : Muhammad Fauzan L.
*/
void moveToken(Tokens token, int index, int diceroll);

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

    printw("%d", RollADice());

    getch();

    /* 
        Add more code here!
    */

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
    int i;  // Rows of Board
    int j;  // Collumn of Board
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
    int i;  // Rows of Board
    int j;  // Collumns of Board

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
        exit(3);
    }

    // New widow to put the logo
    // Horizontally center
    logo = newWindow(10, 41, getMiddleX(stdscr, 41), 1);

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
            if (i == highlight) wattron(mainmenu, A_REVERSE);

            mvwprintw(mainmenu, i+1, position, options[i]);

            if (i == highlight) wattroff(mainmenu, A_REVERSE);
        }

        // Get user input
        ch = wgetch(mainmenu);
        
        if (ch == (char) KEY_UP)
        {
            highlight--;
            highlight = (highlight < 0) ? 3 : highlight;
        }
        else if (ch == (char) KEY_DOWN)
        {
            highlight++;
            highlight = (highlight > 3) ? 0 : highlight;
        }
        else if (ch == 10)  // Somehow the usage of KEY_ENTER doesn't work, so char 10 is used instead
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
    char col;
    
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

    players[0].col = col;
    players[0].comp = false;
    players[0].move = 0;

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

void initBotPlayerData(int botIndexes, int colour, int index)
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

    players[index].col = col;
    players[index].comp = true;
    players[index].comptype = botIndex;
    players[index].move = 0;

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
    int i, j; // Looping variable
    int temp;

    i = 0;
    srand(time(NULL));

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
            randTemp[i++] == temp;
        }
    }

    initHumanPlayerData(randTemp[0]);

    for (i = 0; i < numberOfBots; i++)
    {
        initBotPlayerData(botIndexes[i], randTemp[j], i+1);
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

void moveToken(Tokens token, int index, int diceroll)
{
    Tokens temp; // Temporarly put opponents check

    if (isTransitionToSafezone(token, diceroll))
    {
        token.safe = true;
        token.pos = (token.pos + diceroll) - 52;
        token.relpos = (token.pos + diceroll) - 52;
    }
    else
    {
        temp = isThereOpponents(token, index);

        if (temp.col == 'n')
        {
            token.pos = index;
            token.relpos += diceroll;
        }
        else
        {
            /*
                Add Suit menu here
            */
        }
        
    }
}

int distanceBetween(Tokens token1, Tokens token2)
{
    int distance = 0; // Distance between token1 and 2
    int token1pos = token1.pos; // Temporary variable for positions
    int token2pos = token2.pos;

    /* Add calculation here */
}

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