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



