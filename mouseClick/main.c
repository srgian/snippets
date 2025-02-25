// gcc main.c -o mouseclick.exe -mconsole
/* can be used to trigger actions on certain console events
 * tail -f /path/to/logfile | grep --line-buffered "pattern" | while read -r line; do echo "Found pattern: $line"; some_command "$line"; done
 * see plink or other niceties
 */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void move_mouse(int x, int y) {
    SetCursorPos(x, y);
}

void click_mouse(int button) {
    DWORD dwButtonDown, dwButtonUp;

    // Determine the correct button
    switch(button) {
        case 1: // Left button
            dwButtonDown = MOUSEEVENTF_LEFTDOWN;
            dwButtonUp = MOUSEEVENTF_LEFTUP;
            break;
        case 2: // Right button
            dwButtonDown = MOUSEEVENTF_RIGHTDOWN;
            dwButtonUp = MOUSEEVENTF_RIGHTUP;
            break;
        case 3: // Middle button
            dwButtonDown = MOUSEEVENTF_MIDDLEDOWN;
            dwButtonUp = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            printf("Invalid button code!\n");
            return;
    }

    // Mouse button press and release (click)
    mouse_event(dwButtonDown, 0, 0, 0, 0);
    mouse_event(dwButtonUp, 0, 0, 0, 0);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <x> <y> <button> <click>\n", argv[0]);
        printf("<button>: 1 = Left, 2 = Right, 3 = Middle\n");
        printf("<click>: 1 = Click, 0 = No click\n");
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    int button = atoi(argv[3]);
    int click = atoi(argv[4]);

    // Move the mouse
    move_mouse(x, y);

    // Simulate click if requested (1 for click, 0 for no click)
    if (click == 1) {
        click_mouse(button);
    }

    printf("Mouse moved to (%d, %d)\n", x, y);
    if (click == 1) {
        switch (button) {
            case 1:
                printf("Left mouse button clicked!\n");
                break;
            case 2:
                printf("Right mouse button clicked!\n");
                break;
            case 3:
                printf("Middle mouse button clicked!\n");
                break;
        }
    }

    return 0;
}
