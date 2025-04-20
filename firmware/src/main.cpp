#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include "EncHandle.h"
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <freertos/semphr.h>
#include <BLEMidi.h>
#include "music/PianoCat.h"
#include "music/TwinkleStar.h"
#include "music/BicCamera.h"
#include "music/Yodobashi.h"
#include "music/KimiwoNosete.h"
#include "music/Rydeen.h"
#include "music/FlyToTheMoon.h"
#include "MidiPlayer.h"
#include "display.h"

struct SpeedInfo_t {
  int event_count;
  const char* name;
};

static const int AutoNoteOffTicks = 50 / portTICK_RATE_MS;


static int midi_file_index = 0;
static int midi_file_count = 0;

static const int PinEncA = 7;
static const int PinEncB = 1;

static const int MaxKeyOffset = 16;
static const int MinKeyOffset = -16;

EncHandle encoderHandle(PinEncA, PinEncB);
static QueueHandle_t handleEncoderQueue;


static const int MusicCount = 7;
static int selected_music_index = 0;
static const MusicInfo_t Musics[] {
  PianoCatInfo,
  TwinkleLitteStarInfo,
  KimiwoNoseteInfo,
  RydeenInfo,
  FlyMeToTheMoonInfo,
  BicCameraInfo,
  YodobashiInfo,
};

static const int SpeedCount = 7;
static int selected_speed_indes = 3;
static const SpeedInfo_t Speeds[] {
  {
    18,
    "最遅"
  },
  {
    12,
    "超遅"
  },
  {
    8,
    "遅い"
  },
  {
    6,
    "普通"
  },
  {
    5,
    "速い"
  },
  {
    3,
    "超速"
  },
  {
    2,
    "最速"
  }
};

static int playing_timing = 0;
static int play_pos = 0;
static int play_len = 0;

static MidiPlayer midi_player;
static void initQueue() {
  handleEncoderQueue = xQueueCreate(10, sizeof(uint32_t));
}

static void handleTask(void* parameter) {
  portBASE_TYPE res;
  uint32_t v;
  char buffer[4];
  while (true) {
    res = xQueueReceive(handleEncoderQueue, &v, 100 / portTICK_PERIOD_MS);
    if (res != pdTRUE) {
      continue;
    }

    const MusicInfo_t* selected = &Musics[selected_music_index];
    playing_timing++;
    if (play_len > 0) {
      play_len--;
    } else {
      play_pos++;

      {
        int v = playing_timing * 100 / selected->counts_by_sixteens;
        updateSeekbar(v);
      }

      if (play_pos < selected->length) {
        if (selected->data[play_pos].size > 0)
          midi_player.noteOn(&selected->data[play_pos]);
        play_len = selected->data[play_pos].length -1;
      } else if (play_pos == selected->length) {
        play_len = 4;
      } else {
        play_pos = -1;
        playing_timing = 0;
      }
    }
  }
}

static void initTask() {
  xTaskCreate(handleTask, "HandleTask", 4096, NULL, 1, NULL);
}

static void changeMusic(int offset) {
  if (!midi_player.is_connected()) return;
  encoderHandle.pause();

  midi_player.prevNoteOff();

  selected_music_index = (selected_music_index + offset + MusicCount) % MusicCount;
  showMusicTitle(Musics[selected_music_index].name);
  play_pos = -1;
  play_len = 0;
  playing_timing = 0;
  updateSeekbar(0);

  encoderHandle.resume();
}

static void changeSpeed(int offset) {
  if (!midi_player.is_connected()) return;

  int next = selected_speed_indes + offset;
  if (next >= SpeedCount) return;
  else if (next < 0) return;

  encoderHandle.setEventCount(Speeds[next].event_count);

  if (next == 0) {
    showDownButton(false, true);
  } else {
    showDownButton(false);
  }

  if (next == SpeedCount - 1) {
    showUpButton(false, true);
  } else {
    showUpButton(false);
  }

  showSpeed(Speeds[next].name);

  selected_speed_indes = next;
}

static void changeKey(int offset) {
  if (!midi_player.is_connected()) return;

  int next = midi_player.key_offset() + offset;
  if (next > MaxKeyOffset) return;
  else if (next < MinKeyOffset) return;

  if (next == MaxKeyOffset) {
    showKeyUpButton(false, true);
  } else {
    showKeyUpButton(false);
  }

  if (next == MinKeyOffset) {
    showKeyDownButton(false, true);
  } else {
    showKeyDownButton(false);
  }

  showKey(next);
  midi_player.setKeyOffset(next);
}

static void onBleMidiConnectionChanged(bool isConnected) {
  if (isConnected) {
    showMusicTitle(Musics[selected_music_index].name, true);
    updateSeekbar(0);
    showRestartButton();
    showPrevButton();
    showNextButton();
    showUpButton();
    showDownButton();
    encoderHandle.resume();
    changeSpeed(0);
    changeKey(0);
  } else {
    showWaitMessage();
    encoderHandle.pause();
  }
}

void setup() {
  // auto cfg = M5.config();  
  // M5.begin(cfg);
  // M5.Power.begin();
  // M5.Power.setExtPower(false);
  // M5.Power.setExtOutput(false);
  //M5.Touch.begin();


  initDisplay();

  midi_player.registerCallback(onBleMidiConnectionChanged);
  midi_player.begin();
  showWaitMessage();

  initQueue();
  initTask();
  encoderHandle.begin(handleEncoderQueue);
}

void loop() {
  // M5.update();
  Serial.begin(119200);

  if (!midi_player.is_connected()) return;

  auto clicked_button = loopButton();

  switch (clicked_button) {
    case ButtonType::kRestart:
      midi_player.prevNoteOff();
      updateSeekbar(0);
      playing_timing = 0;
      play_pos = -1;
      play_len = 0;
      break;
    case ButtonType::kPrev:
      changeMusic(-1);
      break;
    case ButtonType::kNext:
      changeMusic(1);
      break;
    case ButtonType::kUp:
      changeSpeed(1);
      break;
    case ButtonType::kDown:
      changeSpeed(-1);
      break;
    case ButtonType::kKeyUp:
      changeKey(2);
      break;
    case ButtonType::kKeyDown:
      changeKey(-2);
      break;
  }
  // {
  //   int pos = play_pos * 100 / Musics[selected_music_index].length;
  //   updateSeekbar(pos);
  // }
}
