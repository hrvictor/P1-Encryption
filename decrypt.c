#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Konstanter
#define BLOCK_SIZE 2
#define MAX_TEXT_LENGTH 256

/* 
   Matematiske hjælpefunktioner.
   Disse skal implementeres (eller kopieres fra generer_noegler.c) for at RSA virker.
*/
long long int modExp(long long int base, long long int exp, long long int mod);

/*
WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! WARNING!!!!! 
Jeg er lidt gone i hovedet så ingen spårgsmål, jeg skriver på dansk og engelsk depending on what i feel like lol
*/


//Prototyper
void laes_privat_noegle(long long int *n, long long int *d);
int laes_krypteret_input(long long int *ciphertext, int max_blocks);
void dekrypter_data(long long int *ciphertext, int num_blocks, long long int n, long long int d, char *decrypted_message);
void udskriv_besked(char *message);

int main(void) {
    // Variabler til at gemme nøgler og data
    long long int n = 0, d = 0;
    long long int ciphertext[MAX_TEXT_LENGTH];
    char decrypted_message[MAX_TEXT_LENGTH];
    int num_blocks = 0;

    printf("P1 Dekrypterings Program\n");

    // Trin 1: Hent den private nøgle (n, d)
    // Vi skal bruge 'd' og 'n' for at dekryptere: m = c^d mod n
    laes_privat_noegle(&n, &d);
    
    // Tjek om nøgler blev indlæst korrekt
    if (n == 0 || d == 0) {
        printf("Fejl: Kunne ikke indlaese noegler.\n");
        return 1;
    }
    printf("Noegle indlaest: n=%lld, d=%lld\n", n, d);

    // Trin 2: Læs den krypterede besked (chiffertekst)
    // Dette kunne være fra en fil eller brugerinput, har dog ikke implementeret så man kan skrive direkte i terminalen.
    num_blocks = laes_krypteret_input(ciphertext, MAX_TEXT_LENGTH);

    // Trin 3: Udfør selve dekrypteringen
    // Konverterer de store tal (ciphertext) tilbage til tekst 
    dekrypter_data(ciphertext, num_blocks, n, d, decrypted_message);

    // Trin 4: Vis resultatet til brugeren
    udskriv_besked(decrypted_message);

    return 0;
}

void laes_privat_noegle(long long int *n, long long int *d) {
    // Åbn filen "keys.txt" og find den private nøgle, kan godt bruge forbedringer siden at den kan lave fejl hvis man har flere nøgler i filen.
    // Så der skal laves en catch all til at fange hvis der er mere end et pair.

    FILE *f2;
    f2 = fopen("keys.txt", "r");
    if (f2 == NULL) {
        printf("ERROR 404 CANNOT OPEN FILE\n");
        return;
    } else {
        char line[256];
        while (fgets(line, sizeof(line), f2) != NULL) {
            if (strstr(line, "private key") != NULL) {
                // Find den private nøgle
                // Since the file format might have multiple keys, this logic might need adjustment later.
                char *start = strstr(line, "private key");
                sscanf(start, "private key: (n=%llx, d=%llx)", n, d);
                break;
            }
        }
        fclose(f2);
    }
    

}

int laes_krypteret_input(long long int *ciphertext, int max_blocks) {
    // Bed brugeren om at indtaste filnavnet til den krypterede fil
    char fname_a[256];
    printf("What is your encrypted text file called?\n");
    scanf("%s", fname_a);
    
    FILE *f1 = fopen(fname_a, "r");
    if(f1 == NULL){
        printf("!FAILED TO OPEN THE FILE!\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    // Læs heltal fra filen ind i ciphertext arrayet.
    while (i < max_blocks && fscanf(f1, "%lld", &ciphertext[i]) == 1) {
        i++;
    }
    fclose(f1);

    return i; // Return antal læste blokke
}

void dekrypter_data(long long int *ciphertext, int num_blocks, long long int n, long long int d, char *decrypted_message) {
    // Loop igennem hver blok og dekrypter med RSA formlen m = c^d mod n
    // Derefter de to karakterer ud fra heltallet m
    
    int char_index = 0;
    for(int i = 0; i < num_blocks; i++){
        long long int m = modExp(ciphertext[i], d, n);
        
        // Unpack 2 characters from the integer (reverse order of packing)
        // m = char1 * 256 + char2
        // So char2 is m % 256, and char1 is m / 256
        
        char c2 = (char)(m % 256);
        char c1 = (char)((m / 256) % 256);
        
        decrypted_message[char_index++] = c1;
        decrypted_message[char_index++] = c2;
    }
    // Null-terminate the string so printf knows where it ends, took me legit 30 min to figure this shit out :) fuck C
    decrypted_message[char_index] = '\0';
}

void udskriv_besked(char *message) {
    // Udskriv den dekrypterede besked
    printf("\nDecrypted message:\n%s\n", message);
}

/* Matematiske YAPP Kopieret fra generer_noegler.c*/

long long int modExp(long long int base, long long int exp, long long int mod) {
    long long int result = 1 % mod;

    base = base % mod;

    while (exp > 0) {
        // If the current bit of exp is 1, multiply result by base (mod mod)
        if (exp & 1) {
          result = (result * base) % mod;
        }

        base = base * base % mod; 

        // Shift exponent right by 1 bit (divide by 2)
        exp = exp / 2; 
    }

    return result;
}
