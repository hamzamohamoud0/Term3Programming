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

