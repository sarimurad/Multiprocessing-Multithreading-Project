//Dr. abdelSalam Sayyad
//sari abdalghani
//1220982
//section: 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define SizeOfWords 17010000
#define StringSize 90
#define SizeOfUnique 300000
#define NumProcess 2

struct words {
    char string[StringSize];
    int freq;
};
//---------------------------------------------------------------
int main() {
    FILE *file = fopen("text8.txt", "r");

    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // Read all words into an array
    char word[StringSize];
    int NumOfWords = 0;
    char **array = (char **)malloc(SizeOfWords * sizeof(char *));
    while (fscanf(file, "%99s", word) == 1 && NumOfWords < SizeOfWords) {
        array[NumOfWords] = (char *)malloc(StringSize * sizeof(char));
        strcpy(array[NumOfWords], word);
        NumOfWords++;
    }
    fclose(file);

    // Create pipes for processes
    int fd[NumProcess][2];
    for (int i = 0; i < NumProcess; i++) {
        pipe(fd[i]);
    }

    for (int p = 0; p < NumProcess; p++) {
        int pid = fork();

        if (pid == 0) { // Child process
            close(fd[p][0]); // Close reading

            struct words *childFreqWords = (struct words *)malloc(SizeOfUnique * sizeof(struct words));
            int childNumOfUnique = 0;

            // Calculate the range of words for this process
            int start = p * (NumOfWords / NumProcess);
            int end;
             if(p == NumProcess - 1) 
             	end=NumOfWords ;
             else
             	end=start + (NumOfWords / NumProcess);

            // Child process work in its part from the word array
            for (int i = start; i < end; i++) {
                int k;
                for (k = 0; k < childNumOfUnique; k++) {
                    if (strcmp(array[i], childFreqWords[k].string) == 0) {
                        childFreqWords[k].freq++;
                        break;
                    }
                }
                if (k == childNumOfUnique) {
                    strcpy(childFreqWords[childNumOfUnique].string, array[i]);
                    childFreqWords[childNumOfUnique].freq = 1;
                    childNumOfUnique++;
                }
            }

            // Write results to pipe
            write(fd[p][1], &childNumOfUnique, sizeof(int));
            write(fd[p][1], childFreqWords, childNumOfUnique * sizeof(struct words));

            free(childFreqWords);
            close(fd[p][1]);
            exit(0);
        }
    }

    // Parent process
    struct words *freqWords = (struct words *)malloc(SizeOfUnique * sizeof(struct words));
    int numOfUniqueWords = 0;

    // Read results from all child processes and merge
    for (int p = 0; p < NumProcess; p++) {
        close(fd[p][1]); // Close writing 

        int childNumOfUnique;
        read(fd[p][0], &childNumOfUnique, sizeof(int));
        struct words *childFreqWords = (struct words *)malloc(childNumOfUnique * sizeof(struct words));
        read(fd[p][0], childFreqWords, childNumOfUnique * sizeof(struct words));
        close(fd[p][0]);

        // Merge child results into the main frequency list
        for (int i = 0; i < childNumOfUnique; i++) {
            int k;
            for (k = 0; k < numOfUniqueWords; k++) {
                if (strcmp(childFreqWords[i].string, freqWords[k].string) == 0) {
                    freqWords[k].freq += childFreqWords[i].freq;
                    break;
                }
            }
            if (k == numOfUniqueWords) {
                strcpy(freqWords[numOfUniqueWords].string, childFreqWords[i].string);
                freqWords[numOfUniqueWords].freq = childFreqWords[i].freq;
                numOfUniqueWords++;
            }
        }

        free(childFreqWords);
    }

    // Find top 10 frequent words
    struct words *Top10 = (struct words *)malloc(10 * sizeof(struct words));
    for (int i = 0; i < 10; i++) {
        Top10[i].freq = freqWords[i].freq;
        strcpy(Top10[i].string, freqWords[i].string);
    }

    for (int h = 10; h < numOfUniqueWords; h++) {
        int minFreq = Top10[0].freq;
        int index = 0;
        for (int i = 0; i < 10; i++) {
            if (Top10[i].freq < minFreq) {
                minFreq = Top10[i].freq;
                index = i;
            }
        }
        if (freqWords[h].freq > minFreq) {
            Top10[index].freq = freqWords[h].freq;
            strcpy(Top10[index].string, freqWords[h].string);
        }
    }

    for (int h = 0; h < 10; h++) {
        int num;
        char str[StringSize];
        for (int i = 0; i < 9; i++) {
            if (Top10[i].freq < Top10[i + 1].freq) {
                num = Top10[i].freq;
                Top10[i].freq = Top10[i + 1].freq;
                Top10[i + 1].freq = num;

                strcpy(str, Top10[i].string);
                strcpy(Top10[i].string, Top10[i + 1].string);
                strcpy(Top10[i + 1].string, str);
            }
        }
    }

    printf("Top 10 words:\n");
    for (int h = 0; h < 10; h++) {
        printf("%s :      %d\n", Top10[h].string, Top10[h].freq);
    }
    
    printf("\nThis is 2 child process");
    for (int i = 0; i < NumOfWords; i++) {
        free(array[i]);
    }

    free(array);
    free(freqWords);
    free(Top10);

    return 0;
}
