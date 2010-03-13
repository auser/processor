
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAXLINE
#define MAXLINE 1024
#endif

#define SKIP(p) while (*p && isspace (*p)) p++
#define WANT(p) *p && !isspace (*p)
 
/* Count the number of arguments. */
 
static int count_args (const char * input)
{
  const char * p;
  int argc = 0;
  p = input;
  while (*p) {
    SKIP (p);
    if (WANT (p)) {
      argc++;
      while (WANT (p)) p++;
      }
  }
  return argc;
}
 
/* Copy each non-whitespace argument into its own allocated space. */
 
static int copy_args (const char * input, int argc, char ** argv)
{
  int i = 0;
  const char *p;
  p = input;
  while (*p) {
    SKIP (p);
    if (WANT (p)) {
      const char * end = p;
      char * copy;
      while (WANT (end)) end++;
      copy = argv[i] = (char *)malloc (end - p + 1);
      if (! argv[i]) return -1;
      while (WANT (p)) *copy++ = *p++;
      *copy = 0;
      i++;
    }
  }
  if (i != argc) return -1;
  return 0;
}
 
#undef SKIP
#undef WANT

static inline int argify(const char *line, char ***argv_ptr)
{
  int argc;
  char ** argv;

  argc = count_args (line);
  if (argc == 0)
      return -1;
  argv = (char **)malloc (sizeof (char *) * argc);
  if (! argv) return -1;
  if (copy_args (line, argc, argv) < 0) return -1;
  *argv_ptr = argv;
  
  return argc;
}

#ifdef __cplusplus
}
#endif
