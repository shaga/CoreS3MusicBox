#ifndef __MUSIC_DATA_H__
#define __MUSIC_DATA_H__
#include "Arduino.h"

#define MAX_NOTE_COUNT   (2)

typedef struct {
  uint8_t size;
  uint8_t value[MAX_NOTE_COUNT];
  uint8_t length;
} NoteData_t;

typedef struct {
  const char *name;
  int length;
  const NoteData_t* data;
  int counts_by_sixteens;
} MusicInfo_t;

#define SINGLE_PITCH_NOTE(key, len)         {1, {key, 0}, len}
#define DOUBLE_PITCH_NOTE(key1, key2, len)  {2, {key1, key2}, len}
#define REST_NOTE(len)                      {0, {0, 0}, len}

#endif //__MUSIC_DATA_H__
