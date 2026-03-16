# Codoku 🧮

A terminal based Sudoku game for Windows built in C.  
**BEI I / I Project — ENCT 101**

![Gameplay](images/gameplay.gif)

---

## Team

- Anuj Sapkota  
- Bipul Maharjan  
- Nitish Adhikari  
- Prayush Lamsal  

---

## Features

- Three difficulty levels — Easy, Medium, Hard
- Color coded board using Windows console colors
- Mistake tracking (max 3 mistakes before game over)
- Solution reveal on game over
- Play again without restarting the program

---

![Home Screen](images/home-screen.png)
![Game Screen](images/game-screen.png)
![Game Completed](images/game-completed.png)


---

## Requirements

- Windows OS
- GCC or any C compiler (e.g. MinGW)
- Puzzle files: `easy.txt`, `med.txt`, `hard.txt` in the `data/` folder
- Solution file: `sol.txt` in the `data/` folder containing pre-solved solutions

---

## How to Compile

```bash
gcc codoku.c -o codoku.exe
```

## How to Run

```bash
./codoku.exe
```

---

## Controls

| Key | Action |
|-----|--------|
| Arrow keys / WASD | Move cursor |
| 1 – 9 | Place a number |
| 0 / Backspace | Clear a cell |
| ESC | Quit |

---


## Puzzle File Format

Each line in the `.txt` files is one puzzle:  81 digits in a row, left to right, top to bottom.  
`0` means an empty cell.

**Example puzzle string:**
```
003020600900305001001806400008500007000000000700009200005607000600108007007040100
```

**Parsed into the sudoku board:**
```
. . 3 | . 2 . | 6 . .
9 . . | 3 . 5 | . . 1
. . 1 | 8 . 6 | 4 . .
------+-------+------
. . 8 | 5 . . | . . 7
. . . | . . . | . . .
7 . . | . . 9 | 2 . .
------+-------+------
. . 5 | 6 . 7 | . . .
6 . . | 1 . 8 | . . 7
. . 7 | . 4 . | 1 . .
```

(`.` represents empty cells, which are `0` in the file)


---

## Limitations

- **Pre-solved Solutions**: The game uses solved solutions stored in `sol.txt` rather than solving puzzles algorithmically. This was implemented to avoid performance issues with complex puzzles (especially hard difficulty levels).

---

## Future Improvement

- The backtracking algorithm can be integrated in future versions for dynamic solution generation. This would eliminate the need for `sol.txt`.

---

## Project Structure

```
codoku.c           — main source file
data/
  easy.txt         — easy puzzles
  med.txt          — medium puzzles
  hard.txt         — hard puzzles
  sol.txt          — solutions (organized by [EASY], [MED], [HARD] sections)
README.md          — this file
```

---

## Header files used:

### 1. `stdlib.h` (Standard Library)

*   **`rand()` & `srand()`**: 
    *   Used to pick a random puzzle from the data files. 
    *   `srand()` initializes the random number generator, and `rand() % totalPuzzles` ensures we get a valid index within the range of available puzzles.
*   **`system("cls")`**: 
    *   Used in the `clearScreen()` function. It invokes the operating system's command processor to clear the terminal, allowing the board to be "redrawn" in the same spot for a smooth UI.
*   **`exit(1)`**: 
    *   Used for emergency stops. If a critical file (like `easy.txt`) is missing, the program terminates safely rather than crashing.

### 2. `time.h` (Time Library)
*   **`time(NULL)`**: 
    *   Used as a "seed" for `srand`. It returns the current system time in seconds. Because time is always moving forward, the seed is always different, ensuring the "random" puzzle selected is different every time you play.

### 3. `windows.h` (Windows API)

*   **`SetConsoleTextAttribute`**: 
    *   This function changes the foreground (text) and background colors of the console.
    *   **Usage in Program:** Our `setColor()` function uses this to distinguish between **Fixed numbers** (White), **User numbers** (Green), **Errors** (Red), and the **Cursor** (Yellow).
*   **`GetStdHandle(STD_OUTPUT_HANDLE)`**: 
    *   This retrieves a "handle" (a unique ID) for the standard output (the screen). The computer needs this ID to know which window to apply colors to.

### 4. Advanced Data & Logic Techniques

#### A. Custom Structures (`struct`)

```c
typedef struct {
    int difficulty;
    int mistakes;
} Player;
```
*   **Benefit:** This groups related data into one "object," making it easier to pass the player's current state (like mistake count) between functions like `drawBoard` and `main`.


#### B. Secure String Formatting (`snprintf`)
Unlike `printf` which prints to the screen, `snprintf` prints into a character buffer (a string).
*   **Usage:** `snprintf(msg, sizeof(msg), "Error message...")`
*   **Why it's used:** It prevents **Buffer Overflow**. It ensures that even if the message is very long, it will never exceed the size of the `msg` array (128 bytes), making the program more stable.

#### C. Extended Key Handling (Scan Codes)
The game handles arrow keys which are not part of the standard ASCII table.
*   **Prefix `224`**: Most special keys in Windows return a prefix of 224 first. The program catches this prefix and then calls `_getch()` a second time to get the specific direction (72 for Up, 80 for Down, etc.).

### 5. Macro Definitions (`#define`)
We used macros for constants like `MAX_MISTAKES`, `SIZE`, and various color codes (e.g., `CLR_TITLE`).
*   **Benefit:** This makes the code highly **maintainable**.