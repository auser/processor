
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAXLINE
#define MAXLINE 1024
#endif

static inline int argify(const char *line, char **argv)
{
  char *buf;
  char *delim;
  int argc;
  
  strcpy(buf, line);
  
  if (buf[ strlen(buf) - 1 ] == '\n') buf[strlen(buf)-1] = ' ';  // replace trailing '\n' with space
  while (*buf && (*buf == ' ')) buf++; // strip off useless spaces at the front
  
  // Build the argv
  argc = 0;
  if (*buf == '\'') {
    buf++;
    delim = strchr(buf, '\'');
	} else { delim = strchr(buf, ' '); }
  
  while (delim) {
    argv[argc++] = buf;
    *delim = '\0';
    buf = delim + 1;
    
    printf("argv[%d] = %s\n", argc-1, argv[argc-1]);
    
    // Ignore spaces
    while (*buf && (*buf == ' ')) buf++;

    if (*buf == '\'') {
      buf++;
      delim = strchr(buf, '\'');
		} else { delim = strchr(buf, ' '); }
	}
	
	argv[argc] = NULL;
  
  if (argc == 0) return -1;
  return argc;
}

static inline void shift_left(char *argv[])
{
  int i = 0;
  while(argv[i] != NULL) 
    argv[i] = argv[++i];
}

static inline void shift_right(char *argv[])
{
  int i = 0;
  while(argv[i++] != NULL) ;
  for(int j = i; j > 0; j--) argv[j] = argv[j-1];
}

static inline void shift(char *argv[], char direction = 'l')
{
  if (direction == 'l') shift_left(argv);
  else shift_right(argv);
}

#ifdef __cplusplus
}
#endif
