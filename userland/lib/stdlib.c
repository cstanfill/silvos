#include <stddef.h>
#include <stdint.h>

#include "prelude.h"
#include "userland-lib.h"

void memset (void *ptr, char byte, size_t count) {
  char *p = ptr;
  for (size_t i = 0; i < count; i++) {
    p[i] = byte;
  }
}

void memcpy (void *dest, const void *src, size_t count) {
  char *d = dest;
  const char *s = src;
  for (size_t i = 0; i < count; i++) {
    d[i] = s[i];
  }
}

int strncmp (const char *a, const char *b, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (a[i] > b[i]) {
      return 1;
    }
  }
  return 0;
}

int strcmp (const char *a, const char *b) {
  for (size_t i = 0; ; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (a[i] > b[i]) {
      return 1;
    } else if (a[i] == '\0') {
      return 0;
    }
  }
}

uint64_t strlen (const char *s) {
  uint64_t i = 0;
  while (s[i]) i++;
  return i;
}

char *strncpy (char *dest, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; ++i) {
    dest[i] = src[i];
  }
  for (; i < n; ++i) {
    dest[i] = '\0';
  }
  /* No trailing null byte on truncation, as per <string.h> */
  return dest;
}

char *strcpy (char *dest, const char *src) {
  char c;
  size_t i = 0;
  do {
    c = src[i];
    dest[i] = c;
    i++;
  } while (c != '\0');
  return dest;
}

int debug_printf (const char *fmt, ...) {
  va_list argp_count;
  va_start(argp_count, fmt);
  va_list argp_print;
  va_copy(argp_print, argp_count);

  int n = vsprintf(0, fmt, argp_count);
  va_end(argp_count);

  if (n >= 0) {
    char *buf = __builtin_alloca(n);
    vsprintf(buf, fmt, argp_print);
    _debug(buf, n);
  }
  va_end(argp_print);
  return n;
}
