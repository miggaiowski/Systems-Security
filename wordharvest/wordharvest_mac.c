/* Miguel Gaiowski */
/* RA 076116 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             /* required by getopt */
#include <ctype.h>

#define MAXSIZE (1<<30)
#define MAXWORDS (1<<30)
#define BUFFERSIZE (1<<10)
#define MAXLINELEN (1<<8)
#define STARTTHRESHOLD (1<<10)
#define TMPFILE "tmpStrings.tmp"

#define SHRINK

/* As required by qsort */
/* This would've been much simpler */
int compare(const void* a, const void* b) {
  return strcmp(*(char**)a, *(char**)b);
}

/* The saddest code ever written. */
int compare_case(const void* a, const void* b) {
  char str1[1000], str2[1000];
  strcpy(str1, *(char**)a);
  strcpy(str2, *(char**)b);
  int i = 0, j = 0;
  int sz1 = strlen(*(char**)a);
  int sz2 = strlen(*(char**)b);

  while (i < sz1 && j < sz2 && str1[i] == str2[j]) {
    i++; j++;
  }

  if (i >= sz1 || j >= sz2)
    if (sz1 == sz2)
      return 0;
    else
      return sz1 < sz2 ? -1 : 1;

  if (str1[i] >= '0' && str1[i] <= '9') {
    if (str2[j] > '9')
      return -1;
    else 
      return str1[i] < str2[j] ? -1 : 1;
  }
  if (str1[i] >= 'a' && str1[i] <= 'z') {
    if (str2[j] <= '9')
      return 1;
    else if (str2[j] <= 'Z') {
      if (str1[i] == str2[j] + ('a' - 'A'))
        return -1;
      else
        return str1[i] < (str2[j] + ('a' - 'A')) ? -1 : 1;
    }
    else 
      return str1[i] < str2[j] ? -1 : 1;
  }
  if (str1[i] >= 'A' && str1[i] <= 'Z') {
    if (str2[j] < 'A')
      return 1;
    else if (str2[j] >= 'a') {
      if (str1[i] == str2[j] - ('a' - 'A'))
        return 1;
      else
        return str1[i] < (str2[j] - ('a' - 'A')) ? -1 : 1;
    }
    else 
      return str1[i] < str2[j] ? -1 : 1;
  }
  /* return strcasecmp(*(char**)a, *(char**)b); */ 
}

/* Executes cmd and returns string with output */
char* exec(char* cmd) {
  FILE* pipe = popen(cmd, "r");
  if (!pipe) return "ERROR";
  char buffer[1024];
  unsigned long long int size = 0;
  int bytes_read;
  char* result = (char*)malloc(sizeof(char) * MAXSIZE);
  bzero(result, MAXSIZE);
  while(!feof(pipe)) {
    if(fgets(buffer, 1024, pipe) != NULL) {
      bytes_read = strnlen(buffer, 1024);
      size += bytes_read;
      if (size > MAXSIZE) {
        fprintf(stderr, "File list too long, using only the first %d characters from find.\n", MAXSIZE);
        size -= bytes_read;
        break;
      }
      else {
        strncat(result, buffer, 1024);
      }
    }
  }
  /* Maybe our result string was too  short and the last fgets did not
     finish getting a whole line  into buffer, so let's eliminate this
     garbage and leave only whole lines, all ended by '\n' */
  if (result[size-1] != '\n') {
    while (size > 0 && result[size-1] != '\n')
      result[size--] = '\0';
  }
  pclose(pipe);
  return result;
}

/* Returns a string with the find command to be executed */
char* buildFindCmd(char** extensions, char* dir) {
  char* cmd = (char*)malloc(sizeof(char) * MAXLINELEN);
  bzero(cmd, MAXLINELEN);    /* ALWAYS bzero before strcat! */

  strcat(cmd, "find ");
  strcat(cmd, dir);
  strcat(cmd, " -type f");

  int first = 1;
  char *ext;
  while ((ext = strsep(extensions, ":")) != NULL) {
    if (!first) {
      strcat(cmd, " -or");
    }
    strcat(cmd, " -name \"*.");
    strcat(cmd, ext);
    strcat(cmd, "\"");
    first = 0;
  }  
  return cmd;
}

/* Eliminates duplicates, returns the new size */
int shrink2(char** words, int num_words) {
  int i, j, k;
  qsort(words, num_words, sizeof(char*), compare);
  i = 0; j = 0;
  while(i < num_words && j < num_words) {
    while (j < num_words && !strcmp(words[i], words[j])) 
      j++;
    if (i < num_words-1 && j < num_words) {
      words[++i] = words[j];
    }
  }
  return i+1;
}

/* Eliminates duplicates, returns the new size */
int shrink(char** words, int num_words) {
  int i, j, k;
  qsort(words, num_words, sizeof(char*), compare);
  i = 0; j = 1;
  while(i < num_words && j < num_words) {
    while (j < num_words && !strcmp(words[i], words[j])) {
      free(words[j]);
      words[j] = NULL;
      j++;
    }
    if (i < num_words-1 && j < num_words) {
      /* words[++i] = words[j++]; */
      i++;
      words[i] = words[j];
      j++;
    }
  }
  return i+1;
}

/* Inserts a word into words. Returns length of the string. */
int insert(char** words, long long int* num_words, char* buffer, int p) {
  int len = strlen(buffer+p);
  if (len) {
    char* new_word = (char*)malloc(sizeof(char) * MAXLINELEN);
    bzero(new_word, MAXLINELEN);
    strncpy(new_word, buffer+p, MAXLINELEN);
    words[*num_words] = new_word;
    *num_words += 1;
  }
  return len;
}

/* Takes an open file and adds all words from the file to words. */
int addWords(FILE* f, char** words) {
  int p, q;
  long long int num_words = 0;
  char buffer[BUFFERSIZE+1];
  int concat = 0;
  int size, len;
  unsigned long long int threshold = 4 * STARTTHRESHOLD;
  int threshold_hit = 0;

  while (!feof(f)) {
#ifdef SHRINK
    if (num_words > threshold) { /* Whenever we've got more than THRESHOLD words */
      /* fprintf(stderr, " *********************** Mini Shrinking, Threshold: %llu ***********************\n\n", threshold); */
      num_words = shrink(words, num_words); /* Eliminate duplicates */
      threshold_hit++;
      if (threshold_hit > 2) {
        threshold *= 2;
        threshold_hit = 0;
      }
    }
#endif
    if (fgets(buffer, BUFFERSIZE, f) != NULL) {
      size = strlen(buffer)+1;
      /* printf("%s\n", buffer); */
      /* Insert each word into words */
      p = q = 0;
      while (p < size-1) {
        while (q < size && isalnum(buffer[q++]));
        /* printf("q: %d\n", q); */
        if (q < size) {     /* A non-alfanumeric char was found before end of buffer. */
          buffer[q-1] = '\0';       /* Signal that the word ends at q */
        }
        if (!concat) {
          len = insert(words, &num_words, buffer, p);
          /* printf("Adicionada: %s\n", words[num_words-1]); */
        }
        else {
          strcat(words[num_words-1], buffer+p);
          /* printf("Concatenada: %s\n", words[num_words-1]); */
          concat = 0;
        }
        if (q >= size && !len) {  /* This means that '\0' was at the end off buffer, possible split word */
          concat = 1;
          p = q;
        }
        else {
          while (q < size && !isalnum(buffer[q++]));
          p = q-1;
          q--;
        }
      }
    }
  }
#ifdef SHRINK
  num_words = shrink(words, num_words); /* Eliminate duplicates */
#endif
  return num_words;
}

void makeTmpWithStrings(char *file) {
  char strings_cmd[BUFFERSIZE] = "strings -n 6 \"";
  strcat(strings_cmd, file);
  strcat(strings_cmd, "\"");
  char *content = exec(strings_cmd);
  FILE *tmp = fopen("tmpfile", "w");
  fwrite(content, strlen(content), 1, tmp);
  fclose(tmp);
  free(content);
}

int isDoc(char *file) {
  int len = strlen(file);
  if (file[len-3] == 'd' && file[len-2] == 'o' && file[len-1] == 'c') 
    return 1;
  return 0;
}

int countFilelist(char *filelist) {
  int n = 0, i = 0;
  while(filelist[i] != '\0') {
    if (filelist[i] == '\n')
      n++;
    i++;
  }
  return n;
}

int main (int argc, char **argv) {
  /* extensions must be allocated in the heap because strsep will mess
     with it inside buildFindCmd */
  char *extensions = (char*)malloc(sizeof(char) * MAXLINELEN); 
  bzero(extensions, MAXLINELEN);
  strcat(extensions, "txt:text");
  char *dir = ".";
  char *output = NULL;
  int i;
  int c;

  if (argc < 2) {
    printf("%s -d <dir> -e <ext1:ext2:...> -o <output>\n", argv[0]);
    exit(1);
  }

  /* opterr = 0;                   /\* Turn off error msgs from getopt *\/ */
     
  while ((c = getopt (argc, argv, "e::d:o::")) != -1)
    switch (c)
      {
      case 'e':
        free(extensions); 
        extensions = optarg;
        break;
      case 'd':
        dir = optarg;
        break;
      case 'o':
        output = optarg;
        break;
      case '?':
        if (optopt == 'e' || optopt == 'd' || optopt == 'o')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
     
  /* fprintf(stderr, "extensions = %s, dir = %s, output = %s\n", */
  /*         extensions, dir, output); */
     
  /* for (i = optind; i < argc; i++) */
  /*   fprintf(stderr, "Non-option argument %s\n", argv[i]); */
  
  char* cmd = buildFindCmd(&extensions, dir);
  char* filelist = exec(cmd);
  char* savedFilelist = filelist;
  free(cmd);

  /* printf("%s", filelist); */

  char** words = (char**)malloc(sizeof(char*) * MAXWORDS);
  unsigned long long int num_words = 0, num_words_turn;
  unsigned long long int threshold = STARTTHRESHOLD;  
  int threshold_hit = 0;
  char *file;
  FILE *f;
  int numFiles = countFilelist(filelist);
  int numFilesDone = 0;
  while ((file = strsep(&filelist, "\n")) != NULL) {
    if (file[0] != '\0') {
      /* fprintf(stderr, "===== Opening %s\n", file); */
      if (isDoc(file)) {
        makeTmpWithStrings(file);
        strcpy(file, "tmpfile");
      }
      f = fopen(file, "r");
      num_words_turn = addWords(f, words+num_words);
      /* fprintf(stderr, "===== Total: %llu - Added %llu words from file %s\n", num_words, num_words_turn, file); */
      fprintf(stderr, "===== Total: %llu                        %d of %d files \r", num_words, numFilesDone, numFiles-1);
      num_words += num_words_turn;
      if (num_words >= MAXWORDS-1) {
        fprintf(stderr, " *********************** Numero maximo de palavras excedido. ***********************\n\n\n");
      }
#ifdef SHRINK
      if (num_words > threshold) { /* Whenever we've got more than THRESHOLD words */
        /* fprintf(stderr, " *********************** Shrinking, Threshold: %llu ***********************\n\n", threshold); */
        num_words = shrink(words, num_words); /* Eliminate duplicates */
        threshold_hit++;
        if (threshold_hit > 2) {
          threshold *= 2;
          threshold_hit = 0;
        }
      }
#endif
      fclose(f);
      numFilesDone++;
    }
  }
  num_words = shrink(words, num_words);
  /* fprintf(stderr, "===== Total words found: %llu\n", num_words); */
  
  qsort(words, num_words, sizeof(char*), compare_case);

  if (output) {
    f = fopen(output, "w");
    for (i = 0; i < num_words; i++) {
      fprintf(f, "%s\n", words[i]);
      free(words[i]);
    }
    fclose(f);
  }
  else {
    for (i = 0; i < num_words; i++) {
      printf("%s\n", words[i]);
      free(words[i]);
    }
  }

  fprintf(stderr, "===== Total words found: %llu\n", num_words);

  free(words);
  free(savedFilelist);
  if (extensions)
    free(extensions);

  return 0;
}
