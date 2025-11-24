#include <stdio.h>
#include <stdlib.h>

// Funktion prototyper
void generer_noegler();
void krypter_besked();
void dekrypter_besked();

int main() {
    int valg;

    // Vis velkomstbesked
    printf("Velkommen til Gruppe 10 P1 krypteringsprogrammet\n");

    while (1) {
        // Vis menu
        // 1. Krypter besked
        // 2. Dekrypter besked
        // 3. Generer nøglepar
        // 4. Afslut
        printf("\nMenu:\n");
        printf("1. Krypter besked\n");
        printf("2. Dekrypter besked\n");
        printf("3. Generer noeglepar\n");
        printf("4. Afslut\n");
        printf("Indtast dit valg: ");

        // Læs brugerens valg
        if (scanf("%d", &valg) != 1) {
            // Håndter ugyldigt input (f.eks. bogstaver)
            while (getchar() != '\n'); // Ryd input buffer
            valg = 0; // Sæt til ugyldig værdi
        }

        // Hvad valgte brugeren?
        switch (valg) {
            case 1:
                // Krypter
                krypter_besked();
                break;
            case 2:
                // Dekrypter
                dekrypter_besked();
                break;
            case 3:
                // Generer nøglepar
                generer_noegler();
                break;
            case 4:
                // Afslut
                printf("Afslutter programmet...\n");
                return 0;
            default:
                // Ugyldigt valg
                printf("Ugyldigt valg. Proev igen.\n");
                // Loop programmet tilbage for at lave et nyt valg
                break;
        }
    }
    return 0;
}

void generer_noegler() {
    printf("\n--- Starter noeglegenerering ---\n");

    // Generer seed til pseudotilfældighedsgenerator
    // (Her skal der implementeres kode til at seede random funktionen)

    // Find to store primtal p og q via pseudotilfældighedsgenerator
    // (Her skal der være logik til at finde primtal)

    // Beregn n = p * q

    // Beregn phi(n) = (p-1) * (q-1)

    // Vælg offentlig eksponent e
    // (e skal være indbyrdes primisk med phi(n))

    // Beregn privat eksponent d
    // d = e^-1 mod phi(n)

    // Dan nøglepar:
    // Offentlig nøgle: (e, n)
    // Privat nøgle: (d, n)

    // Eksporter nøglepar (e, d, n) til nøglefil (.txt)
    // (Skriv til fil)

    printf("Noeglegenerering afsluttet. Retur til hovedmenu.\n");
}

void krypter_besked() {
    printf("\n--- Starter krypteringsflow ---\n");

    // Indtast besked klartekst (fra tastatur eller fil)
    // (Læs input)
    // therkel og jeg ve dikke hvad vi laver

    // Brug eksisterende nøglepar?
    // (Spørg brugeren)
    
    // Hvis Ja:
        // Indlæs nøglepar fra nøglefil
    
    // Hvis Nej:
        // Generer nyt nøglepar (Kald evt. generer_noegler() eller lignende logik)

    // Konverter besked til heltalsrepræsentation
    // (Konverter karakterer til numeriske værdier)

    // Krypter med RSA: cipher = besked^e mod n

    // Krypteringsfejl?
    // (Tjek om operationen lykkedes)

    // Hvis Ja (Fejl):
        // Vis fejlbesked og log fejl

    // Hvis Nej (Succes):
        // Eksporter chiffertekst til .txt fil eller udskriv i terminal

    printf("Retur til hovedmenu.\n");
}

void dekrypter_besked() {
    printf("\n--- Starter dekrypteringsflow ---\n");

    // Indtast chiffertekst (fra fil eller terminal)
    // (Læs input)

    // Indlæs nøglepar fra nøglefil
    // (Læs filen)

    // Dekrypter med RSA: besked = cipher^d mod n

    // Konverter heltalsrepræsentation tilbage til streng
    // (Konverter tal til tekst)

    // Dekrypteringsfejl (ugyldig nøgle)?
    // (Valider resultatet)

    // Hvis Ja (Fejl):
        // Vis fejlbesked og log fejl

    // Hvis Nej (Succes):
        // Udskriv dekrypteret besked

    printf("Retur til hovedmenu.\n");
}
