#ifndef FS_H
#define FS_H

// Die if we can't chdir to a new path.
static inline void xchdir(const char *path)
{
	if (chdir(path)) perror("chdir(%s)", path);
}

#endif