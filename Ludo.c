#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

/* OS Detection to make sure screen clearing and sleep function works */
#if defined(__linux__) || defined(unix)
    #include <unistd.h>
    char* os = "linux";
#else
#ifdef _WIN32
    #include <windows.h>
    char* os = "windows";
#else
#ifdef __APPLE__
    #include <unistd.h>
    char* os = "macos";
#else
    #include <unistd.h>
    char* os = "unknown";
#endif
#endif
#endif

/* Variable Declaration */
typedef struct
{
    char col; //Color of the tokens
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

/* Function Declaration */
/* TODO : Add more function */
void ClearScreen(); //Clearing the console window
void WaitForSecond(int time); //wait for time seconds
int RollADice(); //randomize roll of a dice
// bool Countdown(int Time); //shows time left of turn

/* Global Variable Declaration */
bool DEBUG = false;

int main()
{
    
}

/* 
    Initial state : Screen not clear
    Final state : screen is clear
    Author : Muhammad Fauzan L. 
*/
void ClearScreen()
{
    if (os == "windows")
    {
        system("cls");
    }
    else if (os == "linux" || os == "macos")
    {
        system("clear");
    }
}

/*
   Initial State : Not waiting
   Final State : Waits for time seconds
   Author : Muhammad Fauzan L.
*/

void WaitForSecond(int time)
{
    if(os == "windows")
    {
        time = time * 1000; //windows uses ms instead of seconds
    }

    Sleep(time);
}

/*
   Input : None
   Output : Random dice rool (int)
   Author : Muhammad Fauzan L.
*/
int RollADice()
{
    srand(time(NULL));

    return rand() % 6 + 1;
}

// bool Countdown(int Time)
// {
//     printf("\nTime Left : %d", Time);

//     while(Time != 0)
//     {
//         WaitForSecond(1);
//         Time--;
//         printf("\b%d", Time);
//     }

//     return true;
// }