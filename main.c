#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "decryption.c"
#include "Encryption.c"
#include "generer_noegler.c"

#define buffer 250
#define BLOCK_SIZE 2

void keyselection(void);
void sentenceinput(void);
void menuselection(void);

char sentence[buffer];
char fname_a[buffer], fname_b[buffer];
int valg_a, valg_b;
int encryptordecrypt;
unsigned long long key;                  // RSA modulus n
const long long public_exp = 0x10001LL;  // RSA public exponent e = 65537

int main(void) {

    printf("Do you wish to encrypt a message or do you wish to decrypt. 1 = encryption, 2 = decryption \n");
    scanf("%d",  &encryptordecrypt);
    getchar(); // consume newline

    if (encryptordecrypt == 1) {
        // Encryption flow
        menuselection();  // gets valg_a, key, and sentence

        // --- ENCRYPTION SECTION ---
        int len = (int)strlen(sentence);

        // Pad with nulls if needed so we always have complete 2-byte blocks
        if (len % BLOCK_SIZE != 0) {
            sentence[len] = '\0';
            len++;
        }

        int numBlocks = len / BLOCK_SIZE;
        printf("Encrypted blocks: ");

        FILE* f3 = fopen("crypted.txt", "w"); // Use "w" to overwrite/start fresh
        if (f3 == NULL) {
            printf("Filen kunne ikke aabnes\n");
            exit(EXIT_FAILURE);
        }

        for (int b = 0; b < numBlocks; b++) {
            long long int m = 0;
            // Pack characters into one integer
            for (int j = 0; j < BLOCK_SIZE; j++) {
                unsigned char ch = (unsigned char)sentence[b * BLOCK_SIZE + j];
                m = m * 256 + ch;
            }

            long long int c = modExp(m, public_exp, (long long)key);
            printf("%lld ", c);
            fprintf(f3, "%lld ", c); // Write space-separated blocks
        }
        printf("\n");

        fclose(f3);
        printf("Your encrypted message is located in the text file 'crypted.txt'\n");
    }
    else if (encryptordecrypt == 2) {
        // Decryption flow
        decryption();
    }
    else {
        printf("fejl input!\n");
    }

    printf("Retur til hovedmenu.\n");
    return 0;
}

void menuselection(void)
{
    printf("\n--- Starter krypteringsflow ---\n");

    // --- INPUT SECTION ---
    printf("Do you wish to encrypt from a directly in the terminal or from a file (1 for terminal and 0 for file) \n");
    scanf("%d", &valg_a);
    getchar(); // Consume newline

    keyselection();
}

void keyselection(void)
{
    char line[256];

    // --- KEY SECTION ---
    printf("Do you wish to use an existing key pair or generate a new one? (1 for existing, 0 for new)\n");

    if (!fgets(line, sizeof(line), stdin)) {
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(line, "%d", &valg_b) != 1) {
        printf("Invalid input.\n");
        exit(EXIT_FAILURE);
    }

    if (valg_b == 1) {
        // Use existing key pair
        printf("\nWhat is the name of the text file containing the key pair (n)?\n");

        if (!fgets(fname_b, sizeof(fname_b), stdin)) {
            printf("Error reading filename.\n");
            exit(EXIT_FAILURE);
        }
        // Remove newline from filename
        fname_b[strcspn(fname_b, "\n")] = '\0';

        FILE* f2 = fopen(fname_b, "r");
        if (f2 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }

        char fileline[256];
        int found = 0;

        while (fgets(fileline, sizeof(fileline), f2) != NULL) {
            if (strstr(fileline, "public key") != NULL) {
                // Format: public key key: (n=6f249d, e=10001)
                char* n_ptr = strstr(fileline, "n=");
                if (n_ptr != NULL) {
                    if (sscanf(n_ptr, "n=%llx", &key) == 1) {
                        found = 1;
                        break;
                    }
                }
            }
        }
        fclose(f2);

        if (!found) {
            printf("!Could not find public key in file!\n");
            exit(EXIT_FAILURE);
        }

        printf("Here is your keypair: '%lld' (hex: %llx)\n",
               (long long)key, (unsigned long long)key);
        sentenceinput();
    }
    else if (valg_b == 0) {
        // Generate new key pair
        generate_keys();

        printf("\nWhat is the name of the text file containing the key pair (n)?\n");

        if (!fgets(fname_b, sizeof(fname_b), stdin)) {
            printf("Error reading filename.\n");
            exit(EXIT_FAILURE);
        }
        fname_b[strcspn(fname_b, "\n")] = '\0';

        FILE* f2 = fopen(fname_b, "r");
        if (f2 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }

        char fileline[256];
        int found = 0;

        while (fgets(fileline, sizeof(fileline), f2) != NULL) {
            if (strstr(fileline, "public key") != NULL) {
                char* n_ptr = strstr(fileline, "n=");
                if (n_ptr != NULL) {
                    if (sscanf(n_ptr, "n=%llx", &key) == 1) {
                        found = 1;
                        break;
                    }
                }
            }
        }
        fclose(f2);

        if (!found) {
            printf("!Could not find public key in file!\n");
            exit(EXIT_FAILURE);
        }

        printf("Here is your keypair: '%lld' (hex: %llx)\n",
               (long long)key, (unsigned long long)key);
        sentenceinput();
    }
    else {
        printf("!You have given impossible instructions!\n");
        exit(EXIT_FAILURE);
    }
}

void sentenceinput(void)
{
    if (valg_a == 1) {
        printf("Type the sentence that you wish to have encrypted:\n");

        fgets(sentence, buffer, stdin);
        sentence[strcspn(sentence, "\n")] = '\0';
    }
    else if (valg_a == 0) {
        printf("What is the name of the file that to encrypt from?\n");
        scanf("%s", fname_a);

        FILE* f1 = fopen(fname_a, "r");
        if (f1 == NULL) {
            printf("!FAILED TO OPEN THE FILE!\n");
            exit(EXIT_FAILURE);
        }
        fgets(sentence, buffer, f1);
        fclose(f1);
        sentence[strcspn(sentence, "\n")] = '\0';
    }
    else {
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\nher er din saetning:\n %s\n", sentence);
}
