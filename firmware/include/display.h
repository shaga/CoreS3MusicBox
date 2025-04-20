#ifndef DISPLAY_H_
#define DISPLAY_H_

enum class ButtonType {
    kNone = -1,
    kRestart = 0,
    kPrev = 1,
    kNext = 2,
    kUp = 3,
    kDown = 4,
    kKeyUp = 5,
    kKeyDown = 6,
};

void initDisplay();
void showWaitMessage();
void showMusicTitle(const char *title, bool clear = false);

void updateSeekbar(int value);

void showRestartButton(bool is_pressed = false);

void showPrevButton(bool is_pressed = false);
void showNextButton(bool is_pressed = false);

void showUpButton(bool is_pressed = false, bool is_hide = false);
void showDownButton(bool is_pressed = false, bool is_hide = false);
void showSpeed(const char* speed);

void showKeyUpButton(bool is_pressed = false, bool is_hide = false);
void showKeyDownButton(bool is_pressed = false, bool is_hide = false);
void showKey(int key);

ButtonType loopButton();

#endif //DISPLAY_H_
