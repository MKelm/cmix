/* Geburtstagslistenprogramm */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

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

void listendatei_laden(struct eintrag liste[], int *zll);
void listendatei_speichern(struct eintrag liste[], int ll);

int ngbzeit_berechnen(struct eintrag aeintrag);

int abfrage_menu(void);
void flusheingabe(void);

void abfrage_eintraege_hinzufuegen(int *zlla, int amax);

void eintraege_loeschen(struct eintrag liste[], int *zll);
void sortierung_liste(struct eintrag liste[], int ll);
void ausgabe_liste(struct eintrag liste[], int ll);

void meldung_naechster_gb(struct eintrag liste[], int ll);

int main(int argc, char *argv[]) {
  struct eintrag liste[MAX];
  int i, ll, lla, wahl;

  check_benutzer_verzeichnis();

  if (argc > 2) {
    if (strcmp(argv[1], "ngb") == 0 && atoi(argv[2]) > 0) {
      while (1) {
        listendatei_laden(liste, &ll);
        meldung_naechster_gb(liste, ll);
        fflush(stdout);
        listendatei_speichern(liste, ll);
        sleep(atoi(argv[2]));
      }
      exit(EXIT_SUCCESS);
    }
  }

  listendatei_laden(liste, &ll);
  while (1) {
    wahl = abfrage_menu();

    if (wahl == 1) {
      abfrage_eintraege_hinzufuegen(&lla, MAX-ll);

      if (lla > 0 && ll + lla <= MAX) {
        /* Eingabe/Hinzufügen von Einträgen */
        for (i = ll; i < ll + lla; i++) {
          printf("\nEintrag Nr. %d", i + 1);
          printf("\nBitte Vornamen eingeben: ");
          scanf("%s", liste[i].vname);
          flusheingabe();
          printf("Bitte Nachnamen eingeben: ");
          scanf("%s", liste[i].nname);
          flusheingabe();
          printf("Bitte Geburtstag eingeben: (TT MM JJJJ) ");
          scanf("%2hd %2hd %4hd",
                &liste[i].sdatum.tag, &liste[i].sdatum.monat, &liste[i].sdatum.jahr);
          flusheingabe();
          liste[i].meldungszeit = -1;
        }
        ll = ll + lla;
        sortierung_liste(liste, ll);
      }
    } else if (wahl == 2) {
      eintraege_loeschen(liste, &ll);
      sortierung_liste(liste, ll);
    } else if (wahl == 3) {
      ausgabe_liste(liste, ll);
    } else if (wahl == 4) {
      listendatei_speichern(liste, ll);
      exit(EXIT_SUCCESS);
    }
  }

  return 0;
}

void check_benutzer_verzeichnis(void) {
  char tverz[1024];

  snprintf(
    tverz, sizeof(tverz), "%s/%s", getenv("HOME"), ".cgblist"
  );

  benutzer_verzeichnis = tverz;
  mkdir(benutzer_verzeichnis, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  snprintf(
    datei_daten, sizeof(datei_daten), "%s/%s", benutzer_verzeichnis, "daten.txt"
  );
}

void listendatei_laden(struct eintrag liste[], int *zll) {
  FILE *fp;
  if ((fp = fopen(datei_daten, "r")) == NULL) {
    if ((fp = fopen(datei_daten, "w+")) == NULL) {
      fprintf(stderr, "Fehler beim Einlesen der Listendaten.\n");
      exit(EXIT_FAILURE);
    }
  }
  int i = 0;
  while(1) {
    fscanf(fp, "%s %s %hd %hd %hd %d\n",
      liste[i].vname, liste[i].nname,
      &liste[i].sdatum.tag, &liste[i].sdatum.monat, &liste[i].sdatum.jahr,
      &liste[i].meldungszeit
    );
    liste[i].ngbzeit = ngbzeit_berechnen(liste[i]);
    if (feof(fp)) {
      if (strlen(liste[i].vname) > 0 && strlen(liste[i].nname) > 0)
        *zll = i+1;
      break;
    }
    i++;
  }
  fclose(fp);
}

void listendatei_speichern(struct eintrag liste[], int ll) {
  FILE *fp;
  if ((fp = fopen(datei_daten, "w+")) == NULL) {
    fprintf(stderr, "Fehler beim Abspeichern der Listendaten.\n");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  while (i < ll) {
    fprintf(fp, "%s %s %hd %hd %hd %d\n",
      liste[i].vname, liste[i].nname,
      liste[i].sdatum.tag, liste[i].sdatum.monat, liste[i].sdatum.jahr,
      liste[i].meldungszeit
    );
    i++;
  }
  fclose(fp);
}

int ngbzeit_berechnen(struct eintrag aeintrag) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int naechstes_jahr = 0;
    if (aeintrag.sdatum.monat - 1 < tm.tm_mon ||
        (aeintrag.sdatum.monat - 1 == tm.tm_mon &&
         aeintrag.sdatum.tag < tm.tm_mday)) {
      naechstes_jahr = 1;
    }

    struct tm ngbtm;
    ngbtm.tm_year = tm.tm_year + naechstes_jahr;
    ngbtm.tm_mon = aeintrag.sdatum.monat - 1;
    ngbtm.tm_mday = aeintrag.sdatum.tag;
    ngbtm.tm_hour = 0;
    ngbtm.tm_min = 0;
    ngbtm.tm_sec = 1;
    ngbtm.tm_isdst = -1;
    return mktime(&ngbtm);
}

int abfrage_menu(void) {
  printf("\nGeburtstagsliste Menü\n");
  printf("\n1 = Einträge hinzufügen");
  printf("\n2 = Einträge entfernen");
  printf("\n3 = Einträge anzeigen");
  printf("\n4 = Programm beenden");
  printf("\n");
  char ch = getchar();
  flusheingabe();
  return atoi(&ch);
}

void flusheingabe(void) {
  /* löscht alle restlichen zeichen in eingabe stream bei nutzung von scanf */
  char muell[80];
  fgets(muell,80,stdin);
}

void abfrage_eintraege_hinzufuegen(int *zlla, int amax) {
  printf("Wie viele Einträge möchten sie hinzufügen? (max. %d) ", amax);
  scanf("%d", zlla);
  flusheingabe();

  if (*zlla > amax || *zlla < 0)
    abfrage_eintraege_hinzufuegen(zlla, amax);
}

void eintraege_loeschen(struct eintrag liste[], int *zll) {
  char vname[50], nname[50];
  printf("Welchen Eintrag möchten sie löschen? (Vorname Nachnahme) ");
  scanf("%s %s", vname, nname);
  flusheingabe();

  int i, aenderung = 0;
  for (i = 0; i < *zll; i++) {
    if (strcasecmp(liste[i].vname, vname) == 0 &&
        strcasecmp(liste[i].nname, nname) == 0) {
      aenderung = 1;
    } else if (aenderung == 1) {
      strcpy(liste[i-1].vname, liste[i].vname);
      strcpy(liste[i-1].nname, liste[i].nname);
      liste[i-1].sdatum.tag = liste[i].sdatum.tag;
      liste[i-1].sdatum.monat = liste[i].sdatum.monat;
      liste[i-1].sdatum.jahr = liste[i].sdatum.jahr;
      liste[i-1].meldungszeit = liste[i].meldungszeit;
    }
  }
  if (aenderung == 1) {
    *zll = *zll - 1;
  }
  char cont[10];
  printf("Einen weiteren Eintrag löschen? (j/n) ");
  scanf("%s", cont);
  flusheingabe();
  if (strcasecmp(cont, "j") == 0) {
    eintraege_loeschen(liste, zll);
  }
}

void sortierung_liste(struct eintrag liste[], int ll) {
  /* sortiert liste nach monat / tag für ausgabe absteigend */
  int i, aenderung = 0;
  struct eintrag tmp;
  for (i = 0; i < ll - 1; i++) {
    if (liste[i].sdatum.monat > liste[i+1].sdatum.monat ||
        (liste[i].sdatum.monat == liste[i+1].sdatum.monat &&
         liste[i].sdatum.tag > liste[i+1].sdatum.tag)
       ) {
      aenderung = 1;

      strcpy(tmp.vname, liste[i].vname);
      strcpy(tmp.nname, liste[i].nname);
      tmp.sdatum.tag = liste[i].sdatum.tag;
      tmp.sdatum.monat = liste[i].sdatum.monat;
      tmp.sdatum.jahr = liste[i].sdatum.jahr;
      tmp.meldungszeit = liste[i].meldungszeit;

      strcpy(liste[i].vname, liste[i+1].vname);
      strcpy(liste[i].nname, liste[i+1].nname);
      liste[i].sdatum.tag = liste[i+1].sdatum.tag;
      liste[i].sdatum.monat = liste[i+1].sdatum.monat;
      liste[i].sdatum.jahr = liste[i+1].sdatum.jahr;
      liste[i].meldungszeit = liste[i+1].meldungszeit;

      strcpy(liste[i+1].vname, tmp.vname);
      strcpy(liste[i+1].nname, tmp.nname);
      liste[i+1].sdatum.tag = tmp.sdatum.tag;
      liste[i+1].sdatum.monat = tmp.sdatum.monat;
      liste[i+1].sdatum.jahr = tmp.sdatum.jahr;
      liste[i+1].meldungszeit = tmp.meldungszeit;
    }
  }
  if (aenderung == 1) sortierung_liste(liste, ll);
}

void ausgabe_liste(struct eintrag liste[], int ll) {
  int i;
  for (i = 0; i < ll; i++) {
    printf(
      "\nName: %s %s, Geburtstag: %d.%d.%d",
      liste[i].vname, liste[i].nname,
      liste[i].sdatum.tag, liste[i].sdatum.monat, liste[i].sdatum.jahr
    );
  }
  printf("\n");
}

void meldung_naechster_gb(struct eintrag liste[], int ll) {
  sortierung_liste(liste, ll);

  char buf[256];
  int i;
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  int w2pre, w1pre, d3pre, d1pre;

  for (i = 0; i < ll; i++) {
    w2pre = liste[i].ngbzeit - (2 * 7 * 24 * 60 * 60);
    w1pre = liste[i].ngbzeit - (1 * 7 * 24 * 60 * 60);
    d3pre = liste[i].ngbzeit - (3 * 24 * 60 * 60);
    d1pre = liste[i].ngbzeit - (1 * 24 * 60 * 60);

    if ((t > w2pre && t < w1pre && liste[i].meldungszeit < w2pre) ||
        (t > w1pre && t < d3pre && liste[i].meldungszeit < w1pre) ||
        (t > d3pre && t < d1pre && liste[i].meldungszeit < d3pre) ||
        (t > d1pre && t < liste[i].ngbzeit && liste[i].meldungszeit < d1pre)) {

      liste[i].meldungszeit = t;
      snprintf(
        buf, sizeof(buf),
        "%s \"Geburtstag\" \"Am %hd.%hd. wird %s %s %d Jahre alt!\"",
        "notify-send -i /usr/share/icons/gnome/48x48/status/appointment-soon.png",
        liste[i].sdatum.tag, liste[i].sdatum.monat,
        liste[i].vname, liste[i].nname,
        tm->tm_year + 1900 - liste[i].sdatum.jahr
      );
      system(buf);
    }
  }
}