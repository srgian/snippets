// gcc main.c -o sendkey.exe -mconsole
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define PRINT_OUT(fmt, ...) if (verbose) printf(fmt, ##__VA_ARGS__)
#define DEFAULT_INTERVAL 10  // Default interval in seconds
volatile int keepRunning = 1;
bool verbose = false;

// Ctrl+C handler
BOOL WINAPI handleConsoleSignal(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        PRINT_OUT("\nCtrl + C detected. Exiting program...\n");
        fflush(stdout);
        keepRunning = 0;
        return TRUE;
    }
    return FALSE;
}

// SIGTERM handler (for MSYS kill -15)
void handleSigterm(int sig) {
    PRINT_OUT("%d signal detected. Exiting program...\n", sig);
    fflush(stdout);
    keepRunning = 0;
}

// Convert a string key name to a virtual key code
int getKeyCode(const char *key, int *needsShift) {
    if (strcasecmp(key, "Shift") == 0) return VK_SHIFT;
    if (strcasecmp(key, "Ctrl") == 0) return VK_CONTROL;
    if (strcasecmp(key, "Alt") == 0) return VK_MENU;
    if (strcasecmp(key, "Win") == 0) return VK_LWIN;
    if (strcasecmp(key, "Enter") == 0) return VK_RETURN;
    if (strcasecmp(key, "Space") == 0) return VK_SPACE;
    if (strcasecmp(key, "Backspace") == 0) return VK_BACK;
    if (strcasecmp(key, "Tab") == 0) return VK_TAB;
    if (strcasecmp(key, "Esc") == 0) return VK_ESCAPE;
    if (strcasecmp(key, "Del") == 0) return VK_DELETE;

    // Function keys (F1–F24)
    if (strlen(key) == 2 && key[0] == 'F' && key[1] >= '1' && key[1] <= '9') {
        return VK_F1 + (key[1] - '1');
    }
    if (strlen(key) == 3 && key[0] == 'F' && key[1] >= '1' && key[1] <= '2' && key[2] >= '0' && key[2] <= '4') {
        return VK_F10 + (key[2] - '0');
    }

    if (strlen(key) == 1) {
        SHORT vk = VkKeyScanA(key[0]); 
        if (vk != -1) {
            *needsShift = HIBYTE(vk) & 1; // Check if Shift is required
            return LOBYTE(vk); 
        }
    }

    return 0;
}

// Function to simulate key presses from parsed keys
void simulateKeyPress(const char *macro) {
    INPUT inputs[10] = {0};  // Max 10 keys
    int count = 0;

    char temp[100];
    strcpy(temp, macro);
    
    char *token = strtok(temp, "+");
    while (token && count < 10) {
        int needsShift = 0;
        int keyCode = getKeyCode(token, &needsShift);
        if (keyCode) {
            if (needsShift) { // Press Shift first
                inputs[count].type = INPUT_KEYBOARD;
                inputs[count].ki.wVk = VK_SHIFT;
                count++;
            }
            inputs[count].type = INPUT_KEYBOARD;
            inputs[count].ki.wVk = keyCode;
            count++;
        } else {
            PRINT_OUT("Unknown key: %s\n", token);
        }
        token = strtok(NULL, "+");
    }

    // Press keys
    for (int i = 0; i < count; i++) {
        inputs[i].ki.dwFlags = 0;
    }
    SendInput(count, inputs, sizeof(INPUT));

    // Release keys (in reverse order)
    for (int i = count - 1; i >= 0; i--) {
        inputs[i].ki.dwFlags = KEYEVENTF_KEYUP;
    }
    SendInput(count, inputs, sizeof(INPUT));

    PRINT_OUT("Simulated: '%s'\n", macro);
}

int main(int argc, char *argv[]) {
    // Default values
    int interval = DEFAULT_INTERVAL * 1000;
    char macro[100] = "F20";  // Default key combination

    // Handle Ctrl+C
    SetConsoleCtrlHandler(handleConsoleSignal, TRUE);
    signal(SIGTERM, handleSigterm);

    // Parse arguments
    if (argc > 1) {
        interval = atoi(argv[1]) * 1000;  // Convert seconds to ms
    }
    if (argc > 2) {
        strncpy(macro, argv[2], sizeof(macro) - 1);
        macro[sizeof(macro) - 1] = '\0';  // Ensure null termination
    }
    if (argc > 3 && strcmp(argv[3], "-v") == 0) {
        verbose = true;
    }

    PRINT_OUT("Starting with interval: %d sec, macro: '%s'\n", interval / 1000, macro);
    
    int timeout = interval;
    while (keepRunning) {
        if (timeout <= 0) {
            simulateKeyPress(macro);
            timeout = interval;
        }
        timeout -= 1000;
        Sleep(1000);
    }

    PRINT_OUT("Program exited.\n");
    return 0;
}
