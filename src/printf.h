#ifndef PRINTF_H
#define PRINTF_H

// Error out and die
void xperror_and_die(int exit_code, const char *s, ...)
{
	va_list p;

	va_start(p, s);
	/* Guard against "<error message>: Success" */
	bb_verror_msg(s, p, errno ? strerror(errno) : NULL);
	va_end(p);
	exit(exit_code);
}

#endif