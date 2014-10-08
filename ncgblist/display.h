/* display.h: Header-Datei für display.c. */

void anzeige_init(void);
void setze_listenlaenge(void);
void fenster_einrichten(void);

void menue_anzeigen(void);
void eingabe_hinzuf_anzeigen(void);
void eingabe_loesch_anzeigen(void);

void hilfe_anzeigen(int textid);
void liste_anzeigen(void);

int eingabe_abfrage(void);
int anzeige_ende(void);

/* Ende von display.h */