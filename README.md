# **Project: Network Awalé Game**

## **Introduction to Awalé**
Awalé is a traditional African board game belonging to the family of sowing games. It is played on a board with two rows of six holes called "houses," with a certain number of seeds distributed among these houses.

### **Objective of the Game:**
The goal is to collect more seeds than your opponent. Players, taking turns, choose a house on their side of the board and distribute its seeds into the following houses, following a circular movement. When a player "sows" the seeds, they can capture the opponent’s seeds under certain conditions. Awalé is both a strategy and thinking game.

For more information, you can consult the detailed article about Awalé:  
[https://fr.wikipedia.org/wiki/Awal%C3%A9](https://fr.wikipedia.org/wiki/Awal%C3%A9)

This project implements Awalé in a client-server environment, allowing two players to compete online or other users to join games as spectators.

---

## **Project Structure**

The project is organized into the following folders:
- **Client:** Contains the source code for the client.
- **Server:** Contains the source code for the server.

---

## **Client Features**

### **Available Commands**
1. **`commands`**  
   Displays the list of available client commands.

2. **`getPlayersList`**  
   Allows you to see the list of players currently connected to the server.  
   - **Command:** `1`

3. **`request Starting Game with Player`**  
   Allows requesting another player to start a game.  
   - **Command:** `2 [PlayerName]`  
   - Example: `2 Ahmed`

4. **`get The list of active games`**  
   Displays a list of active games and the players involved.  
   - **Command:** `ag`

5. **`reject/accept game`**  
   Allows the player to reject or accept a game request. If accepted, the game starts.  
   - **Command:** When a request is received, type `y` to accept or `n` to reject.

6. **`join game as an observer`**  
   Allows the user to join an active game as a spectator.  
   - **Command:** `o [game index]`  
   - Example: `o 1`

7. **`play turn Awalé`**  
   Allows the player to select a "house" (slot) to play their turn.  
   - **Command:** `p [house number (1-6)]`  
   - Example: `p 3`  
   - **Note:** Houses with 0 seeds cannot be selected.

8. **Quit the Game**  
   Allows a spectator to stop watching an active game.  
   - **Command:** `q`

9. **Surrender the Game**  
   Allows a player to surrender an active game.  
   - **Command:** `sr`

10. **Player or Observer Reconnection**  
    If a player or observer gets disconnected, they can reconnect and resume the game, provided they use the same username.  
    - **Command:**  
      ```bash
      client [ipAddress] [PlayerName]
      ```

11. **View the History of a Game**  
    Allows a player to view the history of a finished game.  
    - **Command:** `gh [game index]`

12. **Send a Message in a Game Chatroom**  
    Allows a player to send a message visible to the opponent and spectators.  
    - **Command:** `mr [message text]`

---

## **Server Features**

1. **Connection Management:**  
   Manages multiple simultaneous connections for players and spectators.

2. **Active Games Management:**  
   Manages and updates active games in real time.

3. **Move Validation:**  
   Validates that player moves comply with Awalé rules.

4. **Support for Spectators:**  
   Allows users to observe games in real time.

5. **Error and Status Messages:**  
   Handles errors and provides clear messages to users.

6. **Name Restrictions**  
   - Each client must have a unique name. Duplicate names will be rejected.

---

## **Compilation Instructions**

1. **Extract the Files:**  
   Extract the `.zip` archive.

2. **Compile the Server:**  
   ```bash
   gcc playerState.c game.c server2.c -o server
   ```

3. **Compile the Client:**  
   ```bash
   gcc client2.c -o client
   ```

---

## **Execution Instructions**

1. **Start the Server:**  
   ```bash
   ./server
   ```

2. **Start the Client:**  
   ```bash
   ./client [ipAddress] [PlayerName]
   ```
   - **ipAddress:** Server's IP address.  
   - **PlayerName:** Name of the player or observer.

---

## **Additional Notes**

- Players and spectators must reconnect with the same **PlayerName** to resume a session.
- Game updates and boards are reflected in real-time for players. Spectators see updates after player moves.
- Always start the server before starting any clients.

---
