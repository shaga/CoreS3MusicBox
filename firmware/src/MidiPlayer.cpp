#include <BLEMidi.h>
#include "MidiPlayer.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"


static const TickType_t FADEOUT_TIMER_TICK = 50 / portTICK_RATE_MS;
static const TickType_t NOTE_SEM_WAIT_TICK = 10 / portTICK_RATE_MS;
static const TickType_t FADEOU_QUEUE_SEND_TICK = 10 / portTICK_RATE_MS;
static const TickType_t FADETE_QUEUE_WAIT_TICK = 50 / portTICK_RATE_MS;

const uint8_t MidiPlayer::DEFAULT_VELOICTY = 120;

static ConnectionChangedCallback_t connection_changed = NULL;

static QueueHandle_t queueFadeout;
static TimerHandle_t timerFadeout;
static SemaphoreHandle_t semaphoreNote;

#define TIMER_IS_ACTIVE()        (xTimerIsTimerActive(timerFadeout) == pdTRUE)

static MidiPlayer* player = NULL;

static void onConnected() {
    if (connection_changed != NULL) {
        connection_changed(true);
    }
}

static void onDisconnected() {
    if (connection_changed != NULL) {
        connection_changed(false);
    }
}

static void fadeoutTask(void* parameter) {
    uint32_t value;
    while (true) {
        // キューの受信待ち
        while (xQueueReceive(queueFadeout, &value, FADETE_QUEUE_WAIT_TICK) != pdTRUE && player == NULL);

        if (player->is_note_on() && xSemaphoreTake(semaphoreNote, NOTE_SEM_WAIT_TICK) == pdTRUE) {
            player->fadeout();
            xSemaphoreGive(semaphoreNote);
        }
    }
}

static void fadeoutTimerProc(TimerHandle_t timer) {
    uint32_t v = 0;
    xQueueSend(queueFadeout, &v, FADEOU_QUEUE_SEND_TICK);
}


MidiPlayer::MidiPlayer(uint8_t channel): channel_(channel) {
    fadeout_size_ = 2;
    prev_note_ = NULL;
    prev_key_offset_ = 0;
    key_offset_ = 0;
}

void MidiPlayer::begin() {
    player = this;
    BLEMidiServer.begin("M5 Music Box");

    BLEMidiServer.setOnConnectCallback(onConnected);
    BLEMidiServer.setOnDisconnectCallback(onDisconnected);

    semaphoreNote = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphoreNote);
    queueFadeout = xQueueCreate(10, sizeof(uint32_t));
    timerFadeout = xTimerCreate("fade out timer", FADEOUT_TIMER_TICK, pdTRUE, NULL, fadeoutTimerProc);

    xTaskCreate(fadeoutTask, "fadeout task", 4096, NULL, 1, NULL);
}

void MidiPlayer::registerCallback(ConnectionChangedCallback_t callback) {
    connection_changed = callback;
}

bool MidiPlayer::is_connected() {
    return BLEMidiServer.isConnected();
}

void MidiPlayer::noteOn(const NoteData_t *note) {
    if (!BLEMidiServer.isConnected()) return;

    if (note == NULL) return;

    while (xSemaphoreTake(semaphoreNote, NOTE_SEM_WAIT_TICK) != pdTRUE);

    prevNoteOff(false);

    velocity_ = DEFAULT_VELOICTY;
    for (int i = 0; i < note->size; i++) {
        BLEMidiServer.noteOn(channel_, (uint8_t)(note->value[i] + key_offset_), velocity_);
    }

    prev_key_offset_ = key_offset_;
    prev_note_ = (NoteData_t*)note;

    if (note->size > 0) {
        xTimerStart(timerFadeout, 0);
    }

    xSemaphoreGive(semaphoreNote);
}

void MidiPlayer::noteOff(const NoteData_t *note, int key_offset, bool need_sem) {
    if (!BLEMidiServer.isConnected()) return;

    if (note == NULL) return;
    velocity_ = 0;

    for (int i = 0; i < note->size; i++) {
        BLEMidiServer.noteOff(channel_, (uint8_t)(note->value[i] + key_offset), 0);
    }
}

void MidiPlayer::prevNoteOff(bool need_sem) {
    if (need_sem) {
        while (xSemaphoreTake(semaphoreNote, NOTE_SEM_WAIT_TICK) != pdTRUE);
    }

    noteOff(prev_note_, prev_key_offset_);
    prev_note_ = NULL;
    
    if (TIMER_IS_ACTIVE()) xTimerStop(timerFadeout, 0);


    if (need_sem) {
        xSemaphoreGive(semaphoreNote);
    }
}

bool MidiPlayer::fadeout() {
    if (!BLEMidiServer.isConnected()) return true;
    if (velocity_ == 0) return true;

    if (velocity_ > fadeout_size_) {
        velocity_ -= fadeout_size_;
        BLEMidiServer.controlChange(channel_, 11, velocity_);
    } else {
        prevNoteOff(false);
    }

    return velocity_ == 0;
}

void MidiPlayer::setKeyOffset(int key_offset) {
    key_offset_ = key_offset;
}