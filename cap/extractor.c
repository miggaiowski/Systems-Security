#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pcap.h> 
#include <netdb.h> 
#include <arpa/inet.h>

#define MAXPARAMSIZE 1024
#define MAXPROTOS 1024
#define MAXPROTOSIZE 20
     
/* Flag set by ‘--verbose’. */
static int verbose_flag;

/* String to save the params */
char *victim_ip = NULL, *victim_ethernet = NULL, *proto = NULL, *input = NULL;

/* Result vars */
int num_tcp = 0, num_udp = 0, num_tcp_sessions = 0;

int port_comp(const void* a, const void* b) {
  const char **ia = (const char **)a;
  const char **ib = (const char **)b;
  struct servent *p1, *p2;
  p1 = getservbyname(*ia, "tcp");
  p2 = getservbyname(*ib, "tcp");
  return ntohs(p1->s_port) > ntohs(p2->s_port);
}

void parseParams(int argc, char **argv) {
  int i;
  int c;

  /* opterr = 0;                   /\* Turn off error msgs from getopt *\/ */
     
  while (1) {
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
        /* printf ("option -p with value `%s'\n", optarg); */
        strcpy(proto, optarg);
        break;
     
      case 'i':
        /* printf ("option -i with value `%s'\n", optarg); */
        strcpy(victim_ip, optarg);
        break;
     
      case 'e':
        /* printf ("option -e with value `%s'\n", optarg); */
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
  if (optind < argc) {
    while (optind < argc) {
      strcpy(input, argv[optind++]);
    }
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

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
  u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
  u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
  u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
  u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
  u_char  ip_tos;                 /* type of service */
  u_short ip_len;                 /* total length */
  u_short ip_id;                  /* identification */
  u_short ip_off;                 /* fragment offset field */
#define IP_RF 0x8000            /* reserved fragment flag */
#define IP_DF 0x4000            /* dont fragment flag */
#define IP_MF 0x2000            /* more fragments flag */
#define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
  u_char  ip_ttl;                 /* time to live */
  u_char  ip_p;                   /* protocol */
  u_short ip_sum;                 /* checksum */
  struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
  u_short th_sport;               /* source port */
  u_short th_dport;               /* destination port */
  tcp_seq th_seq;                 /* sequence number */
  tcp_seq th_ack;                 /* acknowledgement number */
  u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
  u_char  th_flags;
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
  u_short th_win;                 /* window */
  u_short th_sum;                 /* checksum */
  u_short th_urp;                 /* urgent pointer */
};

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const u_char *payload, int len, int offset) {

  int i;
  int gap;
  const u_char *ch;

  /* offset */
  printf("%05d   ", offset);
	
  /* hex */
  ch = payload;
  for(i = 0; i < len; i++) {
    printf("%02x ", *ch);
    ch++;
    /* print extra space after 8th byte for visual aid */
    if (i == 7)
      printf(" ");
  }
  /* print space to handle line less than 8 bytes */
  if (len < 8)
    printf(" ");
	
  /* fill hex gap with spaces if not full line */
  if (len < 16) {
    gap = 16 - len;
    for (i = 0; i < gap; i++) {
      printf("   ");
    }
  }
  printf("   ");
	
  /* ascii (if printable) */
  ch = payload;
  for(i = 0; i < len; i++) {
    if (isprint(*ch))
      printf("%c", *ch);
    else
      printf(".");
    ch++;
  }

  printf("\n");

  return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void print_payload(const u_char *payload, int len) {

  int len_rem = len;
  int line_width = 16;			/* number of bytes per line */
  int line_len;
  int offset = 0;					/* zero-based offset counter */
  const u_char *ch = payload;

  if (len <= 0)
    return;

  /* data fits on one line */
  if (len <= line_width) {
    print_hex_ascii_line(ch, len, offset);
    return;
  }

  /* data spans multiple lines */
  for ( ;; ) {
    /* compute current line length */
    line_len = line_width % len_rem;
    /* print line */
    print_hex_ascii_line(ch, line_len, offset);
    /* compute total remaining */
    len_rem = len_rem - line_len;
    /* shift pointer to remaining bytes to print */
    ch = ch + line_len;
    /* add offset */
    offset = offset + line_width;
    /* check if we have line width chars or less */
    if (len_rem <= line_width) {
      /* print last line and get out */
      print_hex_ascii_line(ch, len_rem, offset);
      break;
    }
  }

  return;
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {

  static int count = 1;                   /* packet counter */
	
  /* declare pointers to packet headers */
  const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
  const struct sniff_ip *ip;              /* The IP header */
  const struct sniff_tcp *tcp;            /* The TCP header */
  const char *payload;                    /* Packet payload */

  int size_ip;
  int size_tcp;
  int size_payload;
	
  if (verbose_flag) {
    printf("\nPacket number %d:\n", count);
  }
  count++;
	
  /* define ethernet header */
  ethernet = (struct sniff_ethernet*)(packet);
	
  /* define/compute ip header offset */
  ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
  size_ip = IP_HL(ip)*4;
  if (size_ip < 20) {
    printf("   * Invalid IP header length: %u bytes\n", size_ip);
    return;
  }

  /* print source and destination IP addresses */
  if (verbose_flag) {
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));
  }
	
  /* determine protocol */	
  switch(ip->ip_p) {
  case IPPROTO_TCP:
    if (verbose_flag) {
      printf("   Protocol: TCP\n");
    }
    num_tcp++;
    break;
  case IPPROTO_UDP:
    if (verbose_flag) {
      printf("   Protocol: UDP\n");
    }
    num_udp++;
    return;
  case IPPROTO_ICMP:
    if (verbose_flag) {
      printf("   Protocol: ICMP\n");
    }
    return;
  case IPPROTO_IP:
    if (verbose_flag) {
      printf("   Protocol: IP\n");
    }
    return;
  default:
    if (verbose_flag) {
      printf("   Protocol: unknown\n");
    }
    return;
  }
	
  /*
   *  OK, this packet is TCP.
   */
	
  /* define/compute tcp header offset */
  tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
  size_tcp = TH_OFF(tcp)*4;
  if (size_tcp < 20) {
    printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
    return;
  }
  
  if ((tcp->th_flags & TH_SYN) && !(tcp->th_flags & TH_ACK)) {
    num_tcp_sessions++;
  }

  if (verbose_flag) {
    printf("   Src port: %d\n", ntohs(tcp->th_sport));
    printf("   Dst port: %d\n", ntohs(tcp->th_dport));
  }

  /* define/compute tcp payload (segment) offset */
  payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
	
  /* compute tcp payload (segment) size */
  size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
	
  /*
   * Print payload data; it might be binary, so don't just
   * treat it as a string.
   */
  if (verbose_flag && size_payload > 0) {
    printf("   Payload (%d bytes):\n", size_payload);
    print_payload(payload, size_payload);
  }

  return;
}

int main (int argc, char **argv) {
  int i;
  victim_ip = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  victim_ethernet = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  proto = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  input = (char*)malloc(sizeof(char) * MAXPARAMSIZE); 
  
  parseParams(argc, argv);
  if (verbose_flag)
    printParams();
  
  char ** protos;
  int np = splitProto(proto, &protos);

  if (verbose_flag) {
    printf("%d\n", np);
    for (i = 0; i < np; i++) {
      printf("%s\n", protos[i]);
    }
  }
  
  char errbuf[PCAP_ERRBUF_SIZE];
  struct pcap_pkthdr header;	/* The header that pcap gives us */
  const u_char *packet;		/* The actual packet */
  pcap_t *handle;               /* Session handle */

  handle = pcap_open_offline(input, errbuf);
  if (!handle)
    printf("%s\n", errbuf);
  
  char filter_exp[1024];		/* filter expression [3] */
  struct bpf_program fp;			/* compiled filter program (expression) */
  
  bzero(filter_exp, sizeof(filter_exp));
  if (victim_ip) {
    strcat(filter_exp, "ip host ");
    strcat(filter_exp, victim_ip);
  }
  if (victim_ethernet) {
    strcat(filter_exp, " and ether host ");
    strcat(filter_exp, victim_ethernet);
  }
  
  if (verbose_flag) 
    printf("Filter expression: %s\n", filter_exp);

  /* compile the filter expression */
  if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }

  /* apply the compiled filter */
  if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s\n",
            filter_exp, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }

  int numpackets = 0;
  /* now we can set our callback function */
  pcap_loop(handle, numpackets, got_packet, NULL);
  pcap_close(handle);   /* And close the session */

  printf("%d\n", num_tcp);
  printf("%d\n", num_udp);
  printf("%d\n", num_tcp_sessions);

  num_tcp_sessions = num_tcp = num_udp = 0;

  /* sort the protocols */
  qsort(protos, np, sizeof(char*), port_comp);

  struct servent *p;
  int port;
  for (i = 0; i < np; i++) {
    p = getservbyname(protos[i], "tcp");
    port = ntohs(p->s_port);
    if (verbose_flag)
      printf("%s %d\n", protos[i], port);    

    handle = pcap_open_offline(input, errbuf);
    if (!handle)
      printf("%s\n", errbuf);
    bzero(filter_exp, sizeof(filter_exp));
    sprintf(filter_exp,"ip host %s and ether host %s and tcp port %d", victim_ip, victim_ethernet, port);
  
    if (verbose_flag)
      printf("Filter expression: %s\n", filter_exp);

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
      fprintf(stderr, "Couldn't parse filter %s: %s\n",
              filter_exp, pcap_geterr(handle));
      exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
      fprintf(stderr, "Couldn't install filter %s: %s\n",
              filter_exp, pcap_geterr(handle));
      exit(EXIT_FAILURE);
    }

    /* now we can set our callback function */
    pcap_loop(handle, numpackets, got_packet, NULL);
    pcap_close(handle);   /* And close the session */

    printf("%d\n", num_tcp_sessions);
    num_tcp_sessions = 0;
  }
  return 0;
}

