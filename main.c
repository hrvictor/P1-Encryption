#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "decryption.c"
#include "generate_keys.c"



#define buffer 250
#define BLOCK_SIZE 2

void generate_or_use_owned_key(void);
void encrypt_from_file_or_terminal(void);
void choice_a(void);
void choice_b(void);
long long mod_Exp(long long base, long long exp, long long mod);
int encrypt_or_decrypt(void);
int encrypt_message(void);

char sentence[buffer];
char fname_a[buffer], fname_b[buffer];
int choiceA, choiceB, choice_c;
int encryptordecrypt;
unsigned long long key;                  // RSA modulus n
const long long public_exp = 0x10001LL;  // RSA public exponent e = 65537

int main(void) {

    printf("\033[36m\nWelcome to K.A.T.J.C.H the RSA encryption program\n\n\033[0m");
    printf("Do you wish to encrypt a message or do you wish to decrypt:\n0 = encryption 1 = decryption \n");
    scanf("%d", &encryptordecrypt);

    getchar(); // consume newline
    encrypt_or_decrypt();
    return 0;
}

int encrypt_or_decrypt()
{

    if (encryptordecrypt == 0) {
        // Encryption flow
        encrypt_from_file_or_terminal();  // gets choiceA, key, and sentence
        encrypt_message();
    }
    else if (encryptordecrypt == 1) {
        // Decryption flow
        decryption();
    }
    else {
        printf("invalid input!\n");
    }
    return 1;
}

void encrypt_from_file_or_terminal(void)
{

    printf("\n--- Starting encryption flow ---\n");

    // --- INPUT SECTION ---
    printf("Do you wish to encrypt directly in the terminal or from a file:\n0 = file\n1 = terminal \n");
    scanf("%d", &choiceA);
    getchar(); // Consume newline

    generate_or_use_owned_key();
}

void generate_or_use_owned_key(void)
{
    char line[256];

    // --- KEY SECTION ---
    printf("\nDo you wish to use an existing key pair or generate a new one:\n0 = new\n1 = existing \n");

    if (!fgets(line, sizeof(line), stdin)) {
        printf("Error reading input.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(line, "%d", &choiceB) != 1) {
        printf("Invalid input.\n");
        exit(EXIT_FAILURE);
    }
    choice_b();
}

void choice_a(void)
{
    if (choiceA == 1) {
        printf("Type the sentence that you wish to have encrypted:\n");

        fgets(sentence, buffer, stdin);
        sentence[strcspn(sentence, "\n")] = '\0';
    }
    else if (choiceA == 0) {
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

    printf("\n\nHere is your sentence:\n %s\n", sentence);
}


void choice_b(void)
{
    if (choiceB == 1) {
        // Use existing key pair
        printf("\nWhat is the name of the text file containing the public key pair (n)?\n");

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
        choice_a();
    }
    else if (choiceB == 0) {
        // Generate new key pair
        generate_keys();
        printf("type name of the public key txt file + .txt to start encryption.\n");

        //checks for file otherwise 
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

        printf("Here is your keypair: '%lld' (hex: %llx)\n", (long long)key, (unsigned long long)key);
        choice_a();
    }
    else {
        printf("!You have given impossible instructions!\n");
        exit(EXIT_FAILURE);
    }
}

int encrypt_message()
{
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
        printf("File could not open\n");
        exit(EXIT_FAILURE);
    }

    for (int b = 0; b < numBlocks; b++) {
        long long int m = 0;
        // Pack characters into one integer
        for (int j = 0; j < BLOCK_SIZE; j++) {
            unsigned char ch = (unsigned char)sentence[b * BLOCK_SIZE + j];
            m = m * 256 + ch;
        }

        long long int c = mod_Exp(m, public_exp, (long long)key);
        printf("%lld ", c);
        fprintf(f3, "%lld ", c); // Write space-separated blocks
    }
    printf("\n");

    fclose(f3);
    printf("Your encrypted message is located in the text file 'crypted.txt'\n");
    return 0;
}

// Helper function for RSA math to prevent overflow
long long mod_Exp(long long base, long long exp, long long mod) {
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
