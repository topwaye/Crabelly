/* Machine: types.h,v 1.00 2022/06/19 */

#ifndef _SYS_TYPES_H_
#define	_SYS_TYPES_H_

/* Machine type dependent parameters. */
#include <machine/types.h>

#include <machine/ansi.h>
#include <machine/endian.h>

#if !defined(_POSIX_SOURCE) && !defined(_XOPEN_SOURCE)
typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;

typedef unsigned char	unchar;		/* Sys V compatibility */
typedef	unsigned short	ushort;		/* Sys V compatibility */
typedef	unsigned int	uint;		/* Sys V compatibility */
typedef unsigned long	ulong;		/* Sys V compatibility */
#endif

typedef	u_int64_t	u_quad_t;	/* quads */
typedef	int64_t		quad_t;
typedef	quad_t *	qaddr_t;

typedef	int64_t		blkcnt_t;	/* fs block count */
typedef	u_int32_t	blksize_t;	/* fs optimal block size */
typedef	char *		caddr_t;	/* core address */
typedef	int32_t		daddr_t;	/* disk address */
typedef	u_int32_t	dev_t;		/* device number */
typedef	u_int32_t	fixpt_t;	/* fixed point number */
typedef	u_int32_t	gid_t;		/* group id */
typedef	u_int32_t	id_t;		/* group id, process id or user id */
typedef	u_int32_t	ino_t;		/* inode number */
typedef	long		key_t;		/* IPC key (for Sys V IPC) */
typedef	u_int32_t	mode_t;		/* permissions */
typedef	u_int32_t	nlink_t;	/* link count */
typedef	quad_t		off_t;		/* file offset */
typedef	int32_t		pid_t;		/* process id */
typedef quad_t		rlim_t;		/* resource limit */
typedef	int32_t		segsz_t;	/* segment size */
typedef	int32_t		swblk_t;	/* swap offset */
typedef	u_int32_t	uid_t;		/* user id */

/*
 * These belong in unistd.h, but are placed here too to ensure that
 * long arguments will be promoted to off_t if the program fails to
 * include that header or explicitly cast them to off_t.
 */
#if !defined(_POSIX_SOURCE) && !defined(_XOPEN_SOURCE)
#ifndef _KERNEL
#include <sys/cdefs.h>
__BEGIN_DECLS
off_t	 lseek __P((int, off_t, int));
int	 ftruncate __P((int, off_t));
int	 truncate __P((const char *, off_t));
__END_DECLS
#endif /* !_KERNEL */
#endif /* !defined(_POSIX_SOURCE) ... */

#if !defined(_POSIX_SOURCE) && !defined(_XOPEN_SOURCE)
/* Major, minor numbers, dev_t's. */
#define	major(x)	((int32_t)((((x) & 0x000fff00) >>  8)))
#define	minor(x)	((int32_t)((((x) & 0xfff00000) >> 12) | \
				   (((x) & 0x000000ff) >>  0)))
#define	makedev(x,y)	((dev_t)((((x) <<  8) & 0x000fff00) | \
				 (((y) << 12) & 0xfff00000) | \
				 (((y) <<  0) & 0x000000ff)))
#endif

#ifdef	_BSD_CLOCK_T_
typedef	_BSD_CLOCK_T_		clock_t;
#undef	_BSD_CLOCK_T_
#endif

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_		size_t;
#define _SIZE_T
#undef	_BSD_SIZE_T_
#endif

#ifdef	_BSD_SSIZE_T_
typedef	_BSD_SSIZE_T_		ssize_t;
#undef	_BSD_SSIZE_T_
#endif

#ifdef	_BSD_TIME_T_
typedef	_BSD_TIME_T_		time_t;
#undef	_BSD_TIME_T_
#endif

#ifdef	_BSD_CLOCKID_T_
typedef	_BSD_CLOCKID_T_		clockid_t;
#undef	_BSD_CLOCKID_T_
#endif

#ifdef	_BSD_TIMER_T_
typedef	_BSD_TIMER_T_		timer_t;
#undef	_BSD_TIMER_T_
#endif

#ifdef	_BSD_SUSECONDS_T_
typedef	_BSD_SUSECONDS_T_	suseconds_t;
#undef	_BSD_SUSECONDS_T_
#endif

#ifdef	_BSD_USECONDS_T_
typedef	_BSD_USECONDS_T_	useconds_t;
#undef	_BSD_USECONDS_T_
#endif

#if !defined(_POSIX_SOURCE) && !defined(_XOPEN_SOURCE)
#define	NBBY	8		/* number of bits in a byte */

/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

typedef int32_t	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#ifndef howmany
#define	howmany(x, y)	(((x) + ((y) - 1)) / (y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define	FD_COPY(f, t)	(void)memcpy((t), (f), sizeof(*(f)))
#define	FD_ZERO(p)	(void)memset((p), 0, sizeof(*(p)))

#if defined(__STDC__) && defined(_KERNEL)
/*
 * Forward structure declarations for function prototypes.  We include the
 * common structures that cross subsystem boundaries here; others are mostly
 * used in the same place that the structure is defined.
 */
struct	proc;
struct	pgrp;
struct	ucred;
struct	rusage;
struct	file;
struct	buf;
struct	tty;
struct	uio;
#endif

#endif /* !defined(_POSIX_SOURCE) ... */
#endif /* !_SYS_TYPES_H_ */