#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Platform-specific headers for the 'sleep' function
#ifdef _WIN32 // This macro is automatically defined by Windows compilers (like MinGW/MSVC)
    #include <windows.h>
#else // For Linux/macOS (POSIX systems)
    #include <unistd.h>
#endif

// Konstanter
#define BLOCK_SIZE 2
#define MAX_TEXT_LENGTH 256

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
void save_message(char *message);

int main(void) {
    // Variabler til at gemme nøgler og data
    long long int n = 0, d = 0;
    long long int ciphertext[MAX_TEXT_LENGTH];
    char decrypted_message[MAX_TEXT_LENGTH];
    int num_blocks = 0;

    printf("\033[36m P1 Dekrypterings Program\n\n\033[0m");
    sleep(1);

    // Trin 1: Hent den private nøgle (n, d)
    // Vi skal bruge 'd' og 'n' for at dekryptere: m = c^d mod n
    laes_privat_noegle(&n, &d);
    sleep(1);
    // Tjek om nøgler blev indlæst korrekt
    if (n == 0 || d == 0) {
        printf("Fejl: Kunne ikke indlaese noegler.\n");
        return 1;
    }
    printf("Noegle indlaest: n=\033[32m%lld\033[0m, d=\033[34m%lld\033[0m\n\n", n, d);

    // Trin 2: Læs den krypterede besked (chiffertekst)
    // Dette kunne være fra en fil eller brugerinput, har dog ikke implementeret så man kan skrive direkte i terminalen.
    num_blocks = laes_krypteret_input(ciphertext, MAX_TEXT_LENGTH);

    // Trin 3: Udfør selve dekrypteringen
    // Konverterer de store tal (ciphertext) tilbage til tekst 
    dekrypter_data(ciphertext, num_blocks, n, d, decrypted_message);

    // Trin 4: Vis resultatet til brugeren
    udskriv_besked(decrypted_message);

    sleep(3);

    save_message(decrypted_message);

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
    //Spørg bruger om de vil decrypte fra en fil eller fra terminalen
    int n;
    printf("Would you like to decrypt your message from a [.txt file (1)] or [enter a string manually (2)]? Enter (1/2)\n");
    scanf("%d", &n);
    
    if (n==1){
        // Bed brugeren om at indtaste filnavnet til den krypterede fil
        char fname_a[256];
        printf("\nWhat is your encrypted text file called?\n");
        scanf("%s", fname_a);
        
        FILE *f1 = fopen(fname_a, "r");
        if(f1 == NULL){
            printf("\n!FAILED TO OPEN THE FILE!\n\n");
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
    else if (n==2) {
        printf("\nPlease enter your ciphertext:\n");
        
        //Så fgets fungerer
        getchar();

        char buffer[1024];
        //Læs hvad brugeren skriver
        fgets(buffer, sizeof(buffer), stdin);

        int i = 0;
        //Split i mellemrum
        char *token = strtok(buffer, " ");

        //Loop gennem alle tokens
        while (token != NULL && i < max_blocks) {
            ciphertext[i] = atoll(token);
            i++;
            token = strtok(NULL, " ");
        }
        return i;  // antal blokke læst
    }
    else{
        printf("\n!INVALID RESPONSE!\n");
        exit(EXIT_FAILURE);
    }
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

void save_message(char *message){

    int i = 0;

    while (i == 0){

    char fname_a[256];
    int choice;

    printf("\n\n\nDo you want to save the message as a .txt file?\n");
    printf("0. No\n");
    printf("1. Yes save as a .txt\n");
    sleep(1);
    printf("\nEnter your choice: ");

        // Læs brugerens valg
        if (scanf("%d", &choice) != 1) {
            // Håndter ugyldigt input (f.eks. bogstaver)
            while (getchar() != '\n'); // Ryd input buffer
            choice = 0; // Sæt til ugyldig værdi
        }

    switch (choice)
    {
    case 1:
           printf("\nWhat would you like the file to be called?\n");
           scanf("%s", fname_a);
           
           if (strstr(fname_a, ".txt") != NULL) {
                    printf("\nFile saved as %s\n", fname_a);
                } else {
                    strcat(fname_a, ".txt");
                    printf("\nFile saved as %s\n", fname_a);
                }

           FILE *f3 = fopen(fname_a, "w");
           if(f3 == NULL){
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
           }
           fprintf(f3, "%s", message);
           fclose(f3);
           i = 1;
           printf("\nFile saved as %s\n", fname_a);
            break;
    case 0:
           i = 1;
           break;
            
    default:
    printf("Invalid choice. Please try again.\n");
        break;
    }
    }

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


