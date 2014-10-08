/* display.h: Header-Datei für ncgblist.c. */

#define MAX 1000

struct eintrag {
  char vname[50];
  char nname[50];
  struct eintragdatum {
    short tag;
    short monat;
    short jahr;
  } sdatum;
  int ngbzeit; // nächster geburtstagstermin
  int meldungszeit;
};

char *benutzer_verzeichnis;
char datei_daten[1024];

void check_benutzer_verzeichnis(void);

void listendatei_laden(void);
void listendatei_speichern(void);

int ngbzeit_berechnen(struct eintrag aeintrag);

void eintrag_hinzufuegen(void);
void eintrag_loeschen(void);
void sortierung_liste(void);

void meldung_naechster_gb(void);

/* Ende von ncgblist.h */