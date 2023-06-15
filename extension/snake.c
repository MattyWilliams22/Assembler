#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>


#define TICKDELAY 100000.0
#define WIDTH 20
#define HEIGHT 20
#define UP 'w'
#define DOWN 's'
#define LEFT 'a'
#define RIGHT 'd'

int score;
int highscore;
int gameover;
int x, y, fruitX, fruitY, flag;

int tailX[100], tailY[100];
int nTail;

double get_tick_speed() {
    double rate = 0.9;
    double multiplier = 1.0;
    for (int i = 0; i < nTail; i++) {
        multiplier *= rate;
    }
    return TICKDELAY * multiplier + 50000;
}

int keyboard_event()
{
    struct termios oldterminal, newterminal;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldterminal);
    newterminal = oldterminal;
    newterminal.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newterminal);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldterminal);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

void setup()
{
    system("clear");
    gameover = 0;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    nTail = 0;
    flag = -1;
}

void draw()
{
    system("clear");
    if (nTail > 0) {
        printf("Score: %d\n", score);
    } else {
        printf("Welcome to snake!\n");
    }
    
    int i, j;
    for (i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    for (i = 0; i < HEIGHT; i++)
    {
        for (j = 0; j < WIDTH; j++)
        {
            if (j == 0)
                printf("#");
            if (i == y && j == x)
                printf("O");
            else if (i == fruitY && j == fruitX)
                printf("X");
            else
            {
                int print = 0;
                for (int k = 0; k < nTail; k++)
                {
                    if (i == tailY[k] && j == tailX[k])
                    {
                        printf("o");
                        print = 1;
                    }
                }
                if (!print)
                    printf(" ");
            }
            if (j == WIDTH - 1)
                printf("#");
        }
        printf("\n");
    }

    for (i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    if (nTail == 0) {
        printf("Use WASD or Arrow Keys to move\n");
    }
}

void input()
{
    if (keyboard_event())
    {
        char key = getchar();
        int arrow = 0;
        if (key == 27) {
            getchar();
            arrow = getchar();
        }
        if ((key == UP || arrow == 65) && flag != 2) {
            flag = 1;
        } else if ((key == DOWN || arrow == 66) && flag != 1) {
            flag = 2;
        } else if ((key == LEFT || arrow == 68) && flag != 4) {
            flag = 3;
        } else if ((key == RIGHT || arrow == 67) && flag != 3) {
            flag = 4;
        } else if (key == 'x') {
            gameover = 1;
        }
    }
}

void logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (flag)
    {
    case 1:
        y--;
        break;
    case 2:
        y++;
        break;
    case 3:
        x--;
        break;
    case 4:
        x++;
        break;
    default:
        break;
    }
    if (x == -1) {
        x = WIDTH - 1;
    } else if (x == WIDTH) {
        x = 0;
    }
    if (y == -1) {
        y = HEIGHT - 1;
    } else if (y == HEIGHT) {
        y = 0;
    }
    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
        {
            gameover = 1;
            break;
        }
    }
    if (x == fruitX && y == fruitY)
    {
        score += 10;
        int touchesSnake;
        do {
            touchesSnake = 0;
            fruitX = rand() % WIDTH;
            fruitY = rand() % HEIGHT;
            for (int i = 0; i < nTail; i++) {
                if (fruitX == tailX[i] && fruitY == tailY[i]) {
                    touchesSnake = 1;
                }
            }
        } while (touchesSnake);
        draw();
        nTail++;
    } else {
        draw();
    }
}

int main()
{
    int exitProgram = 0;
    while (!exitProgram) {
        srand(time(0));
        setup();
        draw();
        while (!gameover)
        {
            input();
            logic();
            usleep(get_tick_speed()); // Delay controls speed of game
        }
        printf("\nGame Over!\n");
        printf("Your final score was %d\n", score);
        if (score > highscore) {
            highscore = score;
            printf("New Highscore!\n");
        } else {
            printf("The highscore is: %d\n", highscore);
        }
        char response;
        printf("Do you want to play another game? (y/n)\n");
        scanf(" %c", &response);
        if (response != 'y') {
            exitProgram = 1;
        }
    }
    

    return 0;
}
