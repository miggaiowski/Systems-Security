#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAXPARAMSIZE 1024
#define MAXPROTOS 1024
#define MAXPROTOSIZE 20
     
/* Flag set by ‘--verbose’. */
static int verbose_flag;

/* String to save the params */
char *victim_ip, *victim_ethernet, *proto, *input;

void parseParams(int argc, char **argv) {
  int i;
  int c;

  /* if (argc < 2) { */
  /*   printf("%s -d <dir> -e <ext1:ext2:...> -o <output>\n", argv[0]); */
  /*   exit(1); */
  /* } */

  /* opterr = 0;                   /\* Turn off error msgs from getopt *\/ */
     
  while (1)
    {
      static struct option long_options[] =
        {
          /* These options set a flag. */
          {"verbose", no_argument,       &verbose_flag, 1},
          {"brief",   no_argument,       &verbose_flag, 0},
          /* These options don't set a flag.
             We distinguish them by their indices. */
          /* {"add",     no_argument,       0, 'a'}, */
          /* {"append",  no_argument,       0, 'b'}, */
          {"victim-ip"      ,  required_argument, 0, 'i'},
          {"victim-ethernet",  required_argument, 0, 'e'},
          {"proto"          ,  required_argument, 0, 'p'},
          {0, 0, 0, 0}
        };
      /* getopt_long stores the option index here. */
      int option_index = 0;
     
      c = getopt_long (argc, argv, "i:e:p:",
                       long_options, &option_index);
     
      /* Detect the end of the options. */
      if (c == -1)
        break;
     
      switch (c)
        {
        case 0:
          /* If this option set a flag, do nothing else now. */
          if (long_options[option_index].flag != 0)
            break;
          printf ("option %s", long_options[option_index].name);
          if (optarg)
            printf (" with arg %s", optarg);
          printf ("\n");
          break;
        
        case 'p':
          printf ("option -p with value `%s'\n", optarg);
          strcpy(proto, optarg);
          break;
     
        case 'i':
          printf ("option -i with value `%s'\n", optarg);
          strcpy(victim_ip, optarg);
          break;
     
        case 'e':
          printf ("option -e with value `%s'\n", optarg);
          strcpy(victim_ethernet, optarg);
          break;
     
        case '?':
          /* getopt_long already printed an error message. */
          break;
     
        default:
          abort ();
        }
    }
     
  /* Instead of reporting ‘--verbose’
     and ‘--brief’ as they are encountered,
     we report the final status resulting from them. */
  if (verbose_flag)
    puts ("verbose flag is set");
     
  /* Print any remaining command line arguments (not options). */
  if (optind < argc)
    {
      printf ("non-option ARGV-elements: ");
      while (optind < argc) {
        strcpy(input, argv[optind]);
        printf ("%s ", argv[optind++]);
      }
      putchar ('\n');
    }
}

void printParams() {
  printf("%s\n", victim_ip);
  printf("%s\n", victim_ethernet);
  printf("%s\n", proto);
  printf("%s\n", input);
}

/* Returns the size of an array of strings, each one with one protocol from proto */
/* Splits the words around ',' */
int splitProto(char* proto, char ***protos_ext) {
  char **protos = *protos_ext;  /* Var interna */
  protos = (char**)malloc(sizeof(char*) * MAXPROTOS);
  int i = 0;
  char *word;
  for (word = strtok(proto, ","); word; word = strtok(NULL, ",")) {
    protos[i] = (char*)malloc(sizeof(char) * MAXPROTOSIZE); 
    strcpy(protos[i++], word);
  }
  *protos_ext = protos;         /* Passa ponteiro para fora */
  return i;
}

int main (int argc, char **argv) {
  victim_ip = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  victim_ethernet = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  proto = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  input = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  
  parseParams(argc, argv);
  printParams();
  
  char ** protos;
  int p = splitProto(proto, &protos);
  printf("%d\n", p);
  int i;
  for (i = 0; i < p; i++) {
    printf("%s\n", protos[i]);
  }
  
  return 0;
}

