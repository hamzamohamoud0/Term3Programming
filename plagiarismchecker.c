#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_FILE_SIZE 100000
#define MAX_TOKEN_LENGTH 100


struct word_frequency {
    char word[MAX_TOKEN_LENGTH];
    int frequency;
};
char *read_file(const char *filename) {
 FILE *fp;
   char *buffer = NULL;
   long length;


   fp = fopen(filename, "rb");
   if(fp) {
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

int tokenize_file(const char *test_file, char tokens[][100]) {
  char *file_contents = read_file(test_file);
  if (!file_contents) {
      return -1;
  }
  char *saveptr = NULL;
  char *token = strtok_r(file_contents, " \n\t", &saveptr);
  int num_tokens = 0;
  while (token && num_tokens < 10000) {
      // Remove punctuation
      int length = strlen(token);
      for (int i = length - 1; i >= 0; --i) {
          if (ispunct((unsigned char)token[i])) {
              memmove(&token[i], &token[i + 1], length - i);
          }
      }
      // This allows the words to Convert to lower case
      for(int i = 0; token[i]; i++){
          token[i] = tolower(token[i]);
      }
      strncpy(tokens[num_tokens], token, 100);
      token = strtok_r(NULL, " \n\t", &saveptr);
      num_tokens++;
  }
  free(file_contents);
  return num_tokens;
}

float compare_files(const char *test_file1, const char *test_file2) {
    char tokens1[10000][100];
    char tokens2[10000][100];
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


void calculate_phrase_frequency(char tokens1[][100], int num_tokens1, char tokens2[][100], int num_tokens2, char phrases[][100], int max_phrases, int threshold, int *phrase_frequency) {
   char phrase[200];
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
                   strncat(phrase, "", 1);
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
       
       if (!match) {
           num_consecutive_words = 0;
           phrase[0] = '\0';
       }
   }
}
void word_frequency_in_file(const char *word, char tokens[][MAX_TOKEN_LENGTH], int num_tokens) {
    int count = 0;
    for (int i = 0; i < num_tokens; ++i) {
        if (strcmp(word, tokens[i]) == 0) {
            ++count;
        }
    }
    printf("%s: %d\n", word, count);
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
    // Sort the words based on their frequency in descending order
    qsort(word_frequencies, num_words, sizeof(struct word_frequency), compare_word_frequency);
}


void display_word_frequency(char tokens1[][MAX_TOKEN_LENGTH], int num_tokens1, char tokens2[][MAX_TOKEN_LENGTH], int num_tokens2) {
  struct word_frequency word_frequencies1[10000];
  struct word_frequency word_frequencies2[10000];
   calculate_word_frequency(tokens1, num_tokens1, word_frequencies1);
  calculate_word_frequency(tokens2, num_tokens2, word_frequencies2);
  printf("%-20s %20s %-20s %20s\n", "Word", "Frequency (File 1)", "Word", "Frequency (File 2)");
  printf("%-20s %20s %-20s %20s\n", "--------------------", "--------------------", "--------------------", "--------------------");
   int max_tokens = num_tokens1 > num_tokens2 ? num_tokens1 : num_tokens2;
  for (int i = 0; i < max_tokens; i++) {
      char *word1 = i < num_tokens1 ? word_frequencies1[i].word : "";
      int freq1 = i < num_tokens1 ? word_frequencies1[i].frequency : 0;
      char *word2 = i < num_tokens2 ? word_frequencies2[i].word : "";
      int freq2 = i < num_tokens2 ? word_frequencies2[i].frequency : 0;
      // Only print if both frequencies are not zero
      if (freq1 > 0 && freq2 > 0) {
          printf("%-20s %20d %-20s %20d\n", word1, freq1, word2, freq2);
      }
  }
}

int calculate_phrase_match_percentage(const char *file1, const char *file2, int threshold) {
    char tokens1[10000][100];
    char tokens2[10000][100];
    int num_tokens1 = tokenize_file(file1, tokens1);
    int num_tokens2 = tokenize_file(file2, tokens2);
    
    int matched_words = 0;
    for (int i = 0; i < num_tokens1; i++) {
        for (int j = 0; j < num_tokens2; j++) {
            int k = 0;
            while (i+k < num_tokens1 && j+k < num_tokens2 && strcmp(tokens1[i+k], tokens2[j+k]) == 0)
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
void print_matching_phrases(char tokens1[][100], int num_tokens1, char tokens2[][100], int num_tokens2, int threshold) {
  for (int i = 0; i < num_tokens1; i++) {
    for (int j = 0; j < num_tokens2; j++) {
      int k = 0;
      while (i+k < num_tokens1 && j+k < num_tokens2 && strcmp(tokens1[i+k], tokens2[j+k]) == 0)
          k++;
              
      if (k >= threshold) {
        printf("Matching phrase: ");
        for (int l = 0; l < k; l++) {
          printf("%s ", tokens1[i+l]);
        }
        printf("\n");
        i += k - 1;
        break;
      }
    }
  }
}