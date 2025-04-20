#ifndef MIDI_PLAYER_H_
#define MIDI_PLAYER_H_

#include <Arduino.h>

#include "MusicData.h"

typedef void (*ConnectionChangedCallback_t)(bool);

class MidiPlayer {
public:
    MidiPlayer(uint8_t channel = 1);
    void begin();
    void registerCallback(ConnectionChangedCallback_t callback);

    void noteOn(const NoteData_t *note);
    void noteOff(const NoteData_t *note, int key_offset = 0, bool need_sem = true);
    void prevNoteOff(bool need_sem = true);
    bool fadeout();

    bool is_connected();

    bool is_note_on() { return prev_note_ != NULL; }

    void setKeyOffset(int key_offset);

    int key_offset() { return key_offset_; }
private:
    static const uint8_t DEFAULT_VELOICTY;
    
    uint8_t channel_;
    uint8_t velocity_;
    uint8_t fadeout_size_;
    NoteData_t* prev_note_;
    int key_offset_;
    int prev_key_offset_;
};

#endif //MIDI_PLAYER_H_