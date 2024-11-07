#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_WORDS_CAPACITY 1000
#define MAX_WORD_LEN 100

// Function to reverse a given word
void reverseWord(char *dest, const char *src) {
    int len = strlen(src);
    for (int i = 0; i < len; i++) {
        dest[i] = src[len - i - 1]; // Reverse copy
    }
    dest[len] = '\0'; // Null-terminate the reversed word
}

// Function to check if the reversed word exists in the dictionary
int isPalindromic(const char *word, char **words, int word_count) {
    char reversed[MAX_WORD_LEN];
    reverseWord(reversed, word); // Reverse the input word

    // Check if the reversed word exists in the list of words
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i], reversed) == 0) {
            return 1; // Found a palindromic match
        }
    }
    return 0; // No match found
}

int main() {
    FILE *file = fopen("/usr/share/dict/words", "r"); // Open the dictionary file
    if (!file) {
        perror("Error opening dictionary file");
        return 1;
    }

    int capacity = INITIAL_WORDS_CAPACITY;
    char **words = malloc(sizeof(char *) * capacity); // Allocate memory for words
    if (words == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return 1;
    }

    int word_count = 0;
    char buffer[MAX_WORD_LEN];

    // Read words from the dictionary file
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

        // Expand the words array if necessary
        if (word_count >= capacity) {
            capacity *= 2;
            char **temp = realloc(words, sizeof(char *) * capacity);
            if (temp == NULL) {
                perror("Failed to reallocate memory");
                for (int i = 0; i < word_count; i++) {
                    free(words[i]); // Free allocated memory
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
                free(words[i]); // Free allocated memory
            }
            free(words);
            fclose(file);
            return 1;
        }
        word_count++;
    }
    fclose(file); // Close the file

    // Check for palindromic words
    int palindromic_count = 0;
    for (int i = 0; i < word_count; i++) {
        if (isPalindromic(words[i], words, word_count)) {
            palindromic_count++;
        }
    }

    printf("Total palindromic words: %d\n", palindromic_count);

    // Free allocated memory
    for (int i = 0; i < word_count; i++) {
        free(words[i]);
    }
    free(words);

    return 0;
}
