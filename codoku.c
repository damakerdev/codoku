#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define SIZE 9
#define MAX_PUZZLES 50
#define MAX_LINE 100
#define MAX_MISTAKES 3

/* Arrow key codes returned by _getch() on Windows */
#define ARROW_PREFIX 224
#define ARROW_UP     72
#define ARROW_DOWN   80
#define ARROW_LEFT   75
#define ARROW_RIGHT  77
#define KEY_ESC      27
#define KEY_BACKSPACE 8

/* Color constants (Windows console attributes) */
#define CLR_RESET      7    /* default white on black */
#define CLR_TITLE      11   /* bright cyan */
#define CLR_SUBTITLE   14   /* yellow */
#define CLR_CREDITS    8    /* dark gray */
#define CLR_MENU       15   /* bright white */
#define CLR_MENU_HL    10   /* bright green */
#define CLR_BORDER     8    /* dark gray */
#define CLR_FIXED      15   /* bright white – original puzzle numbers */
#define CLR_PLACED     10   /* bright green – player-placed numbers */
#define CLR_CURSOR     14   /* yellow – highlighted cursor cell */
#define CLR_EMPTY      8    /* dark gray – dots for empty cells */
#define CLR_INFO       11   /* bright cyan – header info */
#define CLR_MSG_OK     10   /* bright green – success messages */
#define CLR_MSG_ERR    12   /* bright red – error/wrong messages */
#define CLR_PROMPT     14   /* yellow – prompts */
#define CLR_ROWNUM     9    /* bright blue – row numbers */
#define CLR_COLNUM     9    /* bright blue – column numbers */

/* Struct to track player info */
typedef struct {
    int difficulty;    /* 1=Easy, 2=Medium, 3=Hard */
    int mistakes;
} Player;

/* ------------------------------------------------------------------ */
/*  Function prototypes                                               */
/* ------------------------------------------------------------------ */
void clearScreen(void);
void setColor(int color);
void showHomeScreen(Player *player);
void loadPuzzle(int (*board)[SIZE], int (*solution)[SIZE], int difficulty);
void drawBoard(int (*board)[SIZE], int (*fixed)[SIZE], int curRow, int curCol, const char *msg, Player *player);
int  isSafe(int (*board)[SIZE], int row, int col, int num);
int  isBoardFull(int (*board)[SIZE]);
int  solveSudoku(int (*board)[SIZE]);

/* ------------------------------------------------------------------ */
/*  Set console text color (Windows)                                  */
/* ------------------------------------------------------------------ */
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

/* ------------------------------------------------------------------ */
/*  Clear screen                                                      */
/* ------------------------------------------------------------------ */
void clearScreen(void)
{
    system("cls");
}

/* ------------------------------------------------------------------ */
/*  Show the home screen with ASCII art and difficulty selection      */
/* ------------------------------------------------------------------ */
void showHomeScreen(Player *player)
{
    int choice;

       clearScreen();
    printf("\n");
    setColor(CLR_TITLE);

    
    printf("                    _       _           \n");
    printf("       ___ ___   __| | ___ | | ___   _  \n");
    printf("      / __/ _ \\ / _` |/ _ \\| |/ / | | | \n");
    printf("     | (_| (_) | (_| | (_) |   <| |_| | \n");
    printf("      \\___\\___/ \\__,_|\\___/|_|\\_\\__,__| \n");                           
    printf("\n");

    setColor(CLR_SUBTITLE);
    printf("    +-  BEI I/I Project - ENCT 101  -+\n\n");

    setColor(CLR_CREDITS);
    printf("    Developed by: Anuj Sapkota, Bipul Maharjan, Nitish Adhikari, Prayush Lamsal\n\n");
    printf("    Controls: Arrow keys to move | 1-9 to place numbers\n\n");

    setColor(CLR_BORDER);
    printf("    ========================================================\n\n");

    printf("\n");
    setColor(CLR_PROMPT);
    printf("      Select Difficulty:\n\n");
    setColor(CLR_MENU_HL);
    printf("        [1]");
    setColor(CLR_MENU);
    printf("  Easy    - Relaxed, more clues given\n");
    setColor(CLR_MENU_HL);
    printf("        [2]");
    setColor(CLR_MENU);
    printf("  Medium  - Balanced challenge\n");
    setColor(CLR_MENU_HL);
    printf("        [3]");
    setColor(CLR_MENU);
    printf("  Hard    - Minimal clues, maximum brain\n\n");

    setColor(CLR_PROMPT);
    printf("      Choice: ");
    setColor(CLR_MENU);
    if (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        setColor(CLR_MSG_ERR);
        printf("      Invalid choice. Defaulting to Easy.\n");
        choice = 1;
    }
    /* Flush leftover newline from scanf */
    while (getchar() != '\n');

    player->difficulty = choice;
    player->mistakes = 0;

    setColor(CLR_RESET);
}

/* ------------------------------------------------------------------ */
/*  Draw the board with colors and a highlighted cursor cell           */
/* ------------------------------------------------------------------ */
void drawBoard(int (*board)[SIZE], int (*fixed)[SIZE], int curRow, int curCol, const char *msg, Player *player)
{
    int i, j;
    const char *diffStr;
    int diffColor;

    clearScreen();

    switch (player->difficulty) {
        case 1:  diffStr = "Easy";   diffColor = CLR_MENU_HL; break;
        case 2:  diffStr = "Medium"; diffColor = CLR_SUBTITLE; break;
        default: diffStr = "Hard";   diffColor = CLR_MSG_ERR;  break;
    }

    /* Header bar */
    printf("\n");
    setColor(CLR_TITLE);
    printf("   CODOKU");
    setColor(CLR_BORDER);
    printf("  |  ");
    setColor(diffColor);
    printf("%s", diffStr);
    setColor(CLR_BORDER);
    printf("  |  ");
    if (player->mistakes > 0) {
        setColor(CLR_MSG_ERR);
    } else {
        setColor(CLR_MENU);
    }
    printf("Mistakes: %d/%d", player->mistakes, MAX_MISTAKES);
    setColor(CLR_RESET);
    printf("\n");

    setColor(CLR_BORDER);
    printf("   --------------------------------------------------------\n\n");

    /* Column numbers */
    setColor(CLR_COLNUM);
    printf("        1    2    3     4    5    6     7    8    9\n");

    i = 0;
    while (i < SIZE) {
        if (i % 3 == 0) {
            setColor(CLR_BORDER);
            printf("     +---------------+---------------+---------------+\n");
        }
        setColor(CLR_ROWNUM);
        printf("  %d  ", i + 1);
        setColor(CLR_BORDER);
        printf("|");

        j = 0;
        while (j < SIZE) {
            if (j > 0 && j % 3 == 0) {
                setColor(CLR_BORDER);
                printf("|");
            }

            if (i == curRow && j == curCol) {
                /* Cursor cell – highlighted */
                setColor(CLR_CURSOR);
                if (board[i][j] == 0) {
                    printf(" [.] ");
                } else {
                    printf(" [%d] ", board[i][j]);
                }
            } else if (board[i][j] == 0) {
                setColor(CLR_EMPTY);
                printf("  .  ");
            } else if (fixed[i][j]) {
                setColor(CLR_FIXED);
                printf("  %d  ", board[i][j]);
            } else {
                setColor(CLR_PLACED);
                printf("  %d  ", board[i][j]);
            }
            j++;
        }
        setColor(CLR_BORDER);
        printf("|\n");
        i++;
    }
    setColor(CLR_BORDER);
    printf("     +---------------+---------------+---------------+\n\n");

    /* Controls bar */
    setColor(CLR_CREDITS);
    printf("   ");
    setColor(CLR_COLNUM);
    printf("Arrows/WASD");
    setColor(CLR_CREDITS);
    printf(": move   ");
    setColor(CLR_COLNUM);
    printf("1-9");
    setColor(CLR_CREDITS);
    printf(": place   ");
    setColor(CLR_COLNUM);
    printf("0/Bksp");
    setColor(CLR_CREDITS);
    printf(": clear   ");
    setColor(CLR_COLNUM);
    printf("ESC");
    setColor(CLR_CREDITS);
    printf(": quit\n");

    /* Status message */
    if (msg[0] != '\0') {
        /* Color the message based on content */
        if (msg[0] == 'C') {
            /* "Correct!" or "Cleared" */
            setColor(CLR_MSG_OK);
        } else if (msg[0] == 'W') {
            /* "Wrong!" */
            setColor(CLR_MSG_ERR);
        } else {
            setColor(CLR_SUBTITLE);
        }
        printf("   >> %s\n", msg);
    }

    setColor(CLR_RESET);
    printf("\n");
}

/* ------------------------------------------------------------------ */
/*  Validate whether 'num' can be placed at board[row][col]          */
/*  Checks row, column, and 3x3 sub-grid                            */
/* ------------------------------------------------------------------ */
int isSafe(int (*board)[SIZE], int row, int col, int num)
{
    int i, startRow, startCol;

    /* Check the row */
    i = 0;
    while (i < SIZE) {
        if (board[row][i] == num) {
            return 0;
        }
        i++;
    }

    /* Check the column */
    i = 0;
    while (i < SIZE) {
        if (board[i][col] == num) {
            return 0;
        }
        i++;
    }

    /* Check the 3x3 sub-grid */
    startRow = (row / 3) * 3;
    startCol = (col / 3) * 3;
    {
        int r, c;
        r = startRow;
        while (r < startRow + 3) {
            c = startCol;
            while (c < startCol + 3) {
                if (board[r][c] == num) {
                    return 0;
                }
                c++;
            }
            r++;
        }
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/*  Check whether every cell on the board is filled (non-zero)       */
/* ------------------------------------------------------------------ */
int isBoardFull(int (*board)[SIZE])
{
    int i, j;
    i = 0;
    while (i < SIZE) {
        j = 0;
        while (j < SIZE) {
            if (board[i][j] == 0) {
                return 0;
            }
            j++;
        }
        i++;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Backtracking solver – used to generate the solution board         */
/* ------------------------------------------------------------------ */
int solveSudoku(int (*board)[SIZE])
{
    int i, j, num;
    i = 0;
    while (i < SIZE) {
        j = 0;
        while (j < SIZE) {
            if (board[i][j] == 0) {
                num = 1;
                while (num <= 9) {
                    if (isSafe(board, i, j, num)) {
                        board[i][j] = num;
                        if (solveSudoku(board)) {
                            return 1;
                        }
                        board[i][j] = 0;
                    }
                    num++;
                }
                return 0;
            }
            j++;
        }
        i++;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Count the number of lines (puzzles) in a file                    */
/* ------------------------------------------------------------------ */
int countLines(const char *filename)
{
    FILE *fp;
    char line[MAX_LINE];
    int count;

    count = 0;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fgets(line, MAX_LINE, fp) != NULL) {
        /* Only count lines that have at least 81 characters of digits */
        if (strlen(line) >= 81) {
            count++;
        }
    }

    fclose(fp);
    return count;
}

/* ------------------------------------------------------------------ */
/*  Load puzzle from difficulty file and solution from sol.txt        */
/* ------------------------------------------------------------------ */
void loadPuzzle(int (*board)[SIZE], int (*solution)[SIZE], int difficulty)
{
    const char *puzzle_filename;
    const char *sol_filename = "data/sol.txt";
    const char *section_header;
    FILE *puzzle_fp, *sol_fp;
    char line[MAX_LINE];
    int totalPuzzles, chosen, current;
    int i, j;

    switch (difficulty) {
        case 1:  
            puzzle_filename = "data/easy.txt";
            section_header = "[EASY]";
            break;
        case 2:  
            puzzle_filename = "data/med.txt";
            section_header = "[MED]";
            break;
        default: 
            puzzle_filename = "data/hard.txt";
            section_header = "[HARD]";
            break;
    }

    totalPuzzles = countLines(puzzle_filename);
    if (totalPuzzles == 0) {
        printf("Error: Could not open or read '%s'.\n", puzzle_filename);
        printf("Make sure the file exists and contains valid puzzles.\n");
        exit(1);
    }

    /* Pick a random puzzle index */
    chosen = rand() % totalPuzzles;

    puzzle_fp = fopen(puzzle_filename, "r");
    if (puzzle_fp == NULL) {
        printf("Error: Could not open '%s'.\n", puzzle_filename);
        exit(1);
    }

    current = 0;
    while (fgets(line, MAX_LINE, puzzle_fp) != NULL) {
        if (strlen(line) >= 81) {
            if (current == chosen) {
                break;
            }
            current++;
        }
    }
    fclose(puzzle_fp);

    /* Parse the 81-character line into the 9x9 board */
    i = 0;
    while (i < SIZE) {
        j = 0;
        while (j < SIZE) {
            board[i][j] = line[i * SIZE + j] - '0';
            j++;
        }
        i++;
    }

    /* Load corresponding solution from sol.txt */
    sol_fp = fopen(sol_filename, "r");
    if (sol_fp == NULL) {
        printf("Error: Could not open '%s'.\n", sol_filename);
        printf("Make sure sol.txt exists in the levels folder.\n");
        exit(1);
    }

    /* Skip to the correct section */
    int found_section = 0;
    while (fgets(line, MAX_LINE, sol_fp) != NULL) {
        if (strncmp(line, section_header, strlen(section_header)) == 0) {
            found_section = 1;
            break;
        }
    }

    if (!found_section) {
        printf("Error: Could not find section '%s' in sol.txt.\n", section_header);
        fclose(sol_fp);
        exit(1);
    }

    /* Read the chosen solution */
    current = 0;
    while (fgets(line, MAX_LINE, sol_fp) != NULL) {
        /* Skip section headers and empty lines */
        if (line[0] == '[' || strlen(line) < 81) {
            break;
        }
        
        if (current == chosen) {
            /* Parse the solution */
            i = 0;
            while (i < SIZE) {
                j = 0;
                while (j < SIZE) {
                    solution[i][j] = line[i * SIZE + j] - '0';
                    j++;
                }
                i++;
            }
            break;
        }
        current++;
    }

    fclose(sol_fp);
}

/* ================================================================== */
/*  MAIN                                                              */
/* ================================================================== */
int main(void)
{
    int board[SIZE][SIZE];
    int solution[SIZE][SIZE];
    int fixed[SIZE][SIZE];
    Player player;
    int curRow, curCol;
    int i, j;
    int emptyCells;
    int running;
    int key;
    int playAgain;
    char msg[128];

    srand((unsigned int)time(NULL));

    /* ============ Outer loop: replay ============ */
    playAgain = 1;
    while (playAgain) {



        /* ---- Home screen ---- */
        showHomeScreen(&player);

        /* ---- Load puzzle ---- */
        loadPuzzle(board, solution, player.difficulty);

        /* Mark fixed cells and count empties */
        emptyCells = 0;
        i = 0;
        while (i < SIZE) {
            j = 0;
            while (j < SIZE) {
                if (board[i][j] != 0) {
                    fixed[i][j] = 1;
                } else {
                    fixed[i][j] = 0;
                    emptyCells++;
                }
                j++;
            }
            i++;
        }

        curRow = 0;
        curCol = 0;
        msg[0] = '\0';

        /* ---- Main game loop ---- */
        running = 1;
        while (running) {
            drawBoard(board, fixed, curRow, curCol, msg, &player);
            msg[0] = '\0';

            if (isBoardFull(board)) {
                setColor(CLR_MSG_OK);
                printf("   ****  Congratulations! You completed the puzzle!  ****\n\n");
                setColor(CLR_RESET);
                break;
            }

            key = _getch();

            if (key == KEY_ESC) {
                running = 0;
                break;
            }

            if (key == 0 || key == ARROW_PREFIX) {
                key = _getch();
                switch (key) {
                    case ARROW_UP:
                        if (curRow > 0) curRow--;
                        break;
                    case ARROW_DOWN:
                        if (curRow < SIZE - 1) curRow++;
                        break;
                    case ARROW_LEFT:
                        if (curCol > 0) curCol--;
                        break;
                    case ARROW_RIGHT:
                        if (curCol < SIZE - 1) curCol++;
                        break;
                    default:
                        break;
                }
            } else if (key == 'w' || key == 'W') {
                if (curRow > 0) curRow--;
            } else if (key == 's' || key == 'S') {
                if (curRow < SIZE - 1) curRow++;
            } else if (key == 'a' || key == 'A') {
                if (curCol > 0) curCol--;
            } else if (key == 'd' || key == 'D') {
                if (curCol < SIZE - 1) curCol++;
            } else if (key >= '1' && key <= '9') {
                int num;
                num = key - '0';

                if (fixed[curRow][curCol]) {
                    snprintf(msg, sizeof(msg), "Cell (%d,%d) is fixed and cannot be changed.",
                             curRow + 1, curCol + 1);
                } else if (board[curRow][curCol] != 0) {
                    snprintf(msg, sizeof(msg), "Cell (%d,%d) is already filled.",
                             curRow + 1, curCol + 1);
                } else if (num == solution[curRow][curCol]) {
                    board[curRow][curCol] = num;
                    snprintf(msg, sizeof(msg), "Correct! Placed %d at (%d,%d).",
                             num, curRow + 1, curCol + 1);
                } else {
                    player.mistakes++;
                    snprintf(msg, sizeof(msg), "Wrong! %d does not go at (%d,%d). Mistakes: %d/%d",
                             num, curRow + 1, curCol + 1,
                             player.mistakes, MAX_MISTAKES);
                    if (player.mistakes >= MAX_MISTAKES) {
                        drawBoard(board, fixed, curRow, curCol, msg, &player);
                        setColor(CLR_MSG_ERR);
                        printf("   ****  Game Over! Too many mistakes.  ****\n\n");
                        setColor(CLR_RESET);

                        /* Show the solution */
                        setColor(CLR_PROMPT);
                        printf("   SOLUTION:\n\n");
                        setColor(CLR_COLNUM);
                        printf("        1    2    3     4    5    6     7    8    9\n");
                        {
                            int r, c;
                            r = 0;
                            while (r < SIZE) {
                                if (r % 3 == 0) {
                                    setColor(CLR_BORDER);
                                    printf("     +---------------+---------------+---------------+\n");
                                }
                                setColor(CLR_ROWNUM);
                                printf("  %d  ", r + 1);
                                setColor(CLR_BORDER);
                                printf("|");
                                c = 0;
                                while (c < SIZE) {
                                    if (c > 0 && c % 3 == 0) {
                                        setColor(CLR_BORDER);
                                        printf("|");
                                    }
                                    if (board[r][c] != solution[r][c]) {
                                        setColor(CLR_MSG_OK);
                                    } else if (fixed[r][c]) {
                                        setColor(CLR_FIXED);
                                    } else {
                                        setColor(CLR_PLACED);
                                    }
                                    printf("  %d  ", solution[r][c]);
                                    c++;
                                }
                                setColor(CLR_BORDER);
                                printf("|\n");
                                r++;
                            }
                            setColor(CLR_BORDER);
                            printf("     +---------------+---------------+---------------+\n\n");
                        }
                        setColor(CLR_RESET);
                        running = 0;
                    }
                }
            } else if (key == '0' || key == KEY_BACKSPACE || key == 127) {
                if (fixed[curRow][curCol]) {
                    snprintf(msg, sizeof(msg), "Cannot clear a fixed cell.");
                } else if (board[curRow][curCol] == 0) {
                    snprintf(msg, sizeof(msg), "Cell is already empty.");
                } else {
                    board[curRow][curCol] = 0;
                    snprintf(msg, sizeof(msg), "Cleared cell (%d,%d).",
                             curRow + 1, curCol + 1);
                }
            }
        }

        /* ---- Replay or Quit prompt ---- */
        printf("\n");
        setColor(CLR_BORDER);
        printf("   ========================================================\n\n");
        setColor(CLR_PROMPT);
        printf("   What would you like to do?\n\n");
        setColor(CLR_MENU_HL);
        printf("     [R]");
        setColor(CLR_MENU);
        printf("  Play Again\n");
        setColor(CLR_MENU_HL);
        printf("     [Q]");
        setColor(CLR_MENU);
        printf("  Quit\n\n");
        setColor(CLR_PROMPT);
        printf("   Choice: ");
        setColor(CLR_MENU);

        key = _getch();
        if (key == 'r' || key == 'R') {
            playAgain = 1;
        } else {
            playAgain = 0;
        }
    }

    /* ---- Goodbye ---- */
    clearScreen();
    printf("\n");
    setColor(CLR_TITLE);
    printf("   Thanks for playing CODOKU!\n");
    setColor(CLR_CREDITS);
    printf("   Goodbye!\n\n");
    setColor(CLR_RESET);

    return 0;
}
