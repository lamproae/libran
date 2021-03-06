/* Returns the value of 'c' as a hexadecimal digit. */
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "util.h"

void *
zalloc(size_t size)
{
    return calloc(1, size);
}

void *
memdup(const void *p_, size_t size)
{
    void *p = malloc(size);
    memcpy(p, p_, size);
    return p;
}

char *
memdup0(const char *p_, size_t length)
{
    char *p = malloc(length + 1);
    memcpy(p, p_, length);
    p[length] = '\0';
    return p;
}

int
hexit_value(int c)
{
    switch (c) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return c - '0';

    case 'a': case 'A':
        return 0xa;

    case 'b': case 'B':
        return 0xb;

    case 'c': case 'C':
        return 0xc;

    case 'd': case 'D':
        return 0xd;

    case 'e': case 'E':
        return 0xe;

    case 'f': case 'F':
        return 0xf;

    default:
        return -1;
    }
}

/* Returns the integer value of the 'n' hexadecimal digits starting at 's', or
 * UINTMAX_MAX if one of those "digits" is not really a hex digit.  Sets '*ok'
 * to true if the conversion succeeds or to false if a non-hex digit is
 * detected. */
uintmax_t
hexits_value(const char *s, size_t n, bool *ok)
{
    uintmax_t value;
    size_t i;

    value = 0;
    for (i = 0; i < n; i++) {
        int hexit = hexit_value(s[i]);
        if (hexit < 0) {
            *ok = false;
            return UINTMAX_MAX;
        }
        value = (value << 4) + hexit;
    }
    *ok = true;
    return value;
}

bool
str_to_int(const char *s, int base, int *i)
{
    long long ll;
    bool ok = str_to_llong(s, base, &ll);
    *i = ll;
    return ok;
}

bool
str_to_long(const char *s, int base, long *li)
{
    long long ll;
    bool ok = str_to_llong(s, base, &ll);
    *li = ll;
    return ok;
}

bool
str_to_llong(const char *s, int base, long long *x)
{
    char *tail;
    bool ok = str_to_llong_with_tail(s, &tail, base, x);
    if (*tail != '\0') {
        *x = 0;
        return false;
    }
    return ok;
}

bool
str_to_llong_with_tail(const char *s, char **tail, int base, long long *x)
{
    int save_errno = errno;
    errno = 0;
    *x = strtoll(s, tail, base);
    if (errno == EINVAL || errno == ERANGE || *tail == s) {
        errno = save_errno;
        *x = 0;
        return false;
    } else {
        errno = save_errno;
        return true;
    }
}

bool
str_to_uint(const char *s, int base, unsigned int *u)
{
    long long ll;
    bool ok = str_to_llong(s, base, &ll);
    if (!ok || ll < 0 || ll > UINT_MAX) {
        *u = 0;
        return false;
    } else {
        *u = ll;
        return true;
    }
}

bool
str_to_llong_range(const char *s, int base, long long *begin,
                   long long *end)
{
    char *tail;
    if (str_to_llong_with_tail(s, &tail, base, begin)
        && *tail == '-'
        && str_to_llong(tail + 1, base, end)) {
        return true;
    }
    *begin = 0;
    *end = 0;
    return false;
}

/* Converts floating-point string 's' into a double.  If successful, stores
 * the double in '*d' and returns true; on failure, stores 0 in '*d' and
 * returns false.
 *
 * Underflow (e.g. "1e-9999") is not considered an error, but overflow
 * (e.g. "1e9999)" is. */
bool
str_to_double(const char *s, double *d)
{
    int save_errno = errno;
    char *tail;
    errno = 0;
    *d = strtod(s, &tail);
    if (errno == EINVAL || (errno == ERANGE && *d != 0)
        || tail == s || *tail != '\0') {
        errno = save_errno;
        *d = 0;
        return false;
    } else {
        errno = save_errno;
        return true;
    }
}

/* Initializes 'buffer' with 'n' bytes of high-quality random numbers.  Returns
 * 0 if successful, otherwise a positive errno value or EOF on error. */
int
get_entropy(void *buffer, size_t n)
{
    size_t bytes_read;
    int error;
    int fd;

    static const char urandom[] = "/dev/urandom";

    fd = open(urandom, O_RDONLY);
    if (fd < 0) {
        return errno ? errno : EINVAL;
    }

    error = read_fully(fd, buffer, n, &bytes_read);
    close(fd);

    if (error) {
    }
    return error;
}

int
read_fully(int fd, void *p_, size_t size, size_t *bytes_read)
{
    uint8_t *p = p_;

    *bytes_read = 0;
    while (size > 0) {
        ssize_t retval = read(fd, p, size);
        if (retval > 0) {
            *bytes_read += retval;
            size -= retval;
            p += retval;
        } else if (retval == 0) {
            return EOF;
        } else if (errno != EINTR) {
            return errno;
        }
    }
    return 0;
}
