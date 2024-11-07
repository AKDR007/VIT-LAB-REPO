#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define INITIAL_WORDS_CAPACITY 1000
#define MAX_WORD_LEN 100
#define NUM_THREADS 26

typedef struct {
    char **words;
    int word_count;
    char start_letter;
    int palindromic_count;
} ThreadData;

// Function to reverse a given word
void reverseWord(char *dest, const char *src) {
    int len = strlen(src);
    for (int i = 0; i < len; i++) {
        dest[i] = src[len - i - 1];
    }
    dest[len] = '\0';
}

// Function to check if the reversed word exists in the dictionary
int isPalindromic(const char *word, char **words, int word_count) {
    char reversed[MAX_WORD_LEN];
    reverseWord(reversed, word);

    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i], reversed) == 0) {
            return 1;
        }
    }
    return 0;
}

// Worker function for thread processing
void *worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->palindromic_count = 0;

    for (int i = 0; i < data->word_count; i++) {
        if (tolower(data->words[i][0]) == data->start_letter &&
            isPalindromic(data->words[i], data->words, data->word_count)) {
            data->palindromic_count++;
        }
    }
    return NULL;
}

int main() {
    FILE *file = fopen("/usr/share/dict/words", "r");
    if (!file) {
        perror("Error opening dictionary file");
        return 1;
    }

    int capacity = INITIAL_WORDS_CAPACITY;
    char **words = malloc(sizeof(char *) * capacity);
    if (words == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    int word_count = 0;
    char buffer[MAX_WORD_LEN];

    // Read words from the dictionary file
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';

        // Expand the words array if necessary
        if (word_count >= capacity) {
            capacity *= 2;
            char **temp = realloc(words, sizeof(char *) * capacity);
            if (temp == NULL) {
                perror("Failed to reallocate memory");
                for (int i = 0; i < word_count; i++) {
                    free(words[i]);
                }
                free(words);
                fclose(file);
                return 1;
            }
            words = temp;
        }

        // Store the word in the array
        words[word_count] = strdup(buffer);
        if (words[word_count] == NULL) {
            perror("Failed to allocate memory for word");
            for (int i = 0; i < word_count; i++) {
                free(words[i]);
            }
            free(words);
            fclose(file);
            return 1;
        }
        word_count++;
    }
    fclose(file);

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    // Initialize thread data
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].words = words;
        threadData[i].word_count = word_count;
        threadData[i].start_letter = 'a' + i;
        threadData[i].palindromic_count = 0;
    }

    // Create threads for each letter
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, worker, &threadData[i]) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

    int total_palindromic_count = 0;

    // Wait for threads to finish and collect results
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %c: Found %d palindromic words\n", threadData[i].start_letter, threadData[i].palindromic_count);
        total_palindromic_count += threadData[i].palindromic_count;
    }

    printf("Total palindromic words: %d\n", total_palindromic_count);

    // Free allocated memory
    for (int i = 0; i < word_count; i++) {
        free(words[i]);
    }
    free(words);

    return 0;
}
