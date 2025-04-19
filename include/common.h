#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define INFO_ENABLED  1
#define DEBUG_ENABLED 1
#define TRACE_ENABLED 1

#ifndef INFO
#define INFO(...) do { if (INFO_ENABLED) fprintf(stderr, "[INFO] " __VA_ARGS__); } while (0)
#endif

#ifndef DEBUG
#define DEBUG(...) do { if (TRACE_ENABLED) fprintf(stderr, "[DEBUG] " __VA_ARGS__); } while (0)
#endif

#ifndef TRACE
#define TRACE(...) do { if (DEBUG_ENABLED) fprintf(stderr, "[TRACE] " __VA_ARGS__); } while (0)
#endif

/* LOCATION */

#define LOCATION __FILE__ ":" STRINGIFY(__LINE__) " in " STRINGIFY(__func__)

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x

/* Gnulib - The GNU Portability Library
https://git.savannah.gnu.org/git/gnulib.git
*/

inline static int safe_alloc_check(void *ptr) {
  /* Return 0 if the allocation was successful, -1 otherwise.  */
  return -!ptr;
}

#define ALLOC(ptr) ALLOC_N(ptr, 1)

#define ALLOC_N(ptr, count) \
  safe_alloc_check((ptr) = calloc(count, sizeof *(ptr))) \

#define REALLOC_N(ptr, count) \
  safe_alloc_check((ptr) = realloc(count, sizeof *(ptr)))

#define FREE(ptr) ((void) (free(ptr), (ptr) = NULL))

static inline void die(const char *msg) {
    perror(msg);
    exit(1);
    abort();
}

#endif
