#include "sqlite/_All.h"

static int posixOpen(const char *zFile, int flags, int mode);
static int openDirectory(const char *, int *);
static int unixGetpagesize(void);

static int posixOpen(const char *zFile, int flags, int mode) { return open(zFile, flags, mode); }

unix_syscall aSyscall[] = {
    {"open", (sqlite3_syscall_ptr)posixOpen, 0},

    {"close", (sqlite3_syscall_ptr)close, 0},

    {"access", (sqlite3_syscall_ptr)access, 0},

    {"getcwd", (sqlite3_syscall_ptr)getcwd, 0},

    {"stat", (sqlite3_syscall_ptr)stat, 0},

    {"fstat", (sqlite3_syscall_ptr)fstat, 0},

    {"ftruncate", (sqlite3_syscall_ptr)ftruncate, 0},

    {"fcntl", (sqlite3_syscall_ptr)fcntl, 0},

    {"read", (sqlite3_syscall_ptr)read, 0},

    {"pread", (sqlite3_syscall_ptr)0, 0},

    {"pread64", (sqlite3_syscall_ptr)pread64, 0},

    {"write", (sqlite3_syscall_ptr)write, 0},

    {"pwrite", (sqlite3_syscall_ptr)0, 0},

    {"pwrite64", (sqlite3_syscall_ptr)pwrite64, 0},

    {"fchmod", (sqlite3_syscall_ptr)fchmod, 0},

    {"fallocate", (sqlite3_syscall_ptr)0, 0},

    {"unlink", (sqlite3_syscall_ptr)unlink, 0},

    {"openDirectory", (sqlite3_syscall_ptr)openDirectory, 0},

    {"mkdir", (sqlite3_syscall_ptr)mkdir, 0},

    {"rmdir", (sqlite3_syscall_ptr)rmdir, 0},

    {"fchown", (sqlite3_syscall_ptr)fchown, 0},

    {"geteuid", (sqlite3_syscall_ptr)geteuid, 0},

    {"mmap", (sqlite3_syscall_ptr)mmap, 0},

    {"munmap", (sqlite3_syscall_ptr)munmap, 0},

    {"mremap", (sqlite3_syscall_ptr)mremap, 0},

    {"getpagesize", (sqlite3_syscall_ptr)unixGetpagesize, 0},

    {"readlink", (sqlite3_syscall_ptr)readlink, 0},

    {"lstat", (sqlite3_syscall_ptr)lstat, 0},

    {"ioctl", (sqlite3_syscall_ptr)0, 0},

};

int robustFchown(int fd, uid_t uid, gid_t gid) { return ((uid_t (*)(void))aSyscall[21].pCurrent)() ? 0 : ((int (*)(int, uid_t, gid_t))aSyscall[20].pCurrent)(fd, uid, gid); }

int robust_open(const char *z, int f, mode_t m) {
  int fd;
  mode_t m2 = m ? m : 0644;
  while (1) {

    fd = ((int (*)(const char *, int, int))aSyscall[0].pCurrent)(z,
                                                                 f |

                                                                     02000000

                                                                 ,
                                                                 m2);

    if (fd < 0) {
      if (

          (*__errno_location())

          ==

          4

      )
        continue;
      break;
    }
    if (fd >= 3)
      break;
    if ((f & (

                 0200

                 |

                 0100

                 )) ==
        (

            0200

            |

            0100

            )) {
      (void)((int (*)(const char *))aSyscall[16].pCurrent)(z);
    }
    ((int (*)(int))aSyscall[1].pCurrent)(fd);
    sqlite3_log(28, "attempt to open \"%s\" as file descriptor %d", z, fd);
    fd = -1;
    if (((int (*)(const char *, int, int))aSyscall[0].pCurrent)("/dev/null",

                                                                00

                                                                ,
                                                                m) < 0)
      break;
  }
  if (fd >= 0) {
    if (m != 0) {
      struct stat statbuf;
      if (((int (*)(int, struct stat *))aSyscall[5].pCurrent)(fd, &statbuf) == 0 && statbuf.st_size == 0 && (statbuf.st_mode & 0777) != m) {
        ((int (*)(int, mode_t))aSyscall[14].pCurrent)(fd, m);
      }
    }
  }
  return fd;
}

void unixEnterMutex(void) { sqlite3_mutex_enter(unixBigLock); }

void unixLeaveMutex(void) { sqlite3_mutex_leave(unixBigLock); }

int robust_ftruncate(int h, sqlite3_int64 sz) {
  int rc;

  do {
    rc = ((int (*)(int, off_t))aSyscall[6].pCurrent)(h, sz);
  } while (rc < 0 &&

           (*__errno_location())

               ==

               4

  );
  return rc;
}

int unixLogErrorAtLine(int errcode, const char *zFunc, const char *zPath, int iLine) {
  char *zErr;
  int iErrno =

      (*__errno_location())

      ;

  zErr = "";

  if (zPath == 0)
    zPath = "";
  sqlite3_log(errcode, "os_unix.c:%d: (%d) %s(%s) - %s", iLine, iErrno, zFunc, zPath, zErr);

  return errcode;
}

int seekAndWriteFd(int fd, i64 iOff, const void *pBuf, int nBuf, int *piErrno) {
  int rc = 0;

  nBuf &= 0x1ffff;
  ;

  do {
    rc = (int)((ssize_t (*)(int, const void *, size_t, off64_t))aSyscall[13].pCurrent)(fd, pBuf, nBuf, iOff);
  } while (rc < 0 &&

           (*__errno_location())

               ==

               4

  );

  ;
  ;

  if (rc < 0)
    *piErrno =

        (*__errno_location())

        ;
  return rc;
}

int full_fsync(int fd, int fullSync, int dataOnly) {
  int rc;

  (void)(fullSync);
  (void)(dataOnly);

  rc = fdatasync(fd);

  if (0 && rc != -1) {
    rc = 0;
  }
  return rc;
}

static int openDirectory(const char *zFilename, int *pFd) {
  int ii;
  int fd = -1;
  char zDirname[512 + 1];

  sqlite3_snprintf(512, zDirname, "%s", zFilename);
  for (ii = (int)strlen(zDirname); ii > 0 && zDirname[ii] != '/'; ii--)
    ;
  if (ii > 0) {
    zDirname[ii] = '\0';
  } else {
    if (zDirname[0] != '/')
      zDirname[0] = '.';
    zDirname[1] = 0;
  }
  fd = robust_open(zDirname,

                   00

                       | 0,
                   0);
  if (fd >= 0) {
    ;
  }
  *pFd = fd;
  if (fd >= 0)
    return 0;
  return unixLogErrorAtLine(sqlite3CantopenError(44090), "openDirectory", zDirname, 44090);
}

static int unixGetpagesize(void) {

  return (int)sysconf(

      _SC_PAGESIZE

  );
}

static const char *unixTempFileDir(void) {
  unsigned int i = 0;
  struct stat buf;
  const char *zDir = sqlite3_temp_directory;

  while (1) {
    if (zDir != 0

        && ((int (*)(const char *, struct stat *))aSyscall[4].pCurrent)(zDir, &buf) == 0 &&

        ((((

              buf.st_mode

              )) &
          0170000) == (0040000))

        && ((int (*)(const char *, int))aSyscall[2].pCurrent)(zDir, 03) == 0) {
      return zDir;
    }
    if (i >= sizeof(azTempDirs) / sizeof(azTempDirs[0]))
      break;
    zDir = azTempDirs[i++];
  }
  return 0;
}

int unixGetTempname(int nBuf, char *zBuf) {
  const char *zDir;
  int iLimit = 0;
  int rc = 0;

  zBuf[0] = 0;
  ;

  sqlite3_mutex_enter(sqlite3MutexAlloc(11));
  zDir = unixTempFileDir();
  if (zDir == 0) {
    rc = (10 | (25 << 8));
  } else {
    do {
      u64 r;
      sqlite3_randomness(sizeof(r), &r);

      ((void)(0))

          ;
      zBuf[nBuf - 2] = 0;
      sqlite3_snprintf(nBuf, zBuf,
                       "%s/"
                       "etilqs_"
                       "%llx%c",
                       zDir, r, 0);
      if (zBuf[nBuf - 2] != 0 || (iLimit++) > 10) {
        rc = 1;
        break;
      }
    } while (((int (*)(const char *, int))aSyscall[2].pCurrent)(zBuf, 0) == 0);
  }
  sqlite3_mutex_leave(sqlite3MutexAlloc(11));
  return rc;
}

unix_syscall unix_syscall_stub;