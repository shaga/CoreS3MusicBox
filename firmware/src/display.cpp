#include "display.h"
#include "M5GFX.h"
#include "M5Unified.h"

static M5GFX display;

static M5Canvas canvas_music_title(&display);
static M5Canvas canvas_seek_bar(&display);
static M5Canvas canvas_restart_button(&display);
static M5Canvas canvas_prev_button(&display);
static M5Canvas canvas_next_button(&display);
static M5Canvas canvas_up_button(&display);
static M5Canvas canvas_down_button(&display);
static M5Canvas canvas_speed(&display);
static M5Canvas canvas_key_up_button(&display);
static M5Canvas canvas_key_down_button(&display);
static M5Canvas canvas_key_display(&display);

struct Button_t {
    int x;
    int y;
    int w;
    int h;
    bool is_pressed = false;;
    bool is_hide = false;
    int touch_id = -1;

    bool contain(int x, int y) {
        return this->x <= x && x < (this->x + this->w) && this->y <= y && y < (this->y + this->h);
    }
};

static Button_t restart_button;
static Button_t prev_button;
static Button_t next_button;
static Button_t up_button;
static Button_t down_button;
static Button_t key_up_button;
static Button_t key_down_button;

#define PREV_NEXT_BUTTON_HEIGHT     (40)
#define PREV_NEXT_BUTTON_WIDTH      (display.width () / 2 - 20)
#define PREV_NEXT_BUTTON_OFFSET_Y   (0)
#define PREV_BUTTON_OFFSET_X        (0)
#define NEXT_BUTTON_OFFSET_X        (display.width() - PREV_NEXT_BUTTON_WIDTH)

#define MUSIC_TITLE_OFFSET_X        (0)
#define MUSIC_TITLE_OFFSET_Y        (40)
#define MUSIC_TITLE_WIDTH           (display.width())
#define MUSIC_TITLE_HEIGHT          (120)

#define RESTART_BUTTON_WIDTH        (40)
#define RESTART_BUTTON_HEIGHT       (40)
#define RESTART_BUTTON_OFFSET_X     (0)
#define RESTART_BUTTON_OFFSET_Y     (160)

#define SEEKBAR_CANVAS_OFFSET_X     (50)
#define SEEKBAR_CANVAS_WIDTH        (display.width() - SEEKBAR_CANVAS_OFFSET_X)
#define SEEKBAR_CANVAS_HEIGHT       (RESTART_BUTTON_HEIGHT)
#define SEEKBAR_CANVAS_OFFSET_Y     (160)
#define SEEKBAR_MARGIN_X            (5)
#define SEEKBAR_MARGIN_B            (10)
#define SEEKBAR_WIDTH               (canvas_seek_bar.width() - SEEKBAR_MARGIN_X * 2)
#define SEEKBAR_HEIGHT              (4)
#define SEEKBAR_OFFSET_Y            ((SEEKBAR_CANVAS_HEIGHT - SEEKBAR_HEIGHT) / 2)
#define SEEKBAR_THUMB_WIDTH         (10)
#define SEEKBAR_THUMB_HEIGHT        (SEEKBAR_CANVAS_HEIGHT)
#define SEEKBAR_THUMB_OFFSET_X      (0 - SEEKBAR_THUMB_WIDTH / 2)

#define SPEED_BUTTON_Y              (220)
#define SPEED_BUTTON_WIDTH          (60)
#define SPEED_BUTTON_HEIGHT         (40)
#define SPEED_DOWN_BUTTON_X         (0)
#define SPEED_UP_BUTTON_X           (display.width() - SPEED_BUTTON_WIDTH)
#define SPEED_METER_X               (SPEED_BUTTON_WIDTH + 10)
#define SPPED_METER_Y               (SPEED_BUTTON_Y)
#define SPEED_METER_WIDTH           (display.width() - (SPEED_BUTTON_WIDTH + 10) * 2)
#define SPEED_METER_HEIGHT          (SPEED_BUTTON_HEIGHT)

#define KEY_BUTTON_Y                (280)
#define KEY_BUTTON_WIDTH            (60)
#define KEY_BUTTON_HEIGHT           (40)
#define KEY_DOWN_BUTTON_X           (0)
#define KEY_UP_BUTTON_X             (display.width() - KEY_BUTTON_WIDTH)
#define KEY_DISPLAY_X               (KEY_BUTTON_WIDTH + 10)
#define KEY_DISPLAY_Y               (KEY_BUTTON_Y)
#define KEY_DISPLAY_WIDTH           (display.width() - (KEY_DISPLAY_X) * 2)
#define KEY_DISPLAY_HEIGHT          (KEY_BUTTON_HEIGHT)

void initDisplay() {
    display.begin();
    display.setRotation(0);

    M5.addDisplay(display);

    display.setTextDatum(middle_center);
    display.setFont(&fonts::lgfxJapanGothicP_32);
    display.setTextColor(WHITE);

    canvas_music_title.createSprite(MUSIC_TITLE_WIDTH, MUSIC_TITLE_HEIGHT);
    canvas_music_title.setColorDepth(8);
    canvas_music_title.setTextColor(WHITE, BLACK);
    canvas_music_title.setTextWrap(true);
    canvas_music_title.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_music_title.setTextDatum(top_left);

    canvas_seek_bar.createSprite(SEEKBAR_CANVAS_WIDTH, SEEKBAR_CANVAS_HEIGHT);

    canvas_restart_button.createSprite(RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT);
    restart_button.x = RESTART_BUTTON_OFFSET_X;
    restart_button.y = RESTART_BUTTON_OFFSET_Y;
    restart_button.w = RESTART_BUTTON_WIDTH;
    restart_button.h = RESTART_BUTTON_HEIGHT;

    canvas_prev_button.createSprite(PREV_NEXT_BUTTON_WIDTH, PREV_NEXT_BUTTON_HEIGHT);
    canvas_prev_button.setColorDepth(8);
    canvas_prev_button.setTextColor(BLACK);
    canvas_prev_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_prev_button.setTextDatum(middle_center);
    prev_button.x = PREV_BUTTON_OFFSET_X;
    prev_button.y = PREV_NEXT_BUTTON_OFFSET_Y;
    prev_button.w = PREV_NEXT_BUTTON_WIDTH;
    prev_button.h = PREV_NEXT_BUTTON_HEIGHT;

    canvas_next_button.createSprite(PREV_NEXT_BUTTON_WIDTH, PREV_NEXT_BUTTON_HEIGHT);
    canvas_next_button.setColorDepth(8);
    canvas_next_button.setTextColor(BLACK);
    canvas_next_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_next_button.setTextDatum(middle_center);
    next_button.x = NEXT_BUTTON_OFFSET_X;
    next_button.y = PREV_NEXT_BUTTON_OFFSET_Y;
    next_button.w = PREV_NEXT_BUTTON_WIDTH;
    next_button.h = PREV_NEXT_BUTTON_HEIGHT;

    canvas_up_button.createSprite(SPEED_BUTTON_WIDTH, SPEED_BUTTON_HEIGHT);
    canvas_up_button.setColorDepth(8);
    canvas_up_button.setTextColor(BLACK);
    canvas_up_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_up_button.setTextDatum(middle_center);
    up_button.x = SPEED_UP_BUTTON_X;
    up_button.y = SPEED_BUTTON_Y;
    up_button.w = SPEED_BUTTON_WIDTH;
    up_button.h = SPEED_BUTTON_HEIGHT;

    canvas_down_button.createSprite(SPEED_BUTTON_WIDTH, SPEED_BUTTON_HEIGHT);
    canvas_down_button.setColorDepth(8);
    canvas_down_button.setTextColor(BLACK);
    canvas_down_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_down_button.setTextDatum(middle_center);
    down_button.x = SPEED_DOWN_BUTTON_X;
    down_button.y = SPEED_BUTTON_Y;
    down_button.w = SPEED_BUTTON_WIDTH;
    down_button.h = SPEED_BUTTON_HEIGHT;

    canvas_speed.createSprite(SPEED_METER_WIDTH, SPEED_METER_HEIGHT);
    canvas_speed.setColorDepth(8);
    canvas_speed.setTextColor(WHITE, BLACK);
    canvas_speed.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_speed.setTextDatum(middle_center);

    canvas_key_up_button.createSprite(KEY_BUTTON_WIDTH, KEY_BUTTON_HEIGHT);
    canvas_key_up_button.setColorDepth(8);
    canvas_key_up_button.setTextColor(BLACK);
    canvas_key_up_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_key_up_button.setTextDatum(middle_center);
    key_up_button.x = KEY_UP_BUTTON_X;
    key_up_button.y = KEY_BUTTON_Y;
    key_up_button.w = KEY_BUTTON_WIDTH;
    key_up_button.h = KEY_BUTTON_HEIGHT;

    canvas_key_down_button.createSprite(KEY_BUTTON_WIDTH, KEY_BUTTON_HEIGHT);
    canvas_key_down_button.setColorDepth(8);
    canvas_key_down_button.setTextColor(BLACK);
    canvas_key_down_button.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_key_down_button.setTextDatum(middle_center);
    key_down_button.x = KEY_DOWN_BUTTON_X;
    key_down_button.y = KEY_BUTTON_Y;
    key_down_button.w = KEY_BUTTON_WIDTH;
    key_down_button.h = KEY_BUTTON_HEIGHT;

    canvas_key_display.createSprite(KEY_DISPLAY_WIDTH, KEY_DISPLAY_HEIGHT);
    canvas_key_display.setColorDepth(8);
    canvas_key_display.setTextColor(WHITE, BLACK);
    canvas_key_display.setFont(&fonts::lgfxJapanGothicP_32);
    canvas_key_display.setTextDatum(middle_center);
}

void showWaitMessage() {
    display.clear();
    display.drawString("接続待ち", display.width() / 2, display.height() / 2);
}

void showMusicTitle(const char *title, bool clear) {
    if (clear) display.clear();
    canvas_music_title.clear();
    canvas_music_title.setCursor(0, 0);
    canvas_music_title.print(title);
    canvas_music_title.pushSprite(MUSIC_TITLE_OFFSET_X, MUSIC_TITLE_OFFSET_Y);
}

void updateSeekbar(int value) {
    if (value < 0) value = 0;
    else if (value > 100) value = 100;
    canvas_seek_bar.clear();
    canvas_seek_bar.fillRect(SEEKBAR_MARGIN_X, SEEKBAR_OFFSET_Y, SEEKBAR_WIDTH, SEEKBAR_HEIGHT, WHITE);

    int seek_pos = SEEKBAR_MARGIN_X + SEEKBAR_WIDTH * value / 100;
    canvas_seek_bar.fillRect(seek_pos + SEEKBAR_THUMB_OFFSET_X, 0, SEEKBAR_THUMB_WIDTH, SEEKBAR_THUMB_HEIGHT, RED);

    canvas_seek_bar.pushSprite(SEEKBAR_CANVAS_OFFSET_X, SEEKBAR_CANVAS_OFFSET_Y);
}

void showRestartButton(bool is_pressed) {
    canvas_restart_button.fillRect(0, 0, RESTART_BUTTON_WIDTH, RESTART_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
    canvas_restart_button.fillRect(2, 2, RESTART_BUTTON_WIDTH - 4, RESTART_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
    canvas_restart_button.fillArc(RESTART_BUTTON_WIDTH / 2, RESTART_BUTTON_HEIGHT / 2, 10, 12, -90, 180, BLACK);
    canvas_restart_button.fillTriangle(RESTART_BUTTON_WIDTH / 2 - 8, 9, RESTART_BUTTON_WIDTH / 2, 5, RESTART_BUTTON_WIDTH / 2, 15, BLACK);

    canvas_restart_button.pushSprite(RESTART_BUTTON_OFFSET_X, RESTART_BUTTON_OFFSET_Y);
}

void showPrevButton(bool is_pressed) {
    canvas_prev_button.fillRect(0, 0, PREV_NEXT_BUTTON_WIDTH, PREV_NEXT_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
    canvas_prev_button.fillRect(2, 2, PREV_NEXT_BUTTON_WIDTH - 4, PREV_NEXT_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
    canvas_prev_button.drawString("<<", PREV_NEXT_BUTTON_WIDTH / 2, PREV_NEXT_BUTTON_HEIGHT / 2);

    canvas_prev_button.pushSprite(PREV_BUTTON_OFFSET_X, PREV_NEXT_BUTTON_OFFSET_Y);
}

void showNextButton(bool is_pressed) {
    canvas_next_button.fillRect(0, 0, PREV_NEXT_BUTTON_WIDTH, PREV_NEXT_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
    canvas_next_button.fillRect(2, 2, PREV_NEXT_BUTTON_WIDTH - 4, PREV_NEXT_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
    canvas_next_button.drawString(">>", PREV_NEXT_BUTTON_WIDTH / 2, PREV_NEXT_BUTTON_HEIGHT / 2);

    canvas_next_button.pushSprite(NEXT_BUTTON_OFFSET_X, PREV_NEXT_BUTTON_OFFSET_Y);
}

void showUpButton(bool is_pressed, bool is_hide) {
    if (is_hide) {
        canvas_up_button.clear();
    } else {
        canvas_up_button.fillRect(0, 0, SPEED_BUTTON_WIDTH, SPEED_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
        canvas_up_button.fillRect(2, 2, SPEED_BUTTON_WIDTH - 4, SPEED_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
        canvas_up_button.drawString("↑", SPEED_BUTTON_WIDTH / 2, PREV_NEXT_BUTTON_HEIGHT / 2);
    }
    canvas_up_button.pushSprite(SPEED_UP_BUTTON_X, SPEED_BUTTON_Y);
    up_button.is_hide = is_hide;
}

void showDownButton(bool is_pressed, bool is_hide) {
    if (is_hide) {
        canvas_down_button.clear();
    } else {
        canvas_down_button.fillRect(0, 0, SPEED_BUTTON_WIDTH, SPEED_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
        canvas_down_button.fillRect(2, 2, SPEED_BUTTON_WIDTH - 4, SPEED_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
        canvas_down_button.drawString("↓", SPEED_BUTTON_WIDTH / 2, PREV_NEXT_BUTTON_HEIGHT / 2);
    }
    canvas_down_button.pushSprite(SPEED_DOWN_BUTTON_X, SPEED_BUTTON_Y);
    down_button.is_hide = is_hide;
}

void showSpeed(const char* speed) {
    canvas_speed.drawString(speed, SPEED_METER_WIDTH / 2, SPEED_METER_HEIGHT / 2);
    canvas_speed.pushSprite(SPEED_METER_X, SPPED_METER_Y);
}

void showKeyUpButton(bool is_pressed, bool is_hide) {
    if (is_hide) {
        canvas_key_up_button.clear();
    } else {
        canvas_key_up_button.fillRect(0, 0, KEY_BUTTON_WIDTH, KEY_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
        canvas_key_up_button.fillRect(2, 2, KEY_BUTTON_WIDTH - 4, KEY_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
        canvas_key_up_button.drawString("+", KEY_BUTTON_WIDTH / 2, KEY_BUTTON_HEIGHT / 2);
    }

    canvas_key_up_button.pushSprite(KEY_UP_BUTTON_X, KEY_BUTTON_Y);
    key_up_button.is_hide = is_hide;
}

void showKeyDownButton(bool is_pressed, bool is_hide) {
    if (is_hide) {
        canvas_key_down_button.clear();
    } else {
        canvas_key_down_button.fillRect(0, 0, KEY_BUTTON_WIDTH, KEY_BUTTON_HEIGHT, is_pressed ? DARKGREY : LIGHTGREY);
        canvas_key_down_button.fillRect(2, 2, KEY_BUTTON_WIDTH - 4, KEY_BUTTON_HEIGHT - 4, is_pressed ? LIGHTGREY : WHITE);
        canvas_key_down_button.drawString("-", KEY_BUTTON_WIDTH / 2, KEY_BUTTON_HEIGHT / 2);
    }

    canvas_key_down_button.pushSprite(KEY_DOWN_BUTTON_X, KEY_BUTTON_Y);
    key_down_button.is_hide = is_hide;
}

void showKey(int key) {
    char buffer[10];
    if (key == 0) {
        sprintf(buffer, "±0");
    } else if (key > 0) {
        sprintf(buffer, "＋%d", key);
    } else {
        sprintf(buffer, "‐%d", (0-key));
    }

    canvas_key_display.clear();
    canvas_key_display.drawString(buffer, KEY_DISPLAY_WIDTH / 2, KEY_DISPLAY_HEIGHT / 2);
    canvas_key_display.pushSprite(KEY_DISPLAY_X, KEY_DISPLAY_Y);
}

ButtonType loopButton() {
    ButtonType ret = ButtonType::kNone;
    M5.Touch.update(millis());
    auto count = M5.Touch.getCount();

    if (count == 0) return ret;


    for (size_t i = 0; i < count; i++) {
        auto t = M5.Touch.getDetail(i);


        if (t.wasPressed()) {
            Serial.printf("%3d, %3d\n", t.x, t.y);
            if (restart_button.contain(t.x, t.y)) {
                restart_button.touch_id = t.id;
                restart_button.is_pressed = true;
                showRestartButton(true);
            } else if (prev_button.contain(t.x, t.y)) {
                prev_button.touch_id = t.id;
                prev_button.is_pressed = true;
                showPrevButton(true);
            } else if (next_button.contain(t.x, t.y)) {
                next_button.touch_id = t.id;
                next_button.is_pressed = true;
                showNextButton(true);
            } else if (!up_button.is_hide && up_button.contain(t.x, t.y)) {
                up_button.touch_id = t.id;
                up_button.is_pressed = true;
                showUpButton(true);
            } else if (!down_button.is_hide && down_button.contain(t.x, t.y)) {
                down_button.touch_id = t.id;
                down_button.is_pressed = true;
                showDownButton(true);
            } else if (!key_up_button.is_hide && key_up_button.contain(t.x, t.y)) {
                Serial.println("key up");
                key_up_button.touch_id = t.id;
                key_up_button.is_pressed = true;
                showKeyUpButton(true);
            } else if (!key_down_button.is_hide && key_down_button.contain(t.x, t.y)) {
                Serial.println("key down");
                key_down_button.touch_id = t.id;
                key_down_button.is_pressed = true;
                showKeyDownButton(true);
            } else {
                Serial.printf("x: %d, y: %d\n", t.x, t.y);
            }
        } else if (t.isDragging() || t.isFlicking()) {
            if (restart_button.touch_id == t.id) {
                if (!restart_button.is_pressed && restart_button.contain(t.x, t.y)) {
                    // 外れたものが戻ってきた
                    restart_button.is_pressed = true;
                    showRestartButton(true);
                } else if (restart_button.is_pressed && !restart_button.contain(t.x, t.y)) {
                    // ボタン外にタッチが外れた
                    restart_button.is_pressed = false;
                    showRestartButton(false);
                }
            } else if (prev_button.touch_id == t.id) {
                if (!prev_button.is_pressed && prev_button.contain(t.x, t.y)) {
                    // 外れたものが戻ってきた
                    prev_button.is_pressed = true;
                    showPrevButton(true);
                } else if (prev_button.is_pressed && !prev_button.contain(t.x, t.y)) {
                    // ボタン外にタッチが外れた
                    prev_button.is_pressed = false;
                    showPrevButton(false);
                }
            } else if (next_button.touch_id == t.id) {
                if (!next_button.is_pressed && next_button.contain(t.x, t.y)) {
                    // 外れたものが戻ってきた
                    next_button.is_pressed = true;
                    showNextButton(true);
                } else if (next_button.is_pressed && !next_button.contain(t.x, t.y)) {
                    // ボタン外にタッチが外れた
                    next_button.is_pressed = false;
                    showNextButton(false);
                }
            } else if (up_button.touch_id == t.id) {
                if (!up_button.is_pressed && up_button.contain(t.x, t.y)) {
                    up_button.is_pressed = true;
                    showUpButton(true);
                } else if (up_button.is_pressed && !up_button.contain(t.x, t.y)) {
                    up_button.is_pressed = false;
                    showUpButton(false);
                }
            } else if (down_button.touch_id == t.id) {
                if (!down_button.is_pressed && down_button.contain(t.x, t.y)) {
                    down_button.is_pressed = true;
                    showDownButton(true);
                } else if (down_button.is_pressed && !down_button.contain(t.x, t.y)) {
                    down_button.is_pressed = false;
                    showDownButton(false);
                }
            } else if (key_up_button.touch_id == t.id) {
                if (!key_up_button.is_pressed && key_up_button.contain(t.x, t.y)) {
                    key_up_button.is_pressed = true;
                    showKeyUpButton(true);
                } else if (key_up_button.is_pressed && !key_up_button.contain(t.x, t.y)) {
                    key_up_button.is_pressed = false;
                    showKeyUpButton(false);
                }
            } else if (key_down_button.touch_id == t.id) {
                if (!key_down_button.is_pressed && key_down_button.contain(t.x, t.y)) {
                    key_down_button.is_pressed = true;
                    showKeyDownButton(true);
                } else if (key_down_button.is_pressed && !key_down_button.contain(t.x, t.y)) {
                    key_down_button.is_pressed = false;
                    showKeyDownButton(false);
                }
            }
        } else if (t.wasReleased()) {
            if (restart_button.touch_id == t.id) {
                if (restart_button.contain(t.x, t.y)) {
                    // ボタン内でリリースされたその演奏を先頭から
                    ret = ButtonType::kRestart;
                }
                restart_button.touch_id = -1;
                restart_button.is_pressed = false;
                showRestartButton(false);
            } else if (prev_button.touch_id == t.id) {
                if (prev_button.contain(t.x, t.y)) {
                    // ボタン内でリリースされたので前の曲に
                    ret = ButtonType::kPrev;
                }
                prev_button.touch_id = -1;
                prev_button.is_pressed = false;
                showPrevButton(false);
            } else if (next_button.touch_id == t.id) {
                if (next_button.contain(t.x, t.y)) {
                    // ボタン内でリリースされたので次の曲に
                    ret = ButtonType::kNext;
                }
                next_button.touch_id = -1;
                next_button.is_pressed = false;
                showNextButton(false);
            } else if (up_button.touch_id == t.id) {
                if (up_button.contain(t.x, t.y)) {
                    ret = ButtonType::kUp;
                }
                up_button.touch_id = -1;
                up_button.is_pressed = false;
                showUpButton(false);
            } else if (down_button.touch_id == t.id) {
                if (down_button.contain(t.x, t.y)) {
                    ret = ButtonType::kDown;
                }
                down_button.touch_id = -1;
                down_button.is_pressed = false;
                showDownButton(false);
            } else if (key_up_button.touch_id == t.id) {
                if (key_up_button.contain(t.x, t.y)) {
                    ret = ButtonType::kKeyUp;
                }
                key_up_button.touch_id = -1;
                key_up_button.is_pressed = false;
                showKeyUpButton(false);
            } else if (key_down_button.touch_id == t.id) {
                if (key_down_button.contain(t.x, t.y)) {
                    ret = ButtonType::kKeyDown;
                }
                key_down_button.touch_id = -1;
                key_down_button.is_pressed = false;
                showKeyDownButton(false);
            }
        }
    }

    return ret;
}