/* Arquivo tirado diretamente de github.com/padeir0/pao
   Ótimo para debugar segfaults.
*/

#ifndef M2_DEBUG_H
#define M2_DEBUG_H

#include <stdio.h>

#define debug_PRINT(str)                                  \
      do {                                                    \
        fprintf(stderr, "[DEBUG] %s:%s:%d\n",                 \
                __FILE__, __func__, __LINE__);                \
        fprintf(stderr, "\t" str "\n");                       \
      } while (0)

#define debug_PRINTFMT(fmt, ...)                          \
      do {                                                    \
        fprintf(stderr, "[DEBUG] %s:%s:%d\n",                 \
                __FILE__, __func__, __LINE__);                \
        fprintf(stderr, "\t" fmt "\n", __VA_ARGS__);          \
      } while (0)

#define debug_FATAL(str)                                  \
      do {                                                    \
        fprintf(stderr, "[DEBUG] %s:%s:%d\n",                 \
                __FILE__, __func__, __LINE__);                \
        fprintf(stderr, "\t" str "\n");                       \
        abort();                                              \
      } while (0)

#define debug_FATALFMT(fmt, ...)                          \
      do {                                                    \
        fprintf(stderr, "[DEBUG] %s:%s:%d\n",                 \
                __FILE__, __func__, __LINE__);                \
        fprintf(stderr, "\t" fmt "\n", __VA_ARGS__);          \
        abort();                                              \
      } while (0)
#endif
