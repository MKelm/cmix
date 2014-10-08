#include <stdio.h>
#include <string.h>

/* Es gibt 3 Stäbe mit min. 3 und max. 8 Scheiben. */
int scheiben[3][8], anz_scheiben;
int quell_scheiben_pos, ziel_scheiben_pos;

void abfrage_scheiben_anzahl();
void einrichtung_scheiben();

int abfrage_stab_bewegung(int modus);
int pruefung_stab_wahl(int modus, int stab_wahl);
int pruefung_scheiben_groessen(int stab_quelle, int stab_ziel);

void anzeige_scheiben();

int main() {
  printf("Die Türme von Hanoi (C-Spiel)\n");

  abfrage_scheiben_anzahl();
  einrichtung_scheiben();

  int bewegung_gueltig = 0, stab_quelle, stab_ziel;
  do {
    anzeige_scheiben();
    do {
      stab_quelle = abfrage_stab_bewegung(0);
      stab_ziel = abfrage_stab_bewegung(1);
      bewegung_gueltig = pruefung_scheiben_groessen(stab_quelle, stab_ziel);
    } while (bewegung_gueltig == 0);

    scheiben[stab_ziel][ziel_scheiben_pos - 1] = scheiben[stab_quelle][quell_scheiben_pos];
    scheiben[stab_quelle][quell_scheiben_pos] = -1;
  } while (1 == 1);

  return 0;
}

void abfrage_scheiben_anzahl() {
  anz_scheiben = 8;
  printf("Wie viele Scheiben möchten sie einsetzen? (3-8)\n");
  scanf("%d", &anz_scheiben);

  if (anz_scheiben < 3 || anz_scheiben > 8)
    abfrage_scheiben_anzahl();
}

void einrichtung_scheiben() {
  /* scheiben auf stab 1 und restliche positionen einrichten */
  /* positionierung größte scheibe unten */
  int stab, scheibe, scheiben_nr;

  for (stab = 0; stab < 3; stab++) {
    scheiben_nr = 0;
    for (scheibe = 0; scheibe < 8; scheibe++) {
      if (stab > 0 || scheibe + 1 > anz_scheiben) {
        scheiben[stab][scheibe] = -1;
      } else {
        scheiben[stab][scheibe] = scheiben_nr;
        scheiben_nr++;
      }
    }
  }
}

int abfrage_stab_bewegung(int modus) {
  int stab_wahl = -1;
  if (modus == 0)
    printf("Von welchem Stab soll eine Scheibe genommen werden? (1-3)\n");
  else
    printf("Auf welchem Stab soll eine Scheibe platziert werden? (1-3)\n");
  scanf("%d", &stab_wahl);

  if (stab_wahl > 0 || stab_wahl < 4)
    stab_wahl = pruefung_stab_wahl(modus, stab_wahl);

  if (stab_wahl < 1 || stab_wahl > 3) {
    printf("Fehlerhafte Stabauswahl!\n");
    abfrage_stab_bewegung(modus);
  }

  return stab_wahl - 1;
}

int pruefung_stab_wahl(int modus, int stab_wahl) {
  /* Überprüfung ob Stab Scheiben hat oder leer ist */
  int scheibe, gueltig = 0;
  if (modus == 0) { /* modus für scheibe nehmen */
    // stab muss scheiben enthalten für auswahl
    for (scheibe = 0; scheibe < anz_scheiben; scheibe++) {
      if (scheiben[stab_wahl-1][scheibe] > -1) {
        gueltig = 1;
      }
    }
  } else { /* modus für scheibe platzieren */
    // stab muss freie plätze enthalten für auswahl
    for (scheibe = 0; scheibe < anz_scheiben; scheibe++) {
      if (scheiben[stab_wahl-1][scheibe] == -1) {
        gueltig = 1;
      }
    }
  }
  if (gueltig == 0)
    stab_wahl = -1;
  return stab_wahl;
}

int pruefung_scheiben_groessen(int stab_quelle, int stab_ziel) {
  // suche nach oberster scheibe in stab quelle
  int scheibe = 0;
  quell_scheiben_pos = -1;
  ziel_scheiben_pos = -1;
  do {
    if (scheiben[stab_quelle][scheibe] != -1) {
      quell_scheiben_pos = scheibe;
    }
    scheibe++;
  } while (quell_scheiben_pos == -1 && scheibe < anz_scheiben);
  if (quell_scheiben_pos == -1) {
    printf("Quellscheibenposition fehlerhaft!\n");
    return 0; // stab komplett frei, keine scheibe zur auswahl
  }

  // suche nach oberster scheibe in ziel quelle
  scheibe = 0;
  do {
    if (scheiben[stab_ziel][scheibe] != -1) {
      ziel_scheiben_pos = scheibe;
    }
    scheibe++;
  } while (ziel_scheiben_pos == -1 && scheibe < anz_scheiben);

  if (ziel_scheiben_pos == -1) {
    ziel_scheiben_pos = anz_scheiben;
    return 1; // stab komplett frei, zug ist möglich
  } else {
    /* stab belegt, scheiben größenvergleich wichtig */
    int quell_groesse = scheiben[stab_quelle][quell_scheiben_pos],
        ziel_groesse = scheiben[stab_ziel][ziel_scheiben_pos];
    if (ziel_groesse > quell_groesse) {
      return 1; // ziel scheibe ist größer als quell scheibe, gueltiger zug
    } else {
      printf("Zielscheibenposition fehlerhaft!\n");
      return 0; // ziel scheibe ist kleiner als quell scheibe, ungueltiger zug
    }
  }

  return 1;
}

void anzeige_scheiben() {
  int stab, scheibe, i, rand, breite;
  for (scheibe = 0; scheibe < anz_scheiben; scheibe++) {
    for (stab = 0; stab < 3; stab++) {
      if (scheiben[stab][scheibe] == -1) {
        /* leere scheiben position */
        rand = (anz_scheiben * 2) / 2;
        for (i = 0; i < rand; i++)
          printf(" ");
        printf("|");
        for (i = 0; i < rand; i++)
          printf(" ");
      } else {
        /* gefüllte scheiben position */
        breite = scheiben[stab][scheibe] * 2 + 1;
        rand = ((anz_scheiben * 2) / 2) - ((breite - 1) / 2);
        for (i = 0; i < rand; i++)
          printf(" ");
        for (i = 0; i < breite; i++)
          printf("#");
        for (i = 0; i < rand; i++)
          printf(" ");
      }
    }
    printf("\n");
  }

  printf("\n");
}