
# Ludo
Ludo is a game that originated from derived from the Indian game Pachisi. The game is written in C as part of DDP TUBES. The game rule are modified version of Ludo games.

## Game Design
### Player
User can chose from 1 to 3 computer player.  There's only one human player in the game.

### Ludo Board
Ludo board consist of 4 corners, with different colour. Each player is assigned to one corner of the board and given 4 tokens. The board itself looks like this

![Ludo Board](https://upload.wikimedia.org/wikipedia/commons/thumb/8/82/Ludo_paths.svg/260px-Ludo_paths.svg.png)

### Game Rules
At the start of the game each of the players token is positioned in their respective corners, the system chooses the first player. The second, third, and fourth are automatically chosen based on their position on the board itself.

Each player rolls a dice, and when the dice show 6 the player can move out one of their token. After the dice shows 6, the player can rolls a dice again, and if the dice shows 6 again, the player have a choice of moving the token or move out another token. If the second roll of dice shows 6, the third roll of dice whatever the number is, the player can only move one of their token. If all of their token are out of their respective corners, the player **cannot** take other players token.

The movement of the tokens follows the pattern showed on the picture of Ludo board.

### Game Over
The game is over after few terms :

 1. The human player has won in first place
 2. If three of the computer player has won
