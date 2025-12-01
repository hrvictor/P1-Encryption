#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//Terker er klam
#define buffer 250

int konverter(const char *sentence, int *sentence_enc);
long long merge(int *sentence_enc, int start_index, int sen_len);

int main(void){

    printf("\n--- Starter krypteringsflow ---\n");

    char sentence[buffer];
    char fname_a[buffer], fname_b[buffer];
    int valg_a, valg_b;
    int len = strlen(sentence);
    int sentence_enc[buffer];
    int sen_len;
    long long int fully_encrypted_sen;
    long long int key;

// Indtast besked klartekst (fra tastatur eller fil)
    printf("Do you wish to encrypt from a directly in the terminal or from a file (1 for terminal and 0 for file)\n");
    scanf("%d", &valg_a);
    getchar();

// (Læs input)
    if(valg_a == 1){
        printf("Type the sentence that you wish to have encrypted:\n");
        fgets(sentence, buffer, stdin);
        sentence[strcspn(sentence, "\n")] = 0;
    } else if(valg_a == 0){
        printf("What is the name of the file that to encrypt from?\n");
        scanf("%s", &fname_a);
        FILE *f1 = fopen(fname_a, "r");
            if(f1 == NULL){
                printf("!FAILED TO OPEN THE FILE!\n");
                exit(EXIT_FAILURE);
            }
            fgets(sentence, buffer, f1);
            fclose(f1);
    } else{
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\nher er din saetning:\n %s\n", sentence); //den er linje er der kun for at tæste input -Therkel

// Brug eksisterende nøglepar?
// (Spørg brugeren)
    printf("Du you wish to use an existing key pair or generate or new one? (1 for existing and 0 for new)\n");
    scanf("%d", &valg_b);
    
    if(valg_b == 1){
// Hvis Ja:
  // Indlæs nøglepar fra nøglefil
    printf("what is the name of the text file containing the key pair?\n");
    scanf("%s", &fname_b);

    FILE *f2 = fopen(fname_b, "r");
            if(f2 == NULL){
                printf("!FAILED TO OPEN THE FILE!\n");
                exit(EXIT_FAILURE);
            }
            fscanf(f2, "%lld", key);
            fclose(f2);
            printf("Here is your keypair: '%lld'\n", key);
    } else if(valg_b == 0){
// Hvis Nej:
  // Generer nyt nøglepar (Kald evt. generer_noegler() eller lignende logik)
        /*---------------------!NOEGLE_FUNKTION_PLACEHOLDER!---------------------*/

    } else {
        printf("!You have given impossible Instructions!\n");
        exit(EXIT_FAILURE);
    }

// Konverter besked til heltalsrepræsentation
// (Konverter karakterer til numeriske værdier)', vi bruger bare "ASCII"
    sen_len = konverter(sentence, sentence_enc); //<--- fuck det her er sutter >:^(

    printf("den oprindelige tekst: '%s'\n", sentence);
    printf("antal tegn gemt: %d\n", sen_len);

    for(int i = 0; i < sen_len; i++){
        printf("%d ", sentence_enc[i]);
    }
    printf("\n");

    long long merged_sentence_enc = merge(sentence_enc, 0, sen_len);

    printf("den samlede tal raekke: %lld\n", merged_sentence_enc);
// Krypter med RSA: cipher = besked^e mod n
    /*sentence_enc[] = (sentence_values[] * n.key);*/ 
    fully_encrypted_sen = (merged_sentence_enc * key);
 
// Krypteringsfejl?
// (Tjek om operationen lykkedes)

    // Hvis Ja (Fejl):
        // Vis fejlbesked og log fejl

    // Hvis Nej (Succes):
        // Eksporter chiffertekst til .txt fil eller udskriv i terminal
    /*printf("You're encrypted message is %s, and is also located in the text file 'encrypted_sentence.txt'\n", fully_encrypted_sen);

    FILE *f = fopen("encrypted_sentence.txt", "w");
    if(f == NULL){
        printf("Filen kunne ikke aabnes\n");
        exit(EXIT_FAILURE);
    }*/

    printf("Retur til hovedmenu.\n");
    return 0;
}

int konverter(const char *text, int *out_sentence_enc){
    int i;
    for(i = 0; text[i] != '\0'; i++){
        out_sentence_enc[i] = (int)text[i];
    }
    return i;
}

long long merge(int *sentence_enc, int start_index, int sen_len){
    long long merged_number = 0;

    for(int i; i < sen_len; i++){
        merged_number = merged_number * 1000 + sentence_enc[start_index + i];
    }
    return merged_number;
}