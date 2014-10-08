/* Geburtstagslistenprogramm */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "ncgblist.h"
#include "display.h"

struct eintrag aenderungs_eintrag;
struct eintrag liste[MAX];
int ll; // aktuelle listenlänge

int main(int argc, char *argv[]) {
  check_benutzer_verzeichnis();

  // dienst für meldungen
  if (argc > 2) {
    if (strcmp(argv[1], "ngb") == 0 && atoi(argv[2]) > 0) {
      while (1) {
        listendatei_laden();
        meldung_naechster_gb();
        fflush(stdout);
        listendatei_speichern();
        sleep(atoi(argv[2]));
      }
      exit(EXIT_SUCCESS);
    }
  }

  // verwaltung
  listendatei_laden();

  anzeige_init();
  fenster_einrichten();
  menue_anzeigen();
  hilfe_anzeigen(0);
  liste_anzeigen();

  int ret;
  do {
    ret = eingabe_abfrage();
    if (ret == -2) {
      // hinzufügen
      eintrag_hinzufuegen();
    } else if (ret == -3) {
      // löschen
      eintrag_loeschen();
    } else if (ret == -4) {
      // resize
      anzeige_init();
      fenster_einrichten();
      menue_anzeigen();
      hilfe_anzeigen(0);
      liste_anzeigen();
    }
  } while (ret != 0);

  listendatei_speichern();
  return anzeige_ende();
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

void listendatei_laden(void) {
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
        ll = i+1;
      break;
    }
    i++;
  }
  fclose(fp);
}

void listendatei_speichern(void) {
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

void eintrag_hinzufuegen(void) {

  if (strlen(aenderungs_eintrag.vname) > 0 &&
      strlen(aenderungs_eintrag.nname) > 0 &&
      aenderungs_eintrag.sdatum.tag > 0 &&
      aenderungs_eintrag.sdatum.monat > 0 &&
      aenderungs_eintrag.sdatum.jahr > 0) {

    if (ll + 1 < MAX) {
      strcpy(liste[ll].vname, aenderungs_eintrag.vname);
      strcpy(liste[ll].nname, aenderungs_eintrag.nname);
      liste[ll].sdatum.tag = aenderungs_eintrag.sdatum.tag;
      liste[ll].sdatum.monat = aenderungs_eintrag.sdatum.monat;
      liste[ll].sdatum.jahr = aenderungs_eintrag.sdatum.jahr;
      liste[ll].meldungszeit = -1;
      ll++;

      sortierung_liste();
      strcpy(aenderungs_eintrag.vname, "");
      strcpy(aenderungs_eintrag.nname, "");
      aenderungs_eintrag.sdatum.tag = 0;
      aenderungs_eintrag.sdatum.monat = 0;
      aenderungs_eintrag.sdatum.jahr = 0;
      setze_listenlaenge();
      liste_anzeigen();
    }
  }
}

void eintrag_loeschen(void) {

  if (strlen(aenderungs_eintrag.vname) > 0 &&
      strlen(aenderungs_eintrag.nname) > 0) {

    int i, aenderung = 0;
    for (i = 0; i < ll; i++) {
      if (strcasecmp(liste[i].vname, aenderungs_eintrag.vname) == 0 &&
          strcasecmp(liste[i].nname, aenderungs_eintrag.nname) == 0) {
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
      ll--;
      sortierung_liste();
      strcpy(aenderungs_eintrag.vname, "");
      strcpy(aenderungs_eintrag.nname, "");
      setze_listenlaenge();
      liste_anzeigen();
    }
  }
}

void sortierung_liste(void) {
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
  if (aenderung == 1) sortierung_liste();
}

void meldung_naechster_gb(void) {
  sortierung_liste();

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