void display_init(void);
void display_set_list_length(void);
void display_init_windows(void);

void display_menu(void);
void display_input_add(void);
void display_input_del(void);

char *dsp_word_wrap(char* buffer, char* string, int line_width);
void display_help(int text_id);
void display_list(void);

int diplay_input(void);
int display_end(void);