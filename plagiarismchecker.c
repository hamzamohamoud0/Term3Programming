#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FILE_SIZE 100000
#define MAX_TOKEN_LENGTH 100

// Structure to hold word and its frequency
struct word_frequency {
    char word[MAX_TOKEN_LENGTH];
    int frequency;
};

// Comparison function for qsort
int compare_word_frequency(const void* a, const void* b) {
    struct word_frequency *wf1 = (struct word_frequency*) a;
    struct word_frequency *wf2 = (struct word_frequency*) b;
    return wf2->frequency - wf1->frequency; // make sure it is order for descending sort
}

char *read_file(const char *filename) {
    FILE *fp;
    char *buffer = NULL;
    long length;

    fp = fopen(filename, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        length = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer) {
            fread(buffer, 1, length, fp);
            buffer[length] = '\0';
        }
        fclose(fp);
    } else {
        printf("Failed to read file: %s\n", filename);
    }
    return buffer;
}

int tokenize_file(const char *test_file, char tokens[][MAX_TOKEN_LENGTH]) {
    char *file_contents = read_file(test_file);
    if (!file_contents) {
        return -1;
    }
    char *saveptr = NULL;
    char *token = strtok_r(file_contents, " \n\t", &saveptr);
    int num_tokens = 0;
    int inside_quotes = 0; 
    while (token && num_tokens < 10000) {
        
        if (strchr(token, '\"')) {
            inside_quotes = !inside_quotes;
        }
        // Only process this token if it's not inside quotes
        if (!inside_quotes) {
            // Remove punctuation
            int length = strlen(token);
            for (int i = length - 1; i >= 0; --i) {
                if (ispunct((unsigned char)token[i])) {
                    memmove(&token[i], &token[i + 1], length - i);
                }
            }
            // Convert to lower case
            for (int i = 0; token[i]; i++) {
                token[i] = tolower(token[i]);
            }
            strncpy(tokens[num_tokens], token, MAX_TOKEN_LENGTH);
            num_tokens++;
        }
        token = strtok_r(NULL, " \n\t", &saveptr);
    }
    free(file_contents);
    return num_tokens;
}

float compare_files(const char *test_file1, const char *test_file2) {
    char tokens1[10000][MAX_TOKEN_LENGTH];
    char tokens2[10000][MAX_TOKEN_LENGTH];
    int num_tokens1 = tokenize_file(test_file1, tokens1);
    int num_tokens2 = tokenize_file(test_file2, tokens2);

    if (num_tokens1 == -1 || num_tokens2 == -1) {
        return -1.0f;
    }

    int count = 0;
    for (int i = 0; i < num_tokens1; ++i) {
        for (int j = 0; j < num_tokens2; ++j) {
            if (strcmp(tokens1[i], tokens2[j]) == 0) {
                ++count;
                break;
            }
        }
    }

    float similarity = ((float)count / (float)(num_tokens1 + num_tokens2 - count)) * 100.0f;
    return similarity;
}

void calculate_phrase_frequency(char tokens1[][MAX_TOKEN_LENGTH], int num_tokens1, char tokens2[][MAX_TOKEN_LENGTH], int num_tokens2, char phrases[][MAX_TOKEN_LENGTH], int max_phrases, int threshold, int *phrase_frequency) {
    char phrase[MAX_TOKEN_LENGTH * threshold];
    int num_consecutive_words = 0;
    int num_phrases = 0;
    phrase[0] = '\0';
for (int i = 0; i < num_tokens1; i++) {
    int match = 0;
    for (int j = 0; j < num_tokens2; j++) {
        if (strncmp(tokens1[i], tokens2[j], 100) == 0) {
            match = 1; 
            num_consecutive_words++;
            if (num_consecutive_words == 1) {
                strncpy(phrase, tokens1[i], 100);
            } else {
                strncat(phrase, "", 1); // Add a space before the next word
                strncat(phrase, tokens1[i], 100);
            }
            if (num_consecutive_words >= threshold) {
                strncpy(phrases[num_phrases], phrase, 100);
                phrase_frequency[num_phrases] = num_consecutive_words;
                num_phrases++;
                if (num_phrases >= max_phrases) {
                    return;
                }
            }
            break;
        }
    }
    // If there was no match, reset phrase and num_consecutive_words
    if (!match) {
        num_consecutive_words = 0;
        phrase[0] = '\0';
    }
}

}

void calculate_word_frequency(char tokens[][MAX_TOKEN_LENGTH], int num_tokens, struct word_frequency word_frequencies[]) {
    int num_words = 0;

    for (int i = 0; i < num_tokens; i++) {
        int found = -1;
        for (int k = 0; k < num_words; k++) {
            if (strcmp(word_frequencies[k].word, tokens[i]) == 0) {
                found = k;
                break;
            }
        }

        if (found != -1) {
            word_frequencies[found].frequency += 1;
        } else {
            strncpy(word_frequencies[num_words].word, tokens[i], MAX_TOKEN_LENGTH);
            word_frequencies[num_words].frequency = 1;
            num_words++;
        }
    }


    qsort(word_frequencies, num_words, sizeof(struct word_frequency), compare_word_frequency);
}

void display_word_frequency(struct word_frequency word_frequencies1[], int num_tokens1, struct word_frequency word_frequencies2[], int num_tokens2) {
    // Create border
    printf("+----------------------+----------------------+----------------------+----------------------+\n");

    // Table headers
    printf("| Word (File 1)        | Frequency (File 1)   | Word (File 2)        | Frequency (File 2)   |\n");

    // Add border
    printf("+----------------------+----------------------+----------------------+----------------------+\n");

    int max_tokens = num_tokens1 > num_tokens2 ? num_tokens1 : num_tokens2;
    for (int i = 0; i < max_tokens; i++) {
        char *word1 = i < num_tokens1 ? word_frequencies1[i].word : "";
        int freq1 = i < num_tokens1 ? word_frequencies1[i].frequency : 0;
        char *word2 = i < num_tokens2 ? word_frequencies2[i].word : "";
        int freq2 = i < num_tokens2 ? word_frequencies2[i].frequency : 0;

        if (freq1 == 0 && freq2 == 0) {
            break;
        }

        printf("| %-20s | %-20d | %-20s | %-20d |\n", word1, freq1, word2, freq2);
    }

    // Add border
    printf("+----------------------+----------------------+----------------------+----------------------+\n");
}

int calculate_phrase_match_percentage(const char *file1, const char *file2, int threshold) {
    char tokens1[10000][MAX_TOKEN_LENGTH];
    char tokens2[10000][MAX_TOKEN_LENGTH];
    int num_tokens1 = tokenize_file(file1, tokens1);
    int num_tokens2 = tokenize_file(file2, tokens2);

    int matched_words = 0;
    for (int i = 0; i < num_tokens1; i++) {
        for (int j = 0; j < num_tokens2; j++) {
            int k = 0;
            while (i + k < num_tokens1 && j + k < num_tokens2 && strcmp(tokens1[i + k], tokens2[j + k]) == 0)
                k++;

            if (k >= threshold) {
                matched_words += k;
                i += k - 1;
                break;
            }
        }
    }

    return (int)(((float)matched_words / (float)num_tokens1) * 100.0f);
}

void print_matching_phrases(char tokens1[][MAX_TOKEN_LENGTH], int num_tokens1, char tokens2[][MAX_TOKEN_LENGTH], int num_tokens2, int threshold) {
    for (int i = 0; i < num_tokens1; i++) {
        for (int j = 0; j < num_tokens2; j++) {
            int k = 0;
            while (i + k < num_tokens1 && j + k < num_tokens2 && strcmp(tokens1[i + k], tokens2[j + k]) == 0)
                k++;

            if (k >= threshold) {
                printf("Matching phrase: ");
                for (int l = 0; l < k; l++) {
                    printf("%s ", tokens1[i + l]);
                }
                printf("\n");
                i += k - 1;
                break;
            }
        }
    }
}



void word_frequency_in_file(const char *word, char tokens[][MAX_TOKEN_LENGTH], int num_tokens) {
    int frequency = 0;

    
    for (int i = 0; i < num_tokens; i++) {
        if (strcmp(tokens[i], word) == 0) {
            frequency++;
        }
    }

    
    printf("Frequency of '%s': %d\n", word, frequency);
}
int compare_phrases(const void* a, const void* b) {
    return strlen((char*)b) - strlen((char*)a);
}

void print_longest_matching_phrases(char phrases[][MAX_TOKEN_LENGTH], int phrase_frequency[], int max_phrases) {
   
    qsort(phrases, max_phrases, sizeof(phrases[0]), compare_phrases);
  
  
    for (int i = 0; i < max_phrases; ++i) {
        
        if (phrase_frequency[i] > 0) {
            printf("Phrase: %s, Frequency: %d\n", phrases[i], phrase_frequency[i]);
        }
    }
}


int main() {
   char test_file1[100], test_file2[100];
   int threshold;
   printf("Enter the first filename to compare: ");
   scanf("%s", test_file1);
   printf("Enter the second filename to compare: ");
   scanf("%s", test_file2);
   printf("Enter the threshold for phrase matching: ");
   scanf("%d", &threshold);

   char tokens1[10000][MAX_TOKEN_LENGTH];
   char tokens2[10000][MAX_TOKEN_LENGTH];
   int num_tokens1 = tokenize_file(test_file1, tokens1);
   int num_tokens2 = tokenize_file(test_file2, tokens2);

   printf("Word Frequencies in %s and %s:\n", test_file1, test_file2);
   struct word_frequency word_frequencies1[10000];
   struct word_frequency word_frequencies2[10000];
   calculate_word_frequency(tokens1, num_tokens1, word_frequencies1);
   calculate_word_frequency(tokens2, num_tokens2, word_frequencies2);
   display_word_frequency(word_frequencies1, num_tokens1, word_frequencies2, num_tokens2);

   char word[100];
   printf("Enter a word to find its frequency: ");
   scanf("%s", word);
   printf("\nFrequency in %s:\n", test_file1);
   word_frequency_in_file(word, tokens1, num_tokens1);
   printf("\nFrequency in %s:\n", test_file2);
   word_frequency_in_file(word, tokens2, num_tokens2);

   float similarity = compare_files(test_file1, test_file2);
   if (similarity == -1.0f) {
       printf("Error: could not open file(s).\n");
   } else {
       printf("Similarity between %s and %s: %.2f%%\n", test_file1, test_file2, similarity);
   }

   int percentage = calculate_phrase_match_percentage(test_file1, test_file2, threshold);
   printf("Phrase Match Percentage: %d%%\n", percentage);

   printf("Matching Phrases (threshold = %d):\n", threshold);
   print_matching_phrases(tokens1, num_tokens1, tokens2, num_tokens2, threshold);

   int max_phrases = 10; 
   char phrases[max_phrases][MAX_TOKEN_LENGTH]; 
   int phrase_frequency[max_phrases]; 
   calculate_phrase_frequency(tokens1, num_tokens1, tokens2, num_tokens2, phrases, max_phrases, threshold, phrase_frequency);

   printf("\nLongest Matching Phrases:\n");
   print_longest_matching_phrases(phrases, phrase_frequency, max_phrases);

   return 0;
}
