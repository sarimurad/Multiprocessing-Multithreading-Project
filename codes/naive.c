//Dr. abdelSalam Sayyad
//sari abdalghani
//1220982
//section: 3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SizeOfWords 17010000
#define StringSize 90
#define SizeOfUnique 300000

struct words{
    char string[StringSize];
    int freq;
};

//------------------------------------------------------------------------------
int main() {
    clock_t start, end;
    double serialTime;
    start = clock();

    FILE *file = fopen("text8.txt", "r");

    if (file == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    //Scan all words from file
    char word[StringSize];
    int NumOfWords = 0;
    char **array = (char **)malloc(SizeOfWords * sizeof(char *)); // I use double pointer becuase the array is 2D
    while (fscanf(file, "%99s", word) == 1 && NumOfWords < SizeOfWords) {
        array[NumOfWords] = (char *)malloc(StringSize * sizeof(char));
        strcpy(array[NumOfWords], word);
        NumOfWords++;
    }
    fclose(file);

    int numOfUniqueWords = 0;
    struct words *freqWords = (struct words *)malloc(SizeOfUnique * sizeof(struct words));
    end = clock();
    serialTime = ((double)(end - start)) / CLOCKS_PER_SEC;
    // Calculate Frequancy for words
    for (int i = 0; i < NumOfWords; i++) {
        int k;
        for ( k = 0; k < numOfUniqueWords; k++) {
            if ( strcmp(array[i], freqWords[k].string) == 0 ) {
                freqWords[k].freq++;
                break;
            }
        }
        if (k == numOfUniqueWords) {
                strcpy(freqWords[k].string, array[i]);
                freqWords[k].freq = 1;
                numOfUniqueWords++;
                continue;
            }
    }
    
    start = clock();
    struct words *Top10 = (struct words *)malloc(10 * sizeof(struct words));
    for( int i=0; i<10 ; i++){
        Top10[i].freq=freqWords[i].freq;
        strcpy(Top10[i].string,freqWords[i].string);
    }

    for(int h=10 ; h < numOfUniqueWords ; h++ ){
         int minFreq = Top10[0].freq;
            int index = 0;
            for (int i = 0; i < 10; i++) {
                if (Top10[i].freq < minFreq) {
                    minFreq = Top10[i].freq;
                    index = i;
                }
            }
            if ( freqWords[h].freq > minFreq) {
                Top10[index].freq = freqWords[h].freq;
                strcpy(Top10[index].string, freqWords[h].string);
            }
    }


    for (int h=0; h<10 ; h++){
        int num;
        char str[StringSize];
        for( int i=0 ; i < 9; i++){
            if(Top10[i].freq<Top10[i+1].freq){
                num=Top10[i].freq;
                Top10[i].freq=Top10[i+1].freq;
                Top10[i+1].freq=num;

                strcpy(str,Top10[i].string);
                strcpy(Top10[i].string,Top10[i+1].string);
                strcpy(Top10[i+1].string,str);

            }

        }
    }

    printf("Top 10 words:\n");
    for(int h=0; h<10 ; h++){
        printf("%s :      %d\n",Top10[h].string,Top10[h].freq);
    }



    // Free the allocated memory
    for (int i = 0; i < NumOfWords; i++) {
        free(array[i]);
    }
    free(array);  // Free the array of strings

    free(freqWords);  // Free the frequency array
    end = clock();
    serialTime = serialTime + ( ((double)(end - start)) / CLOCKS_PER_SEC );
    printf("\nThis is Naive \nThe Serial Time is %f",serialTime);

    return 0;
}

