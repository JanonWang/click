#ifndef GLUE_HH
#define GLUE_HH

/*
 * Junk required to compile as a Linux kernel module as well as in user-space.
 * Removes many common #include <header>s and abstracts differences between
 * kernel and user space.
 */

// produce debugging output on the console or stderr
void click_chatter(const char *fmt, ...);

#ifdef CLICK_LINUXMODULE

#if CLICK_DMALLOC
extern int click_dmalloc_where;
# define CLICK_DMALLOC_REG(s) do { const unsigned char *__str = reinterpret_cast<const unsigned char *>(s); click_dmalloc_where = (__str[0]<<24) | (__str[1]<<16) | (__str[2]<<8) | __str[3]; } while (0)
#else
# define CLICK_DMALLOC_REG(s)
#endif

// ask for ino_t, off_t, &c to be defined
#define _LOOSE_KERNEL_NAMES 1
#undef __KERNEL_STRICT_NAMES

#ifndef __OPTIMIZE__
# define __OPTIMIZE__ /* get ntohl() macros. otherwise undefined. */
#endif

#include <click/cxxprotect.h>
CLICK_CXX_PROTECT
#if defined(CLICK_PACKAGE) || defined(WANT_MOD_USE_COUNT)
# define __NO_VERSION__
# include <linux/module.h>
# define HAVE_MOD_USE_COUNT 1
#endif
#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/skbuff.h>
#include <linux/malloc.h>
#include <linux/ctype.h>
#include <linux/time.h>
#include <linux/errno.h>
CLICK_CXX_UNPROTECT
#include <click/cxxunprotect.h>

// provide a definition for net_device for kernel compatibility
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0)
typedef struct device net_device;
#define dev_get_by_name dev_get
#endif

extern "C" {

__inline__ unsigned
random()
{
  static unsigned seed = 152L;
  seed = seed*69069L + 1;
  return seed^jiffies;
}

__inline__ uint64_t
click_get_cycles()
{
  uint32_t low, high;
  uint64_t x;
  __asm__ __volatile__("rdtsc":"=a" (low), "=d" (high));
  x = high;
  x <<= 32;
  x |= low;
  return x;
}

long strtol(const char *, char **, int);

__inline__ unsigned long
strtoul(const char *nptr, char **endptr, int base)
{
  return simple_strtoul(nptr, endptr, base);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 4, 0) && __GNUC__ == 2 && __GNUC_MINOR__ == 96
int click_strcmp(const char *, const char *);

inline int
strcmp(const char *a, const char *b)
{
  return click_strcmp(a, b);
}
#endif

}

#define CLICK_HZ HZ
#define click_gettimeofday(tvp) (do_gettimeofday(tvp))
#define click_jiffies()		((unsigned)jiffies)

#else /* not CLICK_LINUXMODULE */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define CLICK_DMALLOC_REG(s)

__inline__ unsigned long long
click_get_cycles()
{
  return(0);
}

#define click_gettimeofday(tvp) (gettimeofday(tvp, (struct timezone *) 0))
#define CLICK_HZ 100		// click_jiffies rate
extern unsigned click_jiffies();

// provide a definition for net_device
typedef struct device net_device;

#endif /* CLICK_LINUXMODULE */

#ifndef timercmp
/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
# define timerisset(tvp)	((tvp)->tv_sec || (tvp)->tv_usec)
# define timerclear(tvp)	((tvp)->tv_sec = (tvp)->tv_usec = 0)
# define timercmp(a, b, CMP)			\
  (((a)->tv_sec == (b)->tv_sec) ?		\
   ((a)->tv_usec CMP (b)->tv_usec) :		\
   ((a)->tv_sec CMP (b)->tv_sec))
#endif
#ifndef timeradd
# define timeradd(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;			      \
    if ((result)->tv_usec >= 1000000)					      \
      {									      \
	++(result)->tv_sec;						      \
	(result)->tv_usec -= 1000000;					      \
      }									      \
  } while (0)
#endif
#ifndef timersub
# define timersub(a, b, result)						      \
  do {									      \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			      \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			      \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						      \
      (result)->tv_usec += 1000000;					      \
    }									      \
  } while (0)
#endif

/* static assert, for compile-time assertion checking */
#define static_assert(c) switch (c) case 0: case (c):

#ifndef HAVE_MOD_USE_COUNT
# define MOD_INC_USE_COUNT
# define MOD_DEC_USE_COUNT
# define MOD_IN_USE		0
#endif

#endif
