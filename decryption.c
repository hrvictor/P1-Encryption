#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Constants
#define BLOCK_SIZE 2
#define MAX_TEXT_LENGTH 256

long long int modExp1(long long int base, long long int exp, long long int mod);

//Prototypes
void read_private_key(long long int* n, long long int* d);
int read_encrypted_input(long long int* ciphertext, int max_blocks);
void decrypt_data(long long int* ciphertext, int num_blocks, long long int n, long long int d, char* decrypted_message);
void print_message(char* message);
void save_message(char* message);

int decryption(void) {
    //Variabls to save keys and data
    long long int n = 0, d = 0;
    long long int ciphertext[MAX_TEXT_LENGTH];
    char decrypted_message[MAX_TEXT_LENGTH];
    int num_blocks = 0;

    printf("\033[36mDecryption flow started..\n\n\033[0m");
    

    // Step 1: Load the private key (n, d)
    // We will use 'd' og 'n' to decrypt: m = c^d mod n
    read_private_key(&n, &d);
    
    // Check if keys was read
    if (n == 0 || d == 0) {
        printf("Error: Could not read keys\n");
        return 1;
    }
    printf("Key read: n=\033[32m%lld\033[0m, d=\033[34m%lld\033[0m\n\n", n, d);

    // Step 2: Read the encrypted message (ciphertext)
    num_blocks = read_encrypted_input(ciphertext, MAX_TEXT_LENGTH);

    // Step 3: Decrypt
    // Convert numbers back to text 
    decrypt_data(ciphertext, num_blocks, n, d, decrypted_message);

    // Step 4: Show user the result
    print_message(decrypted_message);

    

    save_message(decrypted_message);

    return 0;
}

void read_private_key(long long int* n, long long int* d) {
    // Open the file "keys.txt" and find the private key; it could definitely use improvements since it can produce errors if you have multiple keys in the file.
    // Therefore, a catch-all needs to be implemented to handle cases where there is more than one pair.

    FILE* f2;
    f2 = fopen("private.txt", "r");
    if (f2 == NULL) {
        printf("ERROR 404 CANNOT OPEN FILE\n");
        return;
    }
    else {
        char line[256];
        while (fgets(line, sizeof(line), f2) != NULL) {
            if (strstr(line, "private key") != NULL) {
                // Find the private key
                // Since the file format might have multiple keys, this logic might need adjustment later.
                char* start = strstr(line, "private key");
                sscanf(start, "private key: (n=%llx, d=%llx)", n, d);
                break;
            }
        }
        fclose(f2);
    }


}

int read_encrypted_input(long long int* ciphertext, int max_blocks) {
    // Ask the user to enter the filename of the encrypted file
    char fname_a[256];
    printf("What is your encrypted text file called?\n");
    scanf("%s", fname_a);

    FILE* f1 = fopen(fname_a, "r");
    if (f1 == NULL) {
        printf("!FAILED TO OPEN THE FILE!\n");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    // Read integers from the file into the ciphertext array.
    while (i < max_blocks && fscanf(f1, "%lld", &ciphertext[i]) == 1) {
        i++;
    }
    fclose(f1);

    return i; //Return number of read blocks
}

void decrypt_data(long long int* ciphertext, int num_blocks, long long int n, long long int d, char* decrypted_message) {
    // Loop through each block and decrypt using the RSA formula m = c^d mod n
    // Then extract the two characters from the integer m

    int char_index = 0;
    for (int i = 0; i < num_blocks; i++) {
        long long int m = modExp1(ciphertext[i], d, n);

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

void print_message(char* message) {
    // Print the decrypted message
    printf("\nDecrypted message:\n%s\n", message);
}

void save_message(char* message) {

    int i = 0;

    while (i == 0) {

        char fname_a[256];
        int choice;

        printf("\n\n\nDo you want to save the message as a .txt file?\n");
        printf("0. No\n");
        printf("1. Yes save as a .txt\n");
        
        printf("\nEnter your choice: ");

        // Read user choice
        if (scanf("%d", &choice) != 1) {
            // Handle invalid input
            while (getchar() != '\n'); // Clear input buffer
            choice = 0; // assign invalid value
        }

        switch (choice)
        {
        case 1:
            printf("\nWhat would you like the file to be called?\n");
            scanf("%s", fname_a);

            if (strstr(fname_a, ".txt") != NULL) {
                printf("\nFile saved as %s\n", fname_a);
            }
            else {
                strcat(fname_a, ".txt");
                printf("\nFile saved as %s\n", fname_a);
            }

            FILE* f3 = fopen(fname_a, "w");
            if (f3 == NULL) {
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

//mod exp copied from generate_keys

long long int modExp1(long long int base, long long int exp, long long int mod) {
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
