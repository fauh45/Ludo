#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    int pos; //Position of the tokens
    bool safe; //Is the token in safezone
} Tokens;

typedef struct
{
    char col; //Color of the player
    bool comp; //Is the player a computer or not
    int move; //How much the player has move
} Player;

WINDOW *board[15][15]; // Ludo board (graphically)

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
   Input : int line -> how much rows the new window have
           int collumns -> how much collumns the new window have
           int starty -> the starting row position of the new window
           int startx -> the starting collumns position of the new window
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
    Input : WINDOW *win -> the target window to get the middle horizontal position
            int width -> the width of object
    Output : leftmost position of the object such that the object is in the middle of said screen
    Author : Muhammad Fauzan L.
*/
int getMiddleX(WINDOW *win, int len);

int main()
{
    // Curses mode intialization
    initscr();
    
    // Starts out colour mode of curses
    start_color();

    // Initalize all the color pairs for the board
    init_pair(BOARD_BLUE, COLOR_WHITE, COLOR_BLUE);
    init_pair(BOARD_RED, COLOR_WHITE, COLOR_RED);
    init_pair(BOARD_GREEN, COLOR_WHITE, COLOR_GREEN);
    init_pair(BOARD_YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(BOARD_WHITE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BOARD_BLACK, COLOR_WHITE, COLOR_BLACK);

    // Check for console capabilities
    if (!has_colors())
    {
        printw("No color support on console");
        exit(1);
    }
    else if (!isBigEnough())
    {
        printw("Console not big enough for the game");
        exit(2);
    }

    showBoard();

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

        printw("Logo file not found");
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

int RollADice()
{
    srand(time(NULL));

    return rand() % 6 + 1;
}