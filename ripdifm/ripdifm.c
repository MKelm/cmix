#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jsmn/jsmn.h"

#define MAX_JSON_LINE_CHARS 1024

char tmp_publist[256] = "/tmp/ripdifm_public_list";
char tmp_playlist[256] = "/tmp/ripdifm_playlist";

#define MAX_PLAYLIST_ITEMS 100

struct playlist_item {
  char key[256];
  char playlist[512];
} playlist_items[MAX_PLAYLIST_ITEMS];

int playlist_items_count = 0;

char stream_url[256];

void load_json_token(char* input, char *token, jsmntok_t *tokens, int token_idx) {
  strncpy(token, "", MAX_JSON_LINE_CHARS);
  strncpy(token, input + tokens[token_idx].start,
    tokens[token_idx].end - tokens[token_idx].start);
}

void curl_public_list(void) {
  char call[1024];
  snprintf(
    call, sizeof(call),
    "curl %s -o %s",
    "http://listen.di.fm/public3",
    tmp_publist
  );
  system(call);
}

void set_public_list_by_tokens(char *output, jsmntok_t *tokens) {
  int i = 0, j, j_max, k, k_max;
  playlist_items_count = 0;
  char line[MAX_JSON_LINE_CHARS];
  if (tokens[i].type == JSMN_ARRAY) {
    // iterate through stream list parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (tokens[i].type == JSMN_OBJECT) {
        // iterate through stream object parts
        k_max = tokens[i].size;
        for (k = 0; k < k_max; k++) {
          i++;
          if (k % 2 == 0 && tokens[i].type == JSMN_STRING &&
              playlist_items_count < MAX_PLAYLIST_ITEMS) {
            load_json_token(output, line, tokens, i);
            // get meta object part content by key
            if (strcmp(line, "key") == 0) {
              load_json_token(output, line, tokens, i+1);
              printf("%s ", line);
              strcpy(playlist_items[playlist_items_count].key, line);
            } else if (strcmp(line, "playlist") == 0) {
              load_json_token(output, line, tokens, i+1);
              strcpy(playlist_items[playlist_items_count].playlist, line);
              playlist_items_count++;
            }
          }
        }
      }
    }
  }
  printf("\n--- set %d playlist items to list\n", playlist_items_count);
}

void load_public_list(void) {
  FILE *fp = fopen(tmp_publist, "r");
  fseek(fp, 0L, SEEK_END);
  int f_size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  int ch;
  char output[f_size], ch_str[2];
  while ((ch = fgetc(fp)) != EOF) {
    snprintf(ch_str, 2, "%c", ch);
    strcat(output, ch_str);
  }
  fclose(fp);

  int r;
  jsmn_parser p;
  jsmntok_t tokens[f_size];
  jsmn_init(&p);
  r = jsmn_parse(&p, output, strlen(output), tokens, f_size);
  if (r > 0) {
    printf("--- loaded %d tokens of %d chars\n", r, f_size);
  }

  set_public_list_by_tokens(output, tokens);
}

void load_playlist(void) {
  FILE *fp = fopen(tmp_playlist, "r");

  char *ptr, chunk[1024];
  while (fgets(chunk, 1024, fp) != NULL) {
    chunk[strlen(chunk)-1] = '\0';

    ptr = strtok(chunk, "=");
    while (ptr != NULL) {
      if (strcmp(ptr, "File1") == 0) {
        ptr = strtok(NULL, "=");
        strncpy(stream_url, ptr, 256);
        printf("--- found stream url: %s\n", stream_url);
      }
      ptr = strtok(NULL, "=");
    }
  }
  fclose(fp);
}

void curl_playlist(char *pl_key) {
  int i, key_found;
  char pl_url[512];
  for (i = 0; i < playlist_items_count; i++) {
    if (strcmp(playlist_items[i].key, pl_key) == 0) {
      strcpy(pl_url, playlist_items[i].playlist);
      key_found = 1;
    }
  }
  if (key_found == 1) {
    char call[1024];
    snprintf(
      call, sizeof(call),
      "curl %s -o %s",
      pl_url,
      tmp_playlist
    );
    system(call);
  } else {
    printf("-- invalid key parameter\n");
  }
}

void start_streamrip(char *dest_directory) {
  char call[1024];
  snprintf(
    call, sizeof(call),
    "streamripper %s -d %s",
    stream_url, dest_directory
  );
  system(call);
}

int main(int argc, char *argv[]) {
  curl_public_list();
  load_public_list();

  if (argc > 1 && strlen(argv[1]) > 0) {
    curl_playlist(argv[1]);
    load_playlist();

    if (strlen(stream_url) > 0 && argc > 2 && strlen(argv[2]) > 0) {
      start_streamrip(argv[2]);
    } else {
      printf("--- please enter a strem output folder as second parameter\n");
    }

  } else {
    printf("--- please select a playlist item from list as first parameter\n");
  }

  return 0;
}