//Dr. abdelSalam Sayyad
//sari abdalghani
//1220982
//section: 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SizeOfWords 17010000
#define StringSize 90
#define SizeOfUnique 300000
#define NumThreads 8

struct words {
    char string[StringSize];
    int freq;
};

struct ThreadInfo {
    char **array;
    int start;
    int end;
    struct words *freqWords;
    int *numOfUniqueWords;
    pthread_mutex_t *mutex;
};
//-----------------------------------------------------------------------------
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *calculateFreq(void *arg) {
    struct ThreadInfo *data = (struct ThreadInfo *)arg;
    char **array = data->array;
    int start = data->start;
    int end = data->end;
    struct words *freqWords = data->freqWords;
    int *numOfUniqueWords = data->numOfUniqueWords;
    pthread_mutex_t *mutex = data->mutex;

    for (int i = start; i < end; i++) {
        int k;
        for (k = 0; k < *numOfUniqueWords; k++) {
            if (strcmp(array[i], freqWords[k].string) == 0) {
                pthread_mutex_lock(mutex); // Lock before modifying 
                freqWords[k].freq++;
                pthread_mutex_unlock(mutex); // Unlock after modification
                break;
            }
        }

        // If the word is not found in freqWords
        if (k == *numOfUniqueWords) {
            pthread_mutex_lock(mutex); // Lock before modifying 
            strcpy(freqWords[k].string, array[i]);
            freqWords[k].freq = 1;
            (*numOfUniqueWords)++;
            pthread_mutex_unlock(mutex); // Unlock after modification
        }
    }

    return NULL;
}
//----------------------------------------------------------------
int main() {
    FILE *file = fopen("text8.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // Scan all words from the file
    char word[StringSize];
    int NumOfWords = 0;
    char **array = (char **)malloc(SizeOfWords * sizeof(char *));
    while (fscanf(file, "%99s", word) == 1 && NumOfWords < SizeOfWords) {
        array[NumOfWords] = (char *)malloc(StringSize * sizeof(char));
        strcpy(array[NumOfWords], word);
        NumOfWords++;
    }
    fclose(file);

    int numOfUniqueWords = 0;
    struct words *freqWords = (struct words *)malloc(SizeOfUnique * sizeof(struct words));

    // Create threads to calculate word frequencies
    pthread_t threads[NumThreads];
    struct ThreadInfo data[NumThreads];
    int arrayPart = NumOfWords / NumThreads;

    for (int i = 0; i < NumThreads; i++) {
        data[i].array = array;
        data[i].start = i * arrayPart;
        if (i == NumThreads - 1) 
           data[i].end = NumOfWords;
        else
           data[i].end = (i + 1) * arrayPart;
        data[i].freqWords = freqWords;
        data[i].numOfUniqueWords = &numOfUniqueWords;
        data[i].mutex = &mutex;

        pthread_create(&threads[i], NULL, calculateFreq, (void *)&data[i]);
    }

    for (int i = 0; i < NumThreads; i++) {
        pthread_join(threads[i], NULL);
    }


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

    // Sort the top 10 by frequency
    for (int h = 0; h < 10; h++) {
        for (int i = 0; i < 9; i++) {
            if (Top10[i].freq < Top10[i + 1].freq) {
                int num = Top10[i].freq;
                Top10[i].freq = Top10[i + 1].freq;
                Top10[i + 1].freq = num;

                char str[StringSize];
                strcpy(str, Top10[i].string);
                strcpy(Top10[i].string, Top10[i + 1].string);
                strcpy(Top10[i + 1].string, str);
            }
        }
    }

    // Print the top 10 words
    printf("Top 10 words:\n");
    for (int h = 0; h < 10; h++) {
        printf("%s : %d\n", Top10[h].string, Top10[h].freq);
    }


    for (int i = 0; i < NumOfWords; i++) {
        free(array[i]);
    }
    printf("\nThis is 8 threads");
    free(array);
    free(freqWords);
    free(Top10);

    return 0;
}
