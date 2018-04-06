#include "bow.h"


struct bag_struct *new_bag() {
   struct bag_struct *bag;
   /* allocate and store temp variables in the new bag struct */
   bag = malloc(sizeof(struct bag_struct));
   bag->bag = NULL;
   bag->bag_size = 0;
   bag->total_words = 0;
   return bag;
}

int bagsearch( struct bag_struct *bow, char *word ) {
   int i;
   int cmp;
   int pos;
   struct bag_struct *temp_bag;
   temp_bag = new_bag();
   /* check is the bag is of the proper size before checking the middle */
   if ( (bow->bag_size) >= 1 ) {
      i = (bow->bag_size - 1) / 2;
      cmp = strcmp(bow->bag[i].word, word);
      /* search the right half */
      if ( cmp < 0 ) {
         temp_bag->bag_size = i;
         temp_bag->bag = bow->bag + i + 1;
         pos = (bagsearch(temp_bag,word) + 1);
         return pos + i;
      }
      /* search the left half */
      else if ( cmp > 0 ) {
         temp_bag->bag_size = i;
         temp_bag->bag = bow->bag;
         pos = (bagsearch(temp_bag,word));
         return pos;
      }
      /* found the word being looked for */
      else {
         return i;
      }
   }
   else {
      return 0;
   }
}

char *get_word( char **string_ptr ) {
   char *word;
   int i;
   word = malloc(919);
   i = 0;
   while ( (is_letter(*string_ptr[0]) == 0) && (*string_ptr[0] != '\0')  ) {
      *string_ptr += 1;
   }
   /* check if the index place is a letter and add it to the current word */
   while ( (is_letter(*string_ptr[0]) == 1) && (*string_ptr[0] != '\0')  ) {
      word[i] = *string_ptr[0];
      word[i] = tolower(word[i]);
      *string_ptr += 1;
      i += 1;
   }
   word[i] = '\0';
   if ( *string_ptr[0] == '\0' ) {
      free(word);
      return NULL;
   }
   word = realloc(word, strlen(word) + 1);
   return word;
}

struct word_count_struct *new_word_count( char *word ) {
   struct word_count_struct *word_count;
   /* allocate the memory for the struct and set temp variables */
   word_count =  malloc(sizeof(struct word_count_struct));
   word_count->word  = word;
   word_count->count = 1;
   return word_count;
}

int is_letter( char character ) {
   if (isalpha(character) == 0 ) {
      return 0;
   }
   else {
      return 1;
   }
}

void print_word_count( struct word_count_struct *word_count ) {
   if ( word_count->word != NULL ) {
      printf("%s : %d",word_count->word,word_count->count);
   }
   return;
}

void print_bag( struct bag_struct *bow ) {
   int i;
   /* print the contents of the bag structure */
   for ( i = 0; i < bow->bag_size; ++i ) {
      printf("%d : ",i);
      print_word_count(bow->bag + i);
      printf("\n");
   }
   printf("total words: %d\n",bow->total_words);
   return;
}

void add_word( struct bag_struct *bow, char *word ) {
   struct word_count_struct *word_count;
   int pos;
   int i;
   int swtch;
   int index;
   swtch = 0;
   pos = bagsearch(bow,word);
   /* loop though the words in the bag and check if it is still there */
   for ( i = 0; i  < bow->bag_size; ++i ) {
      if ( strcmp(bow->bag[i].word,word) == 0 ) {
         index = i;
         swtch += 1;
      }
   }
   /* if that word is a duplicate do not allocate any memory */
   if ( swtch > 0 ) {
      bow->total_words += 1;
      bow->bag[index].count += 1;
   }
   else {
      bow->total_words += 1;
      bow->bag_size += 1;
      word_count = new_word_count(word);
      bow->bag = realloc(bow->bag,sizeof(struct word_count_struct) * (bow->bag_size + 1));
      /* copy the memory for everything one over then add the word into the space */
      memcpy(bow->bag + pos + 1,bow->bag + pos,sizeof(struct word_count_struct) * (bow->bag_size - 1 - pos));
      memcpy(bow->bag + pos,word_count,sizeof(struct word_count_struct));
   }
   free(word_count);
}

void add_text( struct bag_struct *bow, char *line ) {
   char *word;
   struct word_count_struct *word_count;
   /* loop though the line until the line */
   while ( *line ) {
      word = get_word(&line);
      word_count = new_word_count(word);
      /* only add the word if it is not NULL */
      if ( word_count->word != NULL ) {
         add_word(bow,word);
      }
   }
   return;
}


void read_sms_data( struct bag_struct *bow, char *label ) {
   FILE *fp;
   char *line;
   fp = fopen("SMSSpamCollection","r");
   line = malloc(925);
   /* loop though the entire file reading each line */
   while (!feof(fp)) {
      fgets(line,924,fp);
      if ( strcmp(label, "ham") == 0 ) {
          /* check if the label matches the one on the read line */
          if ( strncmp(label,line,3) == 0 ) {
             add_text(bow, line + 3);
          }
      }
      else if ( strcmp(label,"spam" ) == 0 ) {
          if ( strncmp(label,line,4) == 0 ) {
             add_text(bow, line + 4);
          }
      }
   }
   fclose(fp);
   return;
}

void differences( struct bag_struct *ham, struct bag_struct *spam ) {
   int i;
   int j;
   int compareVal;
   double hamF,spamF,diff;
   i = 0;
   j = 0;
   /* loop through both bags by incrementing two indecies */
   while ((i < ham->bag_size) && (j < spam->bag_size)) {
      compareVal = strcmp(ham->bag[i].word,spam->bag[j].word );
      /* depending on the alpha order of the words calculate a different ratio */
      if (compareVal < 0) {
          hamF = (double)ham->bag[i].count / (double)ham->total_words;
          spamF = 1.0 / (double)ham->total_words;
          i += 1;
      }
      else if (compareVal > 0) {
          hamF = 1.0 / (double)spam->total_words;
          spamF = (double)spam->bag[j].count / (double)spam->total_words;
          j += 1;
      }
      else {
          hamF = (double)ham->bag[i].count / (double)ham->total_words;
          spamF = 1.0 / (double)spam->total_words;
          i += 1;
          j += 1;
      }
      /* check if the ham and spam frequencies are above the required value */
      if ((hamF < 0.005) && (spamF < 0.005)) {
         continue;
      }
      /* calculate the difference and printing it */
      else {
         diff = hamF / spamF;
         if (diff < 1.0) {
            diff = 1.0 / diff;
         }
         /* print the differences and frequencies in the proper order */
         if (diff > 2.0) {
            if (ham->bag[i].count > spam->bag[j].count) {
               printf("%20s %8.6f %8.6f %10.6f %s",ham->bag[i].word,hamF,spamF,diff,"ham");
               printf("\n");
            }
            else {
               printf("%20s %8.6f %8.6f %10.6f %s",ham->bag[i].word,hamF,spamF,diff,"spam");
               printf("\n");
            }
         }
      }
   }
   return;
}

