/* Geburtstagslistenprogramm Ausgabe */
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "display.h"
#include "ncgblist.h"

extern struct eintrag aenderungs_eintrag;
extern struct eintrag liste[MAX];
extern int ll;

int maxy, maxx, halfy, halfx;
WINDOW *eingabe, *hilfe, *gbliste;
WINDOW *seingabe, *shilfe, *sgbliste;

/* akt_kontext: 0 = menue, 1 = liste */
/* liste_offset = anzeige pos oben */
int akt_kontext, menue_position, liste_position, liste_offset, liste_laenge, liste_lsichtbar;

/* ll == listen länge */
void anzeige_init() {
  setlocale(LC_ALL, "");
  initscr();
  getmaxyx(stdscr, maxy, maxx);
  halfx = maxx >> 1;
  halfy = maxy >> 1;
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  curs_set(0);
  keypad(stdscr, TRUE);

  akt_kontext = 0; // menue
  menue_position = 0;
  liste_position = 0;
  liste_offset = 0; // offset der liste mit max. xx einträgen (sichtbar)
  setze_listenlaenge();
}

void setze_listenlaenge(void) {
  liste_laenge = ll;
  liste_lsichtbar = (maxy-2) / 2;
  if (liste_lsichtbar > liste_laenge)
    liste_lsichtbar = liste_laenge;
  if (liste_position > liste_lsichtbar-1)
    liste_position = liste_lsichtbar-1;
}

void menue_anzeigen(void) {
  int c;
  char menue_eintraege[3][25] = {
    "* Eintrag hinzufügen",
    "* Eintrag löschen",
    "* Beenden"
  };

  wclear(seingabe);
  for (c = 0; c < 3; c++) {
    if (akt_kontext == 0 && c == menue_position)
      wattron(seingabe, A_REVERSE);
    mvwaddstr(seingabe, 2+(c*2), 2, menue_eintraege[c]);
    wattroff(seingabe, A_REVERSE);
  }
  wrefresh(seingabe);
}

void eingabe_hinzuf_anzeigen(void) {
  char vname[25], nname[25], tag[3], monat[3], jahr[5];
  curs_set(1);

  wclear(seingabe);
  mvwaddstr(seingabe, 2, 2, "* Eintrag hinzufügen");
  mvwaddstr(seingabe, 2+2, 2, "+ Vorname: ");
  wrefresh(seingabe);
  wgetnstr(seingabe, vname, 24);
  mvwaddstr(seingabe, 2+3, 2, "+ Nachname: ");
  wrefresh(seingabe);
  wgetnstr(seingabe, nname, 24);

  mvwaddstr(seingabe, 2+4, 2, "+ Datum:   -  -    ");
  mvwgetnstr(seingabe, 2+4, 11, tag, 2);
  mvwgetnstr(seingabe, 2+4, 14, monat, 2);
  mvwgetnstr(seingabe, 2+4, 17, jahr, 4);

  if (strlen(vname) > 0 && strlen(nname) > 0 &&
      strlen(tag) > 0 && strlen(monat) > 0 && strlen(jahr) > 0) {
    strcpy(aenderungs_eintrag.vname, vname);
    strcpy(aenderungs_eintrag.nname, nname);
    aenderungs_eintrag.sdatum.tag = atoi(tag);
    aenderungs_eintrag.sdatum.monat = atoi(monat);
    aenderungs_eintrag.sdatum.jahr = atoi(jahr);
  }

  curs_set(0);
}

void eingabe_loesch_anzeigen(void) {
  char vname[25], nname[25], bestaetigung[2];
  curs_set(1);

  wclear(seingabe);
  mvwaddstr(seingabe, 2, 2, "* Eintrag löschen");
  mvwaddstr(seingabe, 2+2, 2, "- Vorname: ");
  wrefresh(seingabe);
  wgetnstr(seingabe, vname, 24);
  mvwaddstr(seingabe, 2+3, 2, "- Nachname: ");
  wrefresh(seingabe);
  wgetnstr(seingabe, nname, 24);

  mvwaddstr(seingabe, 2+4, 2, "- Wirklich löschen? ");
  wrefresh(seingabe);
  wgetnstr(seingabe, bestaetigung, 1);

  if (strcasecmp("j", bestaetigung) == 0) {
    snprintf(
      aenderungs_eintrag.vname, sizeof(aenderungs_eintrag.vname), "%s", vname
    );
    snprintf(
      aenderungs_eintrag.nname, sizeof(aenderungs_eintrag.nname), "%s", nname
    );
  }

  curs_set(0);
}

void hilfe_anzeigen(int textid) {
  char hilfe_texte[3][129] = {
    "Navigieren mit den Pfeiltasten hoch/runter. Mit rechts/links zwischen Menü und Liste wechseln.",
    "Geben Sie einen Vornamen, Nachnamen (max. 24 Zeichen) und ein Geburtsdatum in Form Tag-Monat-Jahr an.",
    "Geben Sie einen Vornamen, Nachnamen (max. 24 Zeichen) an. Bestätigen Sie mit (j)a oder (n)ein."
  };

  wclear(shilfe);
  mvwaddstr(shilfe, 0, 0, hilfe_texte[textid]);
  wrefresh(shilfe);
}

void liste_anzeigen(void) {
  int c;
  char text[256];

  wclear(sgbliste);
  for (c = liste_offset; c < liste_offset + liste_lsichtbar; c++) {
    if (akt_kontext == 1 && c - liste_offset == liste_position)
      wattron(sgbliste, A_REVERSE);

    snprintf(
      text, sizeof(text),
      "* %s %s, %d.%d.%d",
      liste[c].vname, liste[c].nname,
      liste[c].sdatum.tag, liste[c].sdatum.monat, liste[c].sdatum.jahr
    );
    mvwaddstr(sgbliste, 1 + ((c - liste_offset) * 2), 1, text);
    wattroff(sgbliste, A_REVERSE);
  }
  if (c == 0)
    mvwaddstr(sgbliste, 1 + (0 * 2), 1, "Keine Einträge vorhanden ...");

  wrefresh(sgbliste);
}

void fenster_einrichten(void) {
  eingabe = newwin(halfy, halfx, 0, 0);
  seingabe = subwin(eingabe, halfy-2, halfx-2, 1, 1);
  hilfe = newwin(halfy, halfx, halfy, 0);
  shilfe = subwin(hilfe, halfy-5, halfx-6, halfy+3, 3);
  gbliste = newwin(maxy, halfx, 0, halfx);
  sgbliste = subwin(gbliste, maxy-2, halfx-2, 1, halfx+1);
  refresh();

  wbkgd(eingabe, COLOR_PAIR(1));
  wbkgd(seingabe, COLOR_PAIR(1));
  box(eingabe, 0, 0);
  wattron(eingabe, A_BOLD);
  mvwaddstr(eingabe, 0, 2, "Menü");
  wattroff(eingabe, A_BOLD);
  wrefresh(eingabe);

  wbkgd(hilfe, COLOR_PAIR(1));
  wbkgd(shilfe, COLOR_PAIR(1));
  box(hilfe, 0, 0);
  wattron(hilfe, A_BOLD);
  mvwaddstr(hilfe, 0, 2, "Hilfe");
  wattroff(hilfe, A_BOLD);
  wrefresh(hilfe);

  wbkgd(gbliste, COLOR_PAIR(1));
  wbkgd(sgbliste, COLOR_PAIR(1));
  box(gbliste, 0, 0);
  wattron(gbliste, A_BOLD);
  mvwaddstr(gbliste, 0, 2, "Liste");
  wattroff(gbliste, A_BOLD);
  scrollok(sgbliste, TRUE);
  wrefresh(gbliste);
}

int eingabe_abfrage(void) {
  int ch = getch();
  switch (ch) {
    case KEY_RESIZE:
      return -4;
    case KEY_DOWN:
      if (akt_kontext == 0 && menue_position < 2) {
        menue_position++;
        menue_anzeigen();
      } else if (akt_kontext == 1) {
        if (liste_position < liste_lsichtbar-1) {
          liste_position++;
          liste_anzeigen();
        } else if (liste_laenge > liste_lsichtbar &&
                   liste_offset + liste_lsichtbar < liste_laenge) {
          liste_offset++;
          liste_anzeigen();
        }
      }
      return -1;
      break;
    case KEY_UP:
      if (akt_kontext == 0 && menue_position > 0) {
        menue_position--;
        menue_anzeigen();
      } else if (akt_kontext == 1) {
        if (liste_position > 0) {
          liste_position--;
          liste_anzeigen();
        } else if (liste_offset > 0) {
          liste_offset--;
          liste_anzeigen();
        }
      }
      return -1;
      break;
    case KEY_LEFT:
      if (akt_kontext == 1) {
        akt_kontext = 0;
        menue_anzeigen();
        liste_anzeigen();
      }
      return -1;
      break;
    case KEY_RIGHT:
      if (akt_kontext == 0 && liste_laenge > 0) {
        akt_kontext = 1;
        menue_anzeigen();
        liste_anzeigen();
      }
      return -1;
      break;
    case '\n':
      if (akt_kontext == 0) {
        switch (menue_position) {
          case 0:
            // eintrag hinzufügen
            hilfe_anzeigen(1);
            eingabe_hinzuf_anzeigen();
            hilfe_anzeigen(0);
            menue_anzeigen();
            //liste_anzeigen();
            return -2;
            break;
          case 1:
            // eintrag löschen
            hilfe_anzeigen(2);
            eingabe_loesch_anzeigen();
            hilfe_anzeigen(0);
            menue_anzeigen();
            //liste_anzeigen();
            return -3;
            break;
          case 2:
            // beenden
            return 0;
            break;
        }
      }
    default:
      return -1;
  }
  return 0;
}

int anzeige_ende(void) {
  curs_set(1);
  endwin();
  return 0;
}