#include "wrap.h"

void recvfrom_int(int);
int count;

/*
typedef struct handshake_struct
{
  SA addr;
  long handshake;
  time_t timestamp;
} handshake_record;
*/
/*
enum SYMBOL_TYPE { HANDSHAKE, ST_INVALID };

typedef struct SYMBOL_tag
{
  char *text;
  struct SYMBOL_tag *next, *prev;
  SYMBOL_TYPE type;
  union 
  {
    handshake_record *record;
  } record_un;
} SYMBOL;

typedef struct SYMTAB_tag
{
  SYMBOL *buckets;
  unsigned long n_buckets;
} SYMTAB;

SYMTAB *CreateSymtab(unsigned long n_buckets)
{
  SYMTAB *symtab;
  symtab = (SYMTAB *)malloc(sizeof(SYMTAB));
  if (!symtab)
    return NULL;
  symtab->buckets = (SYMBOL *)calloc(sizeof(SYMBOL),
                                     n_buckets);
  if (!symtab->buckets)
  {
    free(symtab);
    return NULL;
  }
  symtab->n_buckets = n_buckets;   
  return symtab;
}

SYMBOL *AddIfNew(SYMTAB *table, const char *text)
{
  SYMBOL *walk;
  unsigned long bucket_index;
  bucket_index = TextToHash(text) % table->n_buckets;
  walk = table->buckets + bucket_index;
  while (walk->next)
  {
    if (!strcmp(walk->next->text, text))
      return NULL;
    walk = walk->next;
  }
  walk->next = malloc(sizeof(SYMBOL));
  if (!walk->next)  
    return NULL;  
  walk->next->text = strdup(text);
  walk->next->prev = walk;
  walk->next->next = NULL;
  walk->next->type = ST_INVALID;
  return walk->next;
}


SYMBOL *Find(SYMTAB *table, const char *text)
{

  SYMBOL *walk;

  unsigned long bucket_index;
  bucket_index = TextToHash(text) % table->n_buckets;
  walk = table->buckets + bucket_index;
  while (walk->next)
  {
    if (!strcmp(walk->next->text, text))
      return walk->next;
    walk = walk->next;
  }
  return NULL;
}

SYMBOL *FindOrAdd(SYMTAB *table, const char *text)
{
  SYMBOL *walk;
  unsigned long bucket_index;
  bucket_index = TextToHash(text) % table->n_buckets;
  walk = table->buckets + bucket_index;
  while (walk->next)
  {
    if (!strcmp(walk->next->text, text))
      return walk->next;
    walk = walk->next;
  }
  walk->next = malloc(sizeof(SYMBOL));
  if (!walk->next)
    return NULL;
  walk->next->text = strdup(text);
  walk->next->prev = walk;
  walk->next->next = NULL;
  walk->next->type = ST_INVALID;
  return walk->next;
}

void PushSymtab(SYMTAB *table)
{
  symtab_stack[symtab_stack_top++] = table;
}

SYMTAB *PopSymtab(void)
{
  if (symtab_stack_top)
    return symtab_stack[--symtab_stack_top];
  else
    return NULL;
}
*/

void dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
  socklen_t	len;
  char		mesg[MAXLINE];
  long handshake = 0;
  unsigned long converted;

  Signal(SIGINT, recvfrom_int);

  for( ; ; )
  {
    len = clilen;
    Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);

    if(!strncmp(mesg, "ALIVE", 6))
    {
      handshake = random();
      converted = htonl(handshake);
      memcpy(mesg, &converted, sizeof(unsigned long));
      printf("Sent %ld\n", handshake);
      Sendto(sockfd, mesg, sizeof(unsigned long), 0, pcliaddr, len);
    }

    count++;
  }
}

void recvfrom_int(int signo)
{
  printf("\nreceived %d datagrams\n", count);
  exit(0);
}
