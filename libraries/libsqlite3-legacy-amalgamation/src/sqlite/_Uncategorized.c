#include "sqlite/_All.h"

const char *const sqlite3azCompileOpt[] = {

    "ATOMIC_INTRINSICS="
    "1",

    "COMPILER=gcc-"
    "16.2.1 20260810",

    "DEFAULT_AUTOVACUUM",

    "DEFAULT_CACHE_SIZE="
    "-2000",

    "DEFAULT_FILE_FORMAT="
    "4",

    "DEFAULT_JOURNAL_SIZE_LIMIT="
    "-1",

    "DEFAULT_MMAP_SIZE="
    "0",

    "DEFAULT_PAGE_SIZE="
    "4096",

    "DEFAULT_PCACHE_INITSZ="
    "20",

    "DEFAULT_RECURSIVE_TRIGGERS",

    "DEFAULT_SECTOR_SIZE="
    "4096",

    "DEFAULT_SYNCHRONOUS="
    "2",

    "DEFAULT_WAL_AUTOCHECKPOINT="
    "1000",

    "DEFAULT_WAL_SYNCHRONOUS="
    "2",

    "DEFAULT_WORKER_THREADS="
    "0",

    "DIRECT_OVERFLOW_READ",

    "ENABLE_MATH_FUNCTIONS",

    "ENABLE_PERCENTILE",

    "HAVE_ISNAN",

    "MALLOC_SOFT_LIMIT="
    "1024",

    "MAX_ATTACHED="
    "10",

    "MAX_COLUMN="
    "2000",

    "MAX_COMPOUND_SELECT="
    "500",

    "MAX_DEFAULT_PAGE_SIZE="
    "8192",

    "MAX_EXPR_DEPTH="
    "1000",

    "MAX_FUNCTION_ARG="
    "1000",

    "MAX_LENGTH="
    "1000000000",

    "MAX_LIKE_PATTERN_LENGTH="
    "50000",

    "MAX_MMAP_SIZE="
    "0x7fff0000",

    "MAX_PAGE_COUNT="
    "0xfffffffe",

    "MAX_PAGE_SIZE="
    "65536",

    "MAX_SQL_LENGTH="
    "1000000000",

    "MAX_TRIGGER_DEPTH="
    "1000",

    "MAX_VARIABLE_NUMBER="
    "32766",

    "MAX_VDBE_OP="
    "250000000",

    "MAX_WORKER_THREADS="
    "8",

    "MUTEX_PTHREADS",

    "SYSTEM_MALLOC",

    "TEMP_STORE="
    "1",

    "THREADSAFE="
    "1",

};

const unsigned char *sqlite3aLTb = &sqlite3UpperToLower[256 - 53];

const unsigned char *sqlite3aEQb = &sqlite3UpperToLower[256 + 6 - 53];

const unsigned char *sqlite3aGTb = &sqlite3UpperToLower[256 + 12 - 53];

const char statMutex[] = {
    0, 1, 1, 0, 0, 0, 0, 1, 0, 0,
};

int getDigits(const char *zDate, const char *zFormat, ...) {

  static const u16 aMx[] = {12, 14, 24, 31, 59, 14712};
  va_list ap;
  int cnt = 0;
  char nextC;

  va_start(

      ap, zFormat

  )

      ;
  do {
    char N = zFormat[0] - '0';
    char min = zFormat[1] - '0';
    int val = 0;
    u16 max;

    ((void)(0))

        ;
    max = aMx[zFormat[2] - 'a'];
    nextC = zFormat[3];
    val = 0;
    while (N--) {
      if (!(sqlite3CtypeMap[(unsigned char)(*zDate)] & 0x04)) {
        goto end_getDigits;
      }
      val = val * 10 + *zDate - '0';
      zDate++;
    }
    if (val < (int)min || val > (int)max || (nextC != 0 && nextC != *zDate)) {
      goto end_getDigits;
    }
    *

        va_arg(

            ap

            ,

            int *

            )

        = val;
    zDate++;
    cnt++;
    zFormat += 4;
  } while (nextC);
end_getDigits:

  va_end(

      ap

  )

      ;
  return cnt;
}

int validJulianDay(sqlite3_int64 iJD) { return iJD >= 0 && iJD <= ((((i64)0x1a640) << 32) | 0x1072fdff); }

int osLocaltime(time_t *t, struct tm *pTm) {
  int rc;

  if (sqlite3Config.bLocaltimeFault) {
    if (sqlite3Config.xAltLocaltime != 0) {
      return sqlite3Config.xAltLocaltime((const void *)t, (void *)pTm);
    } else {
      return 1;
    }
  }

  rc = localtime_r(t, pTm) == 0;

  return rc;
}

const struct aXformType_t {
  u8 nName;
  char zName[7];
  float rLimit;
  float rXform;
} aXformType[] = {
    {6, "second", 4.6427e+14, 1.0}, {6, "minute", 7.7379e+12, 60.0}, {4, "hour", 1.2897e+11, 3600.0}, {3, "day", 5373485.0, 86400.0}, {5, "month", 176546.0, 2592000.0}, {4, "year", 14713.0, 31536000.0},
};

int noopMutexInit(void) { return 0; }

int noopMutexEnd(void) { return 0; }

sqlite3_mutex *noopMutexAlloc(int id) {
  (void)(id);
  return (sqlite3_mutex *)8;
}

int pthreadMutexInit(void) { return 0; }

int pthreadMutexEnd(void) { return 0; }

sqlite3_mutex *pthreadMutexAlloc(int iType) {
  static sqlite3_mutex staticMutexes[] = {{

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          },
                                          {

                                              {{0, 0, 0, 0, PTHREAD_MUTEX_TIMED_NP, 0, 0, {((void *)0), ((void *)0)}}}

                                          }};
  sqlite3_mutex *p;
  switch (iType) {
  case 1: {
    p = sqlite3MallocZero(sizeof(*p));
    if (p) {

      pthread_mutexattr_t recursiveAttr;
      pthread_mutexattr_init(&recursiveAttr);
      pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
      pthread_mutex_init(&p->mutex, &recursiveAttr);
      pthread_mutexattr_destroy(&recursiveAttr);
    }
    break;
  }
  case 0: {
    p = sqlite3MallocZero(sizeof(*p));
    if (p) {
      pthread_mutex_init(&p->mutex, 0);
    }
    break;
  }
  default: {

    p = &staticMutexes[iType - 2];
    break;
  }
  }

  return p;
}

void mallocWithAlarm(int n, void **pp) {
  void *p;
  int nFull;

  nFull = sqlite3Config.m.xRoundup(n);

  sqlite3StatusHighwater(5, n);
  if (mem0.alarmThreshold > 0) {
    sqlite3_int64 nUsed = sqlite3StatusValue(0);
    if (nUsed >= mem0.alarmThreshold - nFull) {
      __atomic_store_n((&mem0.nearlyFull), (1), 0);
      sqlite3MallocAlarm(nFull);
      if (mem0.hardLimit) {
        nUsed = sqlite3StatusValue(0);
        if (nUsed >= mem0.hardLimit - nFull) {
          ;
          *pp = 0;
          return;
        }
      }
    } else {
      __atomic_store_n((&mem0.nearlyFull), (0), 0);
    }
  }
  p = sqlite3Config.m.xMalloc(nFull);

  if (p) {
    nFull = sqlite3MallocSize(p);
    sqlite3StatusUp(0, nFull);
    sqlite3StatusUp(9, 1);
  }
  *pp = p;
}

const char aDigits[] = "0123456789ABCDEF0123456789abcdef";

const char aPrefix[] = "-x0\000X0";

const et_info fmtinfo[23] = {{'s', 0, 4, 5, 0, 0, 1}, {'E', 0, 1, 2, 14, 0, 0}, {'u', 10, 0, 16, 0, 0, 3}, {'G', 0, 1, 3, 14, 0, 0}, {'w', 0, 4, 14, 0, 0, 0}, {'x', 16, 0, 0, 16, 1, 0}, {'c', 0, 0, 8, 0, 0, 0}, {'z', 0, 4, 6, 0, 0, 6}, {'d', 10, 1, 16, 0, 0, 0}, {'e', 0, 1, 2, 30, 0, 0}, {'f', 0, 1, 1, 0, 0, 0}, {'g', 0, 1, 3, 30, 0, 0}, {'Q', 0, 4, 10, 0, 0, 0}, {'i', 10, 1, 16, 0, 0, 0}, {'%', 0, 0, 7, 0, 0, 16}, {'T', 0, 0, 11, 0, 0, 0}, {'S', 0, 0, 12, 0, 0, 0}, {'X', 16, 0, 0, 0, 4, 0}, {'n', 0, 0, 4, 0, 0, 0}, {'o', 8, 0, 0, 0, 2, 17}, {'p', 16, 0, 13, 0, 1, 0}, {'q', 0, 4, 9, 0, 0, 0}, {'r', 10, 1, 15, 0, 0, 0}};

void renderLogMsg(int iErrCode, const char *zFormat, va_list ap) {
  StrAccum acc;
  char zMsg[(70 * 10)];

  sqlite3StrAccumInit(&acc, 0, zMsg, sizeof(zMsg), 0);
  sqlite3_str_vappendf(&acc, zFormat, ap);
  sqlite3Config.xLog(sqlite3Config.pLogArg, iErrCode, sqlite3StrAccumFinish(&acc));
}

void chacha_block(u32 *out, const u32 *in) {
  int i;
  u32 x[16];
  memcpy(x, in, 64);
  for (i = 0; i < 10; i++) {
    (x[0] += x[4], x[12] ^= x[0], x[12] = (((x[12]) << (16)) | ((x[12]) >> (32 - (16)))), x[8] += x[12], x[4] ^= x[8], x[4] = (((x[4]) << (12)) | ((x[4]) >> (32 - (12)))), x[0] += x[4], x[12] ^= x[0], x[12] = (((x[12]) << (8)) | ((x[12]) >> (32 - (8)))), x[8] += x[12], x[4] ^= x[8], x[4] = (((x[4]) << (7)) | ((x[4]) >> (32 - (7)))));
    (x[1] += x[5], x[13] ^= x[1], x[13] = (((x[13]) << (16)) | ((x[13]) >> (32 - (16)))), x[9] += x[13], x[5] ^= x[9], x[5] = (((x[5]) << (12)) | ((x[5]) >> (32 - (12)))), x[1] += x[5], x[13] ^= x[1], x[13] = (((x[13]) << (8)) | ((x[13]) >> (32 - (8)))), x[9] += x[13], x[5] ^= x[9], x[5] = (((x[5]) << (7)) | ((x[5]) >> (32 - (7)))));
    (x[2] += x[6], x[14] ^= x[2], x[14] = (((x[14]) << (16)) | ((x[14]) >> (32 - (16)))), x[10] += x[14], x[6] ^= x[10], x[6] = (((x[6]) << (12)) | ((x[6]) >> (32 - (12)))), x[2] += x[6], x[14] ^= x[2], x[14] = (((x[14]) << (8)) | ((x[14]) >> (32 - (8)))), x[10] += x[14], x[6] ^= x[10], x[6] = (((x[6]) << (7)) | ((x[6]) >> (32 - (7)))));
    (x[3] += x[7], x[15] ^= x[3], x[15] = (((x[15]) << (16)) | ((x[15]) >> (32 - (16)))), x[11] += x[15], x[7] ^= x[11], x[7] = (((x[7]) << (12)) | ((x[7]) >> (32 - (12)))), x[3] += x[7], x[15] ^= x[3], x[15] = (((x[15]) << (8)) | ((x[15]) >> (32 - (8)))), x[11] += x[15], x[7] ^= x[11], x[7] = (((x[7]) << (7)) | ((x[7]) >> (32 - (7)))));
    (x[0] += x[5], x[15] ^= x[0], x[15] = (((x[15]) << (16)) | ((x[15]) >> (32 - (16)))), x[10] += x[15], x[5] ^= x[10], x[5] = (((x[5]) << (12)) | ((x[5]) >> (32 - (12)))), x[0] += x[5], x[15] ^= x[0], x[15] = (((x[15]) << (8)) | ((x[15]) >> (32 - (8)))), x[10] += x[15], x[5] ^= x[10], x[5] = (((x[5]) << (7)) | ((x[5]) >> (32 - (7)))));
    (x[1] += x[6], x[12] ^= x[1], x[12] = (((x[12]) << (16)) | ((x[12]) >> (32 - (16)))), x[11] += x[12], x[6] ^= x[11], x[6] = (((x[6]) << (12)) | ((x[6]) >> (32 - (12)))), x[1] += x[6], x[12] ^= x[1], x[12] = (((x[12]) << (8)) | ((x[12]) >> (32 - (8)))), x[11] += x[12], x[6] ^= x[11], x[6] = (((x[6]) << (7)) | ((x[6]) >> (32 - (7)))));
    (x[2] += x[7], x[13] ^= x[2], x[13] = (((x[13]) << (16)) | ((x[13]) >> (32 - (16)))), x[8] += x[13], x[7] ^= x[8], x[7] = (((x[7]) << (12)) | ((x[7]) >> (32 - (12)))), x[2] += x[7], x[13] ^= x[2], x[13] = (((x[13]) << (8)) | ((x[13]) >> (32 - (8)))), x[8] += x[13], x[7] ^= x[8], x[7] = (((x[7]) << (7)) | ((x[7]) >> (32 - (7)))));
    (x[3] += x[4], x[14] ^= x[3], x[14] = (((x[14]) << (16)) | ((x[14]) >> (32 - (16)))), x[9] += x[14], x[4] ^= x[9], x[4] = (((x[4]) << (12)) | ((x[4]) >> (32 - (12)))), x[3] += x[4], x[14] ^= x[3], x[14] = (((x[14]) << (8)) | ((x[14]) >> (32 - (8)))), x[9] += x[14], x[4] ^= x[9], x[4] = (((x[4]) << (7)) | ((x[4]) >> (32 - (7)))));
  }
  for (i = 0; i < 16; i++)
    out[i] = x[i] + in[i];
}

u64 powerOfTen(int p, u32 *pLo) {
  static const u64 aBase[] = {
      0x8000000000000000UL, 0xa000000000000000UL, 0xc800000000000000UL, 0xfa00000000000000UL, 0x9c40000000000000UL, 0xc350000000000000UL, 0xf424000000000000UL, 0x9896800000000000UL, 0xbebc200000000000UL, 0xee6b280000000000UL, 0x9502f90000000000UL, 0xba43b74000000000UL, 0xe8d4a51000000000UL, 0x9184e72a00000000UL, 0xb5e620f480000000UL, 0xe35fa931a0000000UL, 0x8e1bc9bf04000000UL, 0xb1a2bc2ec5000000UL, 0xde0b6b3a76400000UL, 0x8ac7230489e80000UL, 0xad78ebc5ac620000UL, 0xd8d726b7177a8000UL, 0x878678326eac9000UL, 0xa968163f0a57b400UL, 0xd3c21bcecceda100UL, 0x84595161401484a0UL, 0xa56fa5b99019a5c8UL,
  };
  static const u64 aScale[] = {
      0x8049a4ac0c5811aeUL, 0xcf42894a5dce35eaUL, 0xa76c582338ed2621UL, 0x873e4f75e2224e68UL, 0xda7f5bf590966848UL, 0xb080392cc4349decUL, 0x8e938662882af53eUL, 0xe65829b3046b0afaUL, 0xba121a4650e4ddebUL, 0x964e858c91ba2655UL, 0xf2d56790ab41c2a2UL, 0xc428d05aa4751e4cUL, 0x9e74d1b791e07e48UL, 0xccccccccccccccccUL, 0xcecb8f27f4200f3aUL, 0xa70c3c40a64e6c51UL, 0x86f0ac99b4e8dafdUL, 0xda01ee641a708de9UL, 0xb01ae745b101e9e4UL, 0x8e41ade9fbebc27dUL, 0xe5d3ef282a242e81UL, 0xb9a74a0637ce2ee1UL, 0x95f83d0a1fb69cd9UL, 0xf24a01a73cf2dccfUL, 0xc3b8358109e84f07UL, 0x9e19db92b4e31ba9UL,
  };
  static const unsigned int aScaleLo[] = {
      0x205b896d, 0x52064cad, 0xaf2af2b8, 0x5a7744a7, 0xaf39a475, 0xbd8d794e, 0x547eb47b, 0x0cb4a5a3, 0x92f34d62, 0x3a6a07f9, 0xfae27299, 0xaa97e14c, 0x775ea265, 0xcccccccc, 0x00000000, 0x999090b6, 0x69a028bb, 0xe80e6f48, 0x5ec05dd0, 0x14588f14, 0x8f1668c9, 0x6d953e2c, 0x4abdaf10, 0xbc633b39, 0x0a862f81, 0x6c07a2c2,
  };
  int g, n;
  u64 s, x;
  u32 lo;

  if (p < 0) {
    if (p == (-1)) {
      *pLo = aScaleLo[13];
      return aScale[13];
    }
    g = p / 27;
    n = p % 27;
    if (n) {
      g--;
      n += 27;
    }
  } else if (p < 27) {
    *pLo = 0;
    return aBase[p];
  } else {
    g = p / 27;
    n = p % 27;
  }
  s = aScale[g + 13];
  if (n == 0) {
    *pLo = aScaleLo[g + 13];
    return s;
  }
  x = sqlite3Multiply160(s, aScaleLo[g + 13], aBase[n], &lo);
  if (((((u64)1) << (63)) & x) == 0) {
    x = x << 1 | ((lo >> 31) & 1);
    lo = (lo << 1) | 1;
  }
  *pLo = lo;
  return x;
}

int pwr10to2(int p) { return (p * 108853) >> 15; }

int pwr2to10(int p) { return (p * 78913) >> 18; }

int countLeadingZeros(u64 m) { return __builtin_clzll(m); }

int compare2pow63(const char *zNum, int incr) {
  int c = 0;
  int i;

  const char *pow63 = "922337203685477580";
  for (i = 0; c == 0 && i < 18; i++) {
    c = (zNum[i * incr] - pow63[i]) * 10;
  }
  if (c == 0) {
    c = zNum[18 * incr] - '8';
    ;
    ;
    ;
  }
  return c;
}

int __attribute__((noinline)) putVarint64(unsigned char *p, u64 v) {
  int i, j, n;
  u8 buf[10];
  if (v & (((u64)0xff000000) << 32)) {
    p[8] = (u8)v;
    v >>= 8;
    for (i = 7; i >= 0; i--) {
      p[i] = (u8)((v & 0x7f) | 0x80);
      v >>= 7;
    }
    return 9;
  }
  n = 0;
  do {
    buf[n++] = (u8)((v & 0x7f) | 0x80);
    v >>= 7;
  } while (v != 0);
  buf[0] &= 0x7f;

  for (i = 0, j = n - 1; j >= 0; j--, i++) {
    p[i] = buf[j];
  }
  return n;
}

void logBadConnection(const char *zType) { sqlite3_log(21, "API call with %s database connection pointer", zType); }

unsigned int strHash(const char *z) {
  unsigned int h = 0;
  while (z[0]) {

    h += 0xdf & (unsigned char)*(z++);

    h *= 0x9e3779b1;
  }
  return h;
}

pid_t randomnessPid = 0;

int posixOpen(const char *zFile, int flags, int mode) { return open(zFile, flags, mode); }

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

int sqliteErrorFromPosixError(int posixError, int sqliteIOErr) {

  switch (posixError) {
  case

      13

      :
  case

      11

      :
  case

      110

      :
  case

      16

      :
  case

      4

      :
  case

      37

      :

    return 5;

  case

      1

      :
    return 3;

  default:
    return sqliteIOErr;
  }
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

int openDirectory(const char *zFilename, int *pFd) {
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

int unixGetpagesize(void) {

  return (int)sysconf(

      _SC_PAGESIZE

  );
}

const sqlite3_io_methods *posixIoFinderImpl(const char *z, unixFile *p) {
  (void)(z);
  (void)(p);
  return &posixIoMethods;
}

const sqlite3_io_methods *(*const posixIoFinder)(const char *, unixFile *p) = posixIoFinderImpl;

const sqlite3_io_methods *nolockIoFinderImpl(const char *z, unixFile *p) {
  (void)(z);
  (void)(p);
  return &nolockIoMethods;
}

const sqlite3_io_methods *(*const nolockIoFinder)(const char *, unixFile *p) = nolockIoFinderImpl;

const sqlite3_io_methods *dotlockIoFinderImpl(const char *z, unixFile *p) {
  (void)(z);
  (void)(p);
  return &dotlockIoMethods;
}

const sqlite3_io_methods *(*const dotlockIoFinder)(const char *, unixFile *p) = dotlockIoFinderImpl;

const char *azTempDirs[] = {0, 0, "/var/tmp", "/usr/tmp", "/tmp", "."};

void unixTempFileInit(void) {
  azTempDirs[0] = getenv("SQLITE_TMPDIR");
  azTempDirs[1] = getenv("TMPDIR");
}

const char *unixTempFileDir(void) {
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

UnixUnusedFd *findReusableFd(const char *zPath, int flags) {
  UnixUnusedFd *pUnused = 0;

  struct stat sStat;

  unixEnterMutex();

  if (inodeList != 0 && 0 == ((int (*)(const char *, struct stat *))aSyscall[4].pCurrent)(zPath, &sStat)) {
    unixInodeInfo *pInode;

    pInode = inodeList;
    while (pInode && (pInode->fileId.dev != sStat.st_dev || pInode->fileId.ino != (u64)sStat.st_ino)) {
      pInode = pInode->pNext;
    }
    if (pInode) {
      UnixUnusedFd **pp;

      ((void)(0))

          ;
      sqlite3_mutex_enter(pInode->pLockMutex);
      flags &= (0x00000001 | 0x00000002);
      for (pp = &pInode->pUnused; *pp && (*pp)->flags != flags; pp = &((*pp)->pNext))
        ;
      pUnused = *pp;
      if (pUnused) {
        *pp = pUnused->pNext;
      }
      sqlite3_mutex_leave(pInode->pLockMutex);
    }
  }
  unixLeaveMutex();

  return pUnused;
}

int getFileMode(const char *zFile, mode_t *pMode, uid_t *pUid, gid_t *pGid) {
  struct stat sStat;
  int rc = 0;
  if (0 == ((int (*)(const char *, struct stat *))aSyscall[4].pCurrent)(zFile, &sStat)) {
    *pMode = sStat.st_mode & 0777;
    *pUid = sStat.st_uid;
    *pGid = sStat.st_gid;
  } else {
    rc = (10 | (7 << 8));
  }
  return rc;
}

int findCreateFileMode(const char *zPath, int flags, mode_t *pMode, uid_t *pUid, gid_t *pGid) {
  int rc = 0;
  *pMode = 0;
  *pUid = 0;
  *pGid = 0;
  if (flags & (0x00080000 | 0x00000800)) {
    char zDb[512 + 1];
    int nDb;

    nDb = sqlite3Strlen30(zPath) - 1;
    while (nDb > 0 && zPath[nDb] != '.') {
      if (zPath[nDb] == '-') {
        memcpy(zDb, zPath, nDb);
        zDb[nDb] = '\0';
        rc = getFileMode(zDb, pMode, pUid, pGid);
        break;
      }
      nDb--;
    }
  } else if (flags & 0x00000008) {
    *pMode = 0600;
  } else if (flags & 0x00000040) {

    const char *z = sqlite3_uri_parameter(zPath, "modeof");
    if (z) {
      rc = getFileMode(z, pMode, pUid, pGid);
    }
  }
  return rc;
}

void *pcache1Alloc(int nByte) {
  void *p = 0;

  if (nByte <= (pcache1_g).szSlot) {
    sqlite3_mutex_enter((pcache1_g).mutex);
    p = (PgHdr1 *)(pcache1_g).pFree;
    if (p) {
      (pcache1_g).pFree = (pcache1_g).pFree->pNext;
      (pcache1_g).nFreeSlot--;
      __atomic_store_n((&(pcache1_g).bUnderPressure), ((pcache1_g).nFreeSlot < (pcache1_g).nReserve), 0);

      ((void)(0))

          ;
      sqlite3StatusHighwater(7, nByte);
      sqlite3StatusUp(1, 1);
    }
    sqlite3_mutex_leave((pcache1_g).mutex);
  }
  if (p == 0) {

    p = sqlite3Malloc(nByte);

    if (p) {
      int sz = sqlite3MallocSize(p);
      sqlite3_mutex_enter((pcache1_g).mutex);
      sqlite3StatusHighwater(7, nByte);
      sqlite3StatusUp(2, sz);
      sqlite3_mutex_leave((pcache1_g).mutex);
    }

    ;
  }
  return p;
}

void pcache1Free(void *p) {
  if (p == 0)
    return;
  if ((((uptr)(p) >= (uptr)((pcache1_g).pStart)) && ((uptr)(p) < (uptr)((pcache1_g).pEnd)))) {
    PgFreeslot *pSlot;
    sqlite3_mutex_enter((pcache1_g).mutex);
    sqlite3StatusDown(1, 1);
    pSlot = (PgFreeslot *)p;
    pSlot->pNext = (pcache1_g).pFree;
    (pcache1_g).pFree = pSlot;
    (pcache1_g).nFreeSlot++;
    __atomic_store_n((&(pcache1_g).bUnderPressure), ((pcache1_g).nFreeSlot < (pcache1_g).nReserve), 0);

    ((void)(0))

        ;
    sqlite3_mutex_leave((pcache1_g).mutex);
  } else {

    ((void)(0))

        ;
    ;

    {
      int nFreed = 0;
      nFreed = sqlite3MallocSize(p);
      sqlite3_mutex_enter((pcache1_g).mutex);
      sqlite3StatusDown(2, nFreed);
      sqlite3_mutex_leave((pcache1_g).mutex);
    }

    sqlite3_free(p);
  }
}

int pcache1Init(void *NotUsed) {
  (void)(NotUsed);

  memset(&(pcache1_g), 0, sizeof((pcache1_g)));

  (pcache1_g).separateCache = sqlite3Config.pPage == 0 || sqlite3Config.bCoreMutex > 0;

  if (sqlite3Config.bCoreMutex) {
    (pcache1_g).grp.mutex = sqlite3MutexAlloc(6);
    (pcache1_g).mutex = sqlite3MutexAlloc(7);
  }

  if ((pcache1_g).separateCache && sqlite3Config.nPage != 0 && sqlite3Config.pPage == 0) {
    (pcache1_g).nInitPage = sqlite3Config.nPage;
  } else {
    (pcache1_g).nInitPage = 0;
  }
  (pcache1_g).grp.mxPinned = 10;
  (pcache1_g).isInit = 1;
  return 0;
}

void pcache1Shutdown(void *NotUsed) {
  (void)(NotUsed);

  memset(&(pcache1_g), 0, sizeof((pcache1_g)));
}

sqlite3_pcache *pcache1Create(int szPage, int szExtra, int bPurgeable) {
  PCache1 *pCache;
  PGroup *pGroup;
  i64 sz;

  sz = sizeof(PCache1) + sizeof(PGroup) * (pcache1_g).separateCache;
  pCache = (PCache1 *)sqlite3MallocZero(sz);
  if (pCache) {
    if ((pcache1_g).separateCache) {
      pGroup = (PGroup *)&pCache[1];
      pGroup->mxPinned = 10;
    } else {
      pGroup = &(pcache1_g).grp;
    }

    ((void)(0))

        ;
    if (pGroup->lru.isAnchor == 0) {
      pGroup->lru.isAnchor = 1;
      pGroup->lru.pLruPrev = pGroup->lru.pLruNext = &pGroup->lru;
    }
    pCache->pGroup = pGroup;
    pCache->szPage = szPage;
    pCache->szExtra = szExtra;
    pCache->szAlloc = szPage + szExtra + (((sizeof(PgHdr1)) + 7) & ~7);
    pCache->bPurgeable = (bPurgeable ? 1 : 0);
    pcache1ResizeHash(pCache);
    if (bPurgeable) {
      pCache->nMin = 10;
      pGroup->nMinPage += pCache->nMin;
      pGroup->mxPinned = pGroup->nMaxPage + 10 - pGroup->nMinPage;
      pCache->pnPurgeable = &pGroup->nPurgeable;
    } else {
      pCache->pnPurgeable = &pCache->nPurgeableDummy;
    }

    ((void)(0))

        ;
    if (pCache->nHash == 0) {
      pcache1Destroy((sqlite3_pcache *)pCache);
      pCache = 0;
    }
  }
  return (sqlite3_pcache *)pCache;
}

const unsigned char aJournalMagic[] = {
    0xd9, 0xd5, 0x05, 0xf9, 0x20, 0xa1, 0x63, 0xd7,
};

void freeSuperJournal(char *zSuper) {
  if (zSuper) {
    sqlite3_free(&zSuper[-4]);
  }
}

const char zMagicHeader[] = "SQLite format 3";

int copyPayload(void *pPayload, void *pBuf, int nByte, int eOp, DbPage *pDbPage) {
  if (eOp) {

    int rc = sqlite3PagerWrite(pDbPage);
    if (rc != 0) {
      return rc;
    }
    memcpy(pPayload, pBuf, nByte);
  } else {

    memcpy(pBuf, pPayload, nByte);
  }
  return 0;
}

int isFatalError(int rc) { return (rc != 0 && rc != 5 && (rc != 6)); }

void serialGet(const unsigned char *buf, u32 serial_type, Mem *pMem) {
  u64 x = (((u32)(buf)[0] << 24) | ((buf)[1] << 16) | ((buf)[2] << 8) | (buf)[3]);
  u32 y = (((u32)(buf + 4)[0] << 24) | ((buf + 4)[1] << 16) | ((buf + 4)[2] << 8) | (buf + 4)[3]);
  x = (x << 32) + y;
  if (serial_type == 6) {

    pMem->u.i = *(i64 *)&x;
    pMem->flags = 0x0004;
    ;
  } else {

    ((void)(0))

        ;
    ;
    memcpy(&pMem->u.r, &x, sizeof(x));
    pMem->flags = (((x) & (((u64)0x7ff) << 52)) == (((u64)0x7ff) << 52) && ((x) & ((((u64)1) << 52) - 1)) != 0) ? 0x0001 : 0x0008;
  }
}

int serialGet7(const unsigned char *buf, Mem *pMem) {
  u64 x = (((u32)(buf)[0] << 24) | ((buf)[1] << 16) | ((buf)[2] << 8) | (buf)[3]);
  u32 y = (((u32)(buf + 4)[0] << 24) | ((buf + 4)[1] << 16) | ((buf + 4)[2] << 8) | (buf + 4)[3]);
  x = (x << 32) + y;

  ;
  memcpy(&pMem->u.r, &x, sizeof(x));
  if ((((x) & (((u64)0x7ff) << 52)) == (((u64)0x7ff) << 52) && ((x) & ((((u64)1) << 52) - 1)) != 0)) {
    pMem->flags = 0x0001;
    return 1;
  }
  pMem->flags = 0x0008;
  return 0;
}

int isAllZero(const char *z, int n) {
  int i;
  for (i = 0; i < n; i++) {
    if (z[i])
      return 0;
  }
  return 1;
}

int invokeValueDestructor(const void *p, void (*xDel)(void *), sqlite3_context *pCtx) {

  if (xDel == 0) {

  } else if (xDel == ((sqlite3_destructor_type)-1)) {

  } else {
    xDel((void *)p);
  }

  sqlite3_result_error_toobig(pCtx);

  return 18;
}

const char *const azExplainColNames8[] = {"addr", "opcode", "p1", "p2", "p3", "p4", "p5", "comment", "id", "parent", "notused", "detail"};

const u16 azExplainColNames16data[] = {'a', 'd', 'd', 'r', 0, 'o', 'p', 'c', 'o', 'd', 'e', 0, 'p', '1', 0, 'p', '2', 0, 'p', '3', 0, 'p', '4', 0, 'p', '5', 0, 'c', 'o', 'm', 'm', 'e', 'n', 't', 0, 'i', 'd', 0, 'p', 'a', 'r', 'e', 'n', 't', 0, 'n', 'o', 't', 'u', 's', 'e', 'd', 0, 'd', 'e', 't', 'a', 'i', 'l', 0};

const u8 iExplainColNames16[] = {0, 5, 12, 15, 18, 21, 24, 27, 35, 38, 45, 53};

i64 findNextHostParameter(const char *zSql, i64 *pnToken) {
  int tokenType;
  i64 nTotal = 0;
  i64 n;

  *pnToken = 0;
  while (zSql[0]) {
    n = sqlite3GetToken((u8 *)zSql, &tokenType);

    ((void)(0))

        ;
    if (tokenType == 157) {
      *pnToken = n;
      break;
    }
    nTotal += n;
    zSql += n;
  }
  return nTotal;
}

__attribute__((noinline)) int isValidSchemaTableName(const char *zTab, Table *pTab, const char *zDb) {
  const char *zLegacy;

  if (sqlite3_strnicmp(zTab, "sqlite_", 7) != 0)
    return 0;
  zLegacy = pTab->zName;
  if (strcmp(zLegacy + 7, &"sqlite_temp_master"[7]) == 0) {
    if (sqlite3StrICmp(zTab + 7, &"sqlite_temp_schema"[7]) == 0) {
      return 1;
    }
    if (zDb == 0)
      return 0;
    if (sqlite3StrICmp(zTab + 7, &"sqlite_master"[7]) == 0)
      return 1;
    if (sqlite3StrICmp(zTab + 7, &"sqlite_schema"[7]) == 0)
      return 1;
  } else {
    if (sqlite3StrICmp(zTab + 7, &"sqlite_schema"[7]) == 0)
      return 1;
  }
  return 0;
}

const struct ExprList_item zeroItem = {0};

int getConstraintToken(const u8 *z, int *piToken) {
  int iOff = 0;
  int t = 0;
  do {
    iOff += sqlite3GetToken(&z[iOff], &t);
  } while (t == 184 || t == 185);

  *piToken = t;

  if (t == 22) {
    int nNest = 1;
    while (nNest > 0) {
      iOff += sqlite3GetToken(&z[iOff], &t);
      if (t == 22) {
        nNest++;
      } else if (t == 23) {
        t = 22;
        nNest--;
      } else if (t == 186) {
        break;
      }
    }
  }

  *piToken = t;
  return iOff;
}

int getWhitespace(const u8 *z) {
  int nRet = 0;
  while (1) {
    int t = 0;
    int n = sqlite3GetToken(&z[nRet], &t);
    if (t != 184 && t != 185)
      break;
    nRet += n;
  }
  return nRet;
}

int getConstraint(const u8 *z) {
  int iOff = 0;
  int t = 0;

  while (1) {
    int n = getConstraintToken(&z[iOff], &t);
    if (t == 120 || t == 123 || t == 19 || t == 124 || t == 125 || t == 121 || t == 114 || t == 126 || t == 133 || t == 23 || t == 25 || t == 186 || t == 24 || t == 96) {
      break;
    }
    iOff += n;
  }

  return iOff;
}

void decodeIntArray(char *zIntArray, int nOut, tRowcnt *aOut, LogEst *aLog, Index *pIndex) {
  char *z = zIntArray;
  int c;
  int i;
  tRowcnt v;

  for (i = 0; *z && i < nOut; i++) {
    v = 0;
    while ((c = z[0]) >= '0' && c <= '9') {
      v = v * 10 + c - '0';
      z++;
    }

    ((void)(0))

        ;
    (void)(aOut);

    ((void)(0))

        ;
    aLog[i] = sqlite3LogEst(v);

    if (*z == ' ')
      z++;
  }

  {

    pIndex->bUnordered = 0;
    pIndex->noSkipScan = 0;
    while (z[0]) {
      if (sqlite3_strglob("unordered*", z) == 0) {
        pIndex->bUnordered = 1;
      } else if (sqlite3_strglob("sz=[0-9]*", z) == 0) {
        int sz = sqlite3Atoi(z + 3);
        if (sz < 2)
          sz = 2;
        pIndex->szIdxRow = sqlite3LogEst(sz);
      } else if (sqlite3_strglob("noskipscan*", z) == 0) {
        pIndex->noSkipScan = 1;
      }

      while (z[0] != 0 && z[0] != ' ')
        z++;
      while (z[0] == ' ')
        z++;
    }
  }
}

int analysisLoader(void *pData, int argc, char **argv, char **NotUsed) {
  analysisInfo *pInfo = (analysisInfo *)pData;
  Index *pIndex;
  Table *pTable;
  const char *z;

  (void)(NotUsed), (void)(argc);

  if (argv == 0 || argv[0] == 0 || argv[2] == 0) {
    return 0;
  }
  pTable = sqlite3FindTable(pInfo->db, argv[0], pInfo->zDatabase);
  if (pTable == 0) {
    return 0;
  }
  if (argv[1] == 0) {
    pIndex = 0;
  } else if (sqlite3_stricmp(argv[0], argv[1]) == 0) {
    pIndex = sqlite3PrimaryKeyIndex(pTable);
  } else {
    pIndex = sqlite3FindIndex(pInfo->db, argv[1], pInfo->zDatabase);
  }
  z = argv[2];

  if (pIndex) {
    tRowcnt *aiRowEst = 0;
    int nCol = pIndex->nKeyCol + 1;

    pIndex->bUnordered = 0;
    decodeIntArray((char *)z, nCol, aiRowEst, pIndex->aiRowLogEst, pIndex);
    pIndex->hasStat1 = 1;
    if (pIndex->pPartIdxWhere == 0) {
      pTable->nRowLogEst = pIndex->aiRowLogEst[0];
      pTable->tabFlags |= 0x00000010;
    }
  } else {
    Index fakeIdx;
    fakeIdx.szIdxRow = pTable->szTabRow;

    decodeIntArray((char *)z, 1, 0, &pTable->nRowLogEst, &fakeIdx);
    pTable->szTabRow = fakeIdx.szIdxRow;
    pTable->tabFlags |= 0x00000010;
  }

  return 0;
}

i64 identLength(const char *z) {
  i64 n;
  for (n = 0; *z; n++, z++) {
    if (*z == '"') {
      n++;
    }
  }
  return n + 2;
}

void identPut(char *z, int *pIdx, char *zSignedIdent) {
  unsigned char *zIdent = (unsigned char *)zSignedIdent;
  int i, j, needQuote;
  i = *pIdx;

  for (j = 0; zIdent[j]; j++) {
    if (!(sqlite3CtypeMap[(unsigned char)(zIdent[j])] & 0x06) && zIdent[j] != '_')
      break;
  }
  needQuote = (sqlite3CtypeMap[(unsigned char)(zIdent[0])] & 0x04) || sqlite3KeywordCode(zIdent, j) != 60 || zIdent[j] != 0 || j == 0;

  if (needQuote)
    z[i++] = '"';
  for (j = 0; zIdent[j]; j++) {
    z[i++] = zIdent[j];
    if (zIdent[j] == '"')
      z[i++] = '"';
  }
  if (needQuote)
    z[i++] = '"';
  z[i] = 0;
  *pIdx = i;
}

int hasColumn(const i16 *aiCol, int nCol, int x) {
  while (nCol-- > 0) {
    if (x == *(aiCol++)) {
      return 1;
    }
  }
  return 0;
}

int collationMatch(const char *zColl, Index *pIndex) {
  int i;

  for (i = 0; i < pIndex->nColumn; i++) {
    const char *z = pIndex->azColl[i];

    ((void)(0))

        ;
    if (0 == sqlite3StrICmp(z, zColl)) {
      return 1;
    }
  }
  return 0;
}

int patternCompare(const u8 *zPattern, const u8 *zString, const struct compareInfo *pInfo, u32 matchOther) {
  u32 c, c2;
  u32 matchOne = pInfo->matchOne;
  u32 matchAll = pInfo->matchAll;
  u8 noCase = pInfo->noCase;
  const u8 *zEscaped = 0;

  while ((c = (zPattern[0] < 0x80 ? *(zPattern++) : sqlite3Utf8Read(&zPattern))) != 0) {
    if (c == matchAll) {

      while ((c = (zPattern[0] < 0x80 ? *(zPattern++) : sqlite3Utf8Read(&zPattern))) == matchAll || (c == matchOne && matchOne != 0)) {
        if (c == matchOne && sqlite3Utf8Read(&zString) == 0) {
          return 2;
        }
      }
      if (c == 0) {
        return 0;
      } else if (c == matchOther) {
        if (pInfo->matchSet == 0) {
          c = sqlite3Utf8Read(&zPattern);
          if (c == 0)
            return 2;
        } else {

          ((void)(0))

              ;
          while (*zString) {
            int bMatch = patternCompare(&zPattern[-1], zString, pInfo, matchOther);
            if (bMatch != 1)
              return bMatch;
            {
              if ((*(zString++)) >= 0xc0) {
                while ((*zString & 0xc0) == 0x80) {
                  zString++;
                }
              }
            };
          }
          return 2;
        }
      }

      if (c < 0x80) {
        char zStop[3];
        int bMatch;
        if (noCase) {
          zStop[0] = ((c) & ~(sqlite3CtypeMap[(unsigned char)(c)] & 0x20));
          zStop[1] = (sqlite3UpperToLower[(unsigned char)(c)]);
          zStop[2] = 0;
        } else {
          zStop[0] = c;
          zStop[1] = 0;
        }
        while (1) {
          zString += strcspn((const char *)zString, zStop);
          if (zString[0] == 0)
            break;
          zString++;
          bMatch = patternCompare(zPattern, zString, pInfo, matchOther);
          if (bMatch != 1)
            return bMatch;
        }
      } else {
        int bMatch;
        while ((c2 = (zString[0] < 0x80 ? *(zString++) : sqlite3Utf8Read(&zString))) != 0) {
          if (c2 != c)
            continue;
          bMatch = patternCompare(zPattern, zString, pInfo, matchOther);
          if (bMatch != 1)
            return bMatch;
        }
      }
      return 2;
    }
    if (c == matchOther) {
      if (pInfo->matchSet == 0) {
        c = sqlite3Utf8Read(&zPattern);
        if (c == 0)
          return 1;
        zEscaped = zPattern;
      } else {
        u32 prior_c = 0;
        int seen = 0;
        int invert = 0;
        c = sqlite3Utf8Read(&zString);
        if (c == 0)
          return 1;
        c2 = sqlite3Utf8Read(&zPattern);
        if (c2 == '^') {
          invert = 1;
          c2 = sqlite3Utf8Read(&zPattern);
        }
        if (c2 == ']') {
          if (c == ']')
            seen = 1;
          c2 = sqlite3Utf8Read(&zPattern);
        }
        while (c2 && c2 != ']') {
          if (c2 == '-' && zPattern[0] != ']' && zPattern[0] != 0 && prior_c > 0) {
            c2 = sqlite3Utf8Read(&zPattern);
            if (c >= prior_c && c <= c2)
              seen = 1;
            prior_c = 0;
          } else {
            if (c == c2) {
              seen = 1;
            }
            prior_c = c2;
          }
          c2 = sqlite3Utf8Read(&zPattern);
        }
        if (c2 == 0 || (seen ^ invert) == 0) {
          return 1;
        }
        continue;
      }
    }
    c2 = (zString[0] < 0x80 ? *(zString++) : sqlite3Utf8Read(&zString));
    if (c == c2)
      continue;
    if (noCase && (sqlite3UpperToLower[(unsigned char)(c)]) == (sqlite3UpperToLower[(unsigned char)(c2)]) && c < 0x80 && c2 < 0x80) {
      continue;
    }
    if (c == matchOne && zPattern != zEscaped && c2 != 0)
      continue;
    return 1;
  }
  return *zString == 0 ? 0 : 1;
}

const char hexdigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int isNHex(const char *z, int N, u32 *pVal) {
  int i;
  u32 v = 0;
  for (i = 0; i < N; i++) {
    if (!(sqlite3CtypeMap[(unsigned char)(z[i])] & 0x08))
      return 0;
    v = (v << 4) + sqlite3HexToInt(z[i]);
  }
  *pVal = v;
  return 1;
}

int strContainsChar(const u8 *zStr, int nStr, u32 ch) {
  const u8 *zEnd = &zStr[nStr];
  const u8 *z = zStr;
  while (z < zEnd) {
    u32 tst = (z[0] < 0x80 ? *(z++) : sqlite3Utf8Read(&z));
    if (tst == ch)
      return 1;
  }
  return 0;
}

double xCeil(double x) { return ceil(x); }

double xFloor(double x) { return floor(x); }

double degToRad(double x) {
  return x * (

                 3.14159265358979323846

                 / 180.0);
}

double radToDeg(double x) {
  return x * (180.0 /

              3.14159265358979323846

             );
}

int percentIsInfinity(double r) {
  sqlite3_uint64 u;

  memcpy(&u, &r, sizeof(u));
  return ((u >> 52) & 0x7ff) == 0x7ff;
}

int percentSameValue(double a, double b) {
  a -= b;
  return a >= -0.001 && a <= 0.001;
}

void percentSort(double *a, unsigned int n) {
  int iLt;
  int iGt;
  int i;
  double rPivot;

  while (n >= 2) {
    if (a[0] > a[n - 1]) {
      {
        double ttt = (a[0]);
        (a[0]) = (a[n - 1]);
        (a[n - 1]) = ttt;
      }
    }
    if (n == 2)
      return;
    iGt = n - 1;
    i = n / 2;
    if (a[0] > a[i]) {
      {
        double ttt = (a[0]);
        (a[0]) = (a[i]);
        (a[i]) = ttt;
      }
    } else if (a[i] > a[iGt]) {
      {
        double ttt = (a[i]);
        (a[i]) = (a[iGt]);
        (a[iGt]) = ttt;
      }
    }
    if (n == 3)
      return;
    rPivot = a[i];
    iLt = i = 1;
    do {
      if (a[i] < rPivot) {
        if (i > iLt) {
          double ttt = (a[i]);
          (a[i]) = (a[iLt]);
          (a[iLt]) = ttt;
        }
        iLt++;
        i++;
      } else if (a[i] > rPivot) {
        do {
          iGt--;
        } while (iGt > i && a[iGt] > rPivot);
        {
          double ttt = (a[i]);
          (a[i]) = (a[iGt]);
          (a[iGt]) = ttt;
        }
      } else {
        i++;
      }
    } while (i < iGt);
    if (iLt > (int)(n / 2)) {
      if (n - iGt >= 2)
        percentSort(a + iGt, n - iGt);
      n = iLt;
    } else {
      if (iLt >= 2)
        percentSort(a, iLt);
      a += iGt;
      n -= iGt;
    }
  }
}

const char *const pragCName[] = {
    "id",         "seq",     "table", "from", "to",   "on_update", "on_delete", "match", "cid",  "name", "type", "notnull", "dflt_value", "pk",  "hidden",

    "name",       "builtin", "type",  "enc",  "narg", "flags",     "schema",    "name",  "type", "ncol", "wr",   "strict",  "seqno",      "cid", "name",   "desc", "coll", "key", "seq", "name", "unique", "origin", "partial", "tbl", "idx", "wdth", "hght", "flgs", "table", "rowid", "parent", "fkid", "busy", "log", "checkpointed", "seq", "name", "file",

    "database",   "status",

    "cache_size",

    "timeout",
};

const PragmaName aPragmaName[] = {

    {"analysis_limit", 1, 0x10, 0, 0, 0},

    {"application_id", 2, 0x04 | 0x10, 0, 0, 8},

    {"auto_vacuum", 3, 0x01 | 0x10 | 0x80 | 0x04, 0, 0, 0},

    {"automatic_index", 4, 0x10 | 0x04, 0, 0, 0x00008000},

    {"busy_timeout", 5, 0x10, 56, 1, 0},

    {"cache_size", 6, 0x01 | 0x10 | 0x80 | 0x04, 0, 0, 0},

    {"cache_spill", 7, 0x10 | 0x80 | 0x04, 0, 0, 0},

    {"case_sensitive_like", 8, 0x02, 0, 0, 0},

    {"cell_size_check", 4, 0x10 | 0x04, 0, 0, 0x00200000},

    {"checkpoint_fullfsync", 4, 0x10 | 0x04, 0, 0, 0x00000010},

    {"collation_list", 9, 0x10, 33, 2, 0},

    {"compile_options", 10, 0x10, 0, 0, 0},

    {"count_changes", 4, 0x10 | 0x04, 0, 0, ((u64)(0x00001) << 32)},

    {"data_version", 2, 0x08 | 0x10, 0, 0, 15},

    {"database_list", 12, 0x10, 50, 3, 0},

    {"default_cache_size", 13, 0x01 | 0x10 | 0x80 | 0x04, 55, 1, 0},

    {"defer_foreign_keys", 4, 0x10 | 0x04, 0, 0, 0x00080000},

    {"empty_result_callbacks", 4, 0x10 | 0x04, 0, 0, 0x00000100},

    {"encoding", 14, 0x10 | 0x04, 0, 0, 0},

    {"foreign_key_check", 15, 0x01 | 0x10 | 0x20 | 0x40, 43, 4, 0},

    {"foreign_key_list", 16, 0x01 | 0x20 | 0x40, 0, 8, 0},

    {"foreign_keys", 4, 0x10 | 0x04, 0, 0, 0x00004000},

    {"freelist_count", 2, 0x08 | 0x10, 0, 0, 0},

    {"full_column_names", 4, 0x10 | 0x04, 0, 0, 0x00000004},
    {"fullfsync", 4, 0x10 | 0x04, 0, 0, 0x00000008},

    {"function_list", 17, 0x10, 15, 6, 0},

    {"hard_heap_limit", 18, 0x10, 0, 0, 0},

    {"ignore_check_constraints", 4, 0x10 | 0x04, 0, 0, 0x00000200},

    {"incremental_vacuum", 19, 0x01 | 0x02, 0, 0, 0},

    {"index_info", 20, 0x01 | 0x20 | 0x40, 27, 3, 0},
    {"index_list", 21, 0x01 | 0x20 | 0x40, 33, 5, 0},
    {"index_xinfo", 20, 0x01 | 0x20 | 0x40, 27, 6, 1},

    {"integrity_check", 22, 0x01 | 0x10 | 0x20 | 0x40, 0, 0, 0},

    {"journal_mode", 23, 0x01 | 0x10 | 0x80, 0, 0, 0},
    {"journal_size_limit", 24, 0x10 | 0x80, 0, 0, 0},

    {"legacy_alter_table", 4, 0x10 | 0x04, 0, 0, 0x04000000},

    {"locking_mode", 26, 0x10 | 0x80, 0, 0, 0},
    {"max_page_count", 27, 0x01 | 0x10 | 0x80, 0, 0, 0},
    {"mmap_size", 28, 0, 0, 0, 0},

    {"module_list", 29, 0x10, 9, 1, 0},

    {"optimize", 30, 0x20 | 0x01, 0, 0, 0},

    {"page_count", 27, 0x01 | 0x10 | 0x80, 0, 0, 0},
    {"page_size", 31, 0x10 | 0x80 | 0x04, 0, 0, 0},

    {"pragma_list", 32, 0x10, 9, 1, 0},

    {"query_only", 4, 0x10 | 0x04, 0, 0, 0x00100000},

    {"quick_check", 22, 0x01 | 0x10 | 0x20 | 0x40, 0, 0, 0},

    {"read_uncommitted", 4, 0x10 | 0x04, 0, 0, ((u64)(0x00004) << 32)},
    {"recursive_triggers", 4, 0x10 | 0x04, 0, 0, 0x00002000},
    {"reverse_unordered_selects", 4, 0x10 | 0x04, 0, 0, 0x00001000},

    {"schema_version", 2, 0x04 | 0x10, 0, 0, 1},

    {"secure_delete", 33, 0x10, 0, 0, 0},

    {"short_column_names", 4, 0x10 | 0x04, 0, 0, 0x00000040},

    {"shrink_memory", 34, 0x02, 0, 0, 0},
    {"soft_heap_limit", 35, 0x10, 0, 0, 0},

    {"synchronous", 36, 0x01 | 0x10 | 0x80 | 0x04, 0, 0, 0},

    {"table_info", 37, 0x01 | 0x20 | 0x40, 8, 6, 0},
    {"table_list", 38, 0x01 | 0x20, 21, 6, 0},
    {"table_xinfo", 37, 0x01 | 0x20 | 0x40, 8, 7, 1},

    {"temp_store", 39, 0x10 | 0x04, 0, 0, 0},
    {"temp_store_directory", 40, 0x04, 0, 0, 0},

    {"threads", 41, 0x10, 0, 0, 0},

    {"trusted_schema", 4, 0x10 | 0x04, 0, 0, 0x00000080},

    {"user_version", 2, 0x04 | 0x10, 0, 0, 6},

    {"wal_autocheckpoint", 42, 0, 0, 0, 0},
    {"wal_checkpoint", 43, 0x01, 47, 3, 0},

    {"writable_schema", 4, 0x10 | 0x04, 0, 0, 0x00000001 | 0x08000000},

};

u8 getSafetyLevel(const char *z, int omitFull, u8 dflt) {

  static const char zText[] = "onoffalseyestruextrafull";
  static const u8 iOffset[] = {0, 1, 2, 4, 9, 12, 15, 20};
  static const u8 iLength[] = {2, 2, 3, 5, 3, 4, 5, 4};
  static const u8 iValue[] = {1, 0, 0, 0, 1, 1, 3, 2};

  int i, n;
  if ((sqlite3CtypeMap[(unsigned char)(*z)] & 0x04)) {
    return (u8)sqlite3Atoi(z);
  }
  n = sqlite3Strlen30(z);
  for (i = 0; i < ((int)(sizeof(iLength) / sizeof(iLength[0]))); i++) {
    if (iLength[i] == n && sqlite3_strnicmp(&zText[iOffset[i]], z, n) == 0 && (!omitFull || iValue[i] <= 1)) {
      return iValue[i];
    }
  }
  return dflt;
}

int getLockingMode(const char *z) {
  if (z) {
    if (0 == sqlite3StrICmp(z, "exclusive"))
      return 1;
    if (0 == sqlite3StrICmp(z, "normal"))
      return 0;
  }
  return -1;
}

int getAutoVacuum(const char *z) {
  int i;
  if (0 == sqlite3StrICmp(z, "none"))
    return 0;
  if (0 == sqlite3StrICmp(z, "full"))
    return 1;
  if (0 == sqlite3StrICmp(z, "incremental"))
    return 2;
  i = sqlite3Atoi(z);
  return (u8)((i >= 0 && i <= 2) ? i : 0);
}

int getTempStore(const char *z) {
  if (z[0] >= '0' && z[0] <= '2') {
    return z[0] - '0';
  } else if (sqlite3StrICmp(z, "file") == 0) {
    return 1;
  } else if (sqlite3StrICmp(z, "memory") == 0) {
    return 2;
  } else {
    return 0;
  }
}

const char *actionName(u8 action) {
  const char *zName;
  switch (action) {
  case 8:
    zName = "SET NULL";
    break;
  case 9:
    zName = "SET DEFAULT";
    break;
  case 10:
    zName = "CASCADE";
    break;
  case 7:
    zName = "RESTRICT";
    break;
  default:
    zName = "NO ACTION";

    ((void)(0))

        ;
    break;
  }
  return zName;
}

const PragmaName *pragmaLocate(const char *zName) {
  int upr, lwr, mid = 0, rc;
  lwr = 0;
  upr = ((int)(sizeof(aPragmaName) / sizeof(aPragmaName[0]))) - 1;
  while (lwr <= upr) {
    mid = (lwr + upr) / 2;
    rc = sqlite3_stricmp(zName, aPragmaName[mid].zName);
    if (rc == 0)
      break;
    if (rc < 0) {
      upr = mid - 1;
    } else {
      lwr = mid + 1;
    }
  }
  return lwr > upr ? 0 : &aPragmaName[mid];
}

int inAnyUsingClause(const char *zName, SrcItem *pBase, int N) {
  while (N > 0) {
    N--;
    pBase++;
    if (pBase->fg.isUsing == 0)
      continue;
    if ((pBase->u3.pUsing == 0))
      continue;
    if (sqlite3IdListIndex(pBase->u3.pUsing, zName) >= 0)
      return 1;
  }
  return 0;
}

int allowedOp(int op) {

  if (op > 58)
    return 0;
  if (op >= 54)
    return 1;
  return op == 50 || op == 51 || op == 45;
}

u16 operatorMask(int op) {
  u16 c;

  if (op >= 54) {

    ((void)(0))

        ;
    c = (u16)(0x0002 << (op - 54));
  } else if (op == 50) {
    c = 0x0001;
  } else if (op == 51) {
    c = 0x0100;
  } else {

    ((void)(0))

        ;
    c = 0x0080;
  }

  return c;
}

LogEst estLog(LogEst N) { return N <= 10 ? 0 : sqlite3LogEst(N) - 33; }

int whereUsablePartialIndex(int iTab, u8 jointype, WhereClause *pWC, Expr *pWhere) {
  int i;
  WhereTerm *pTerm;
  Parse *pParse;

  if (jointype & 0x40)
    return 0;
  pParse = pWC->pWInfo->pParse;
  while (pWhere->op == 44) {
    if (!whereUsablePartialIndex(iTab, jointype, pWC, pWhere->pLeft))
      return 0;
    pWhere = pWhere->pRight;
  }
  for (i = 0, pTerm = pWC->a; i < pWC->nTerm; i++, pTerm++) {
    Expr *pExpr;
    pExpr = pTerm->pExpr;
    if ((!(((pExpr)->flags & (u32)(0x000001)) != 0) || pExpr->w.iJoin == iTab) && ((jointype & 0x20) == 0 || (((pExpr)->flags & (u32)(0x000001)) != 0)) && sqlite3ExprImpliesExpr(pParse, pExpr, pWhere, iTab) && !sqlite3ExprImpliesExpr(pParse, pExpr, pWhere, -1) && (pTerm->wtFlags & 0x0080) == 0) {
      return 1;
    }
  }
  return 0;
}

int allConstraintsUsed(struct sqlite3_index_constraint_usage *aUsage, int nCons) {
  int ii;
  for (ii = 0; ii < nCons; ii++) {
    if (aUsage[ii].argvIndex <= 0)
      return 0;
  }
  return 1;
}

const char row_numberName[] = "row_number";

const char dense_rankName[] = "dense_rank";

const char rankName[] = "rank";

const char percent_rankName[] = "percent_rank";

const char cume_distName[] = "cume_dist";

const char ntileName[] = "ntile";

const char last_valueName[] = "last_value";

const char nth_valueName[] = "nth_value";

const char first_valueName[] = "first_value";

const char leadName[] = "lead";

const char lagName[] = "lag";

void *parserStackRealloc(void *pOld, sqlite3_uint64 newSize, Parse *pParse) {
  void *p = sqlite3FaultSim(700) ? 0 : sqlite3_realloc(pOld, newSize);
  if (p == 0)
    sqlite3OomFault(pParse->db);
  return p;
}

void parserStackFree(void *pOld, Parse *pParse) {
  (void)pParse;
  sqlite3_free(pOld);
}

const unsigned short int yy_action[] = {
    134, 131, 238, 290, 290, 1353, 593, 1332, 478, 1606, 593, 1315, 593, 7, 593, 1353, 590, 593, 579, 424, 1566, 134, 131, 238, 1318, 541, 478, 477, 575, 84, 84, 1005, 303, 84, 84, 51, 51, 63, 63, 1006, 84, 84, 498, 141, 142, 93, 442, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 424, 296, 296, 498, 296, 296, 567, 553, 296, 296, 1306, 574, 1358, 1358, 590, 542, 579, 590, 574, 579, 548, 590, 1304, 579, 141, 142, 93, 576, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 399, 478, 395, 6, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 44, 342, 593, 305, 1127, 1280, 1, 1, 599, 2, 1284, 598, 1200, 1284, 1200, 330, 424, 158, 330, 1613, 158, 390, 116, 308, 1366, 51, 51, 1366, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 141, 142, 93, 515, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 1230, 329, 584, 296, 296, 212, 296, 296, 568, 568, 488, 143, 1072, 1072, 1086, 1089, 590, 1195, 579, 590, 340, 579, 140, 140, 140, 140, 133, 392, 564, 536, 1195, 250, 425, 1195, 250, 137, 137, 136, 136, 136, 135, 132, 463, 291, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 966, 1230, 1231, 1230, 412, 965, 467, 412, 424, 467, 489, 357, 1611, 391, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 463, 134, 131, 238, 555, 1076, 141, 142, 93, 593, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 1317, 134, 131, 238, 424, 549, 1597, 1531, 333, 97, 83, 83, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 141, 142, 93, 1657, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 591, 1230, 958, 958, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 44, 398, 547, 1306, 136, 136, 136, 135, 132, 463, 386, 593, 442, 595, 145, 595, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 500, 1230, 112, 550, 460, 459, 51, 51, 424, 296, 296, 479, 334, 1259, 1230, 1231, 1230, 1599, 1261, 388, 312, 444, 590, 246, 579, 546, 1260, 271, 235, 329, 584, 551, 141, 142, 93, 429, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 22, 22, 1230, 1262, 424, 1262, 216, 296, 296, 98, 1230, 1231, 1230, 264, 884, 45, 528, 525, 524, 1041, 590, 1269, 579, 421, 420, 393, 523, 44, 141, 142, 93, 498, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 593, 1611, 561, 1230, 1231, 1230, 23, 264, 515, 200, 528, 525, 524, 127, 585, 509, 4, 355, 487, 506, 523, 593, 498, 84, 84, 134, 131, 238, 329, 584, 588, 1627, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 19, 19, 435, 1230, 1460, 297, 297, 311, 424, 1565, 464, 1631, 599, 2, 1284, 437, 574, 1107, 590, 330, 579, 158, 582, 489, 357, 573, 593, 592, 1366, 409, 1274, 1230, 141, 142, 93, 1364, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 389, 84, 84, 1062, 567, 1230, 313, 1523, 593, 125, 125, 970, 1230, 1231, 1230, 296, 296, 126, 46, 464, 594, 464, 296, 296, 1050, 1230, 218, 439, 590, 1604, 579, 84, 84, 7, 403, 590, 515, 579, 325, 417, 1230, 1231, 1230, 250, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 1050, 1050, 1052, 1053, 35, 1275, 1230, 1231, 1230, 424, 1370, 993, 574, 371, 414, 274, 412, 1597, 467, 1302, 552, 451, 590, 543, 579, 1530, 1230, 1231, 1230, 1214, 201, 409, 1174, 141, 142, 93, 223, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 296, 296, 1250, 593, 424, 296, 296, 236, 529, 296, 296, 515, 100, 590, 1600, 579, 48, 1605, 590, 1230, 579, 7, 590, 577, 579, 904, 84, 84, 141, 142, 93, 496, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 1365, 1230, 296, 296, 1250, 115, 1275, 326, 233, 539, 1062, 40, 282, 127, 585, 590, 4, 579, 329, 584, 1230, 1231, 1230, 1598, 593, 388, 904, 1051, 1356, 1356, 588, 1050, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 185, 593, 1230, 19, 19, 1230, 971, 1597, 424, 1651, 464, 129, 908, 1195, 1230, 1231, 1230, 1325, 443, 1050, 1050, 1052, 582, 1603, 149, 149, 1195, 7, 5, 1195, 1687, 410, 141, 142, 93, 1536, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 1214, 397, 593, 1062, 424, 1536, 1538, 50, 901, 125, 125, 1230, 1231, 1230, 1230, 1231, 1230, 126, 1230, 464, 594, 464, 515, 1230, 1050, 84, 84, 3, 141, 142, 93, 924, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 1050, 1050, 1052, 1053, 35, 442, 457, 532, 433, 1230, 1062, 1361, 540, 540, 1598, 925, 388, 7, 1129, 1230, 1231, 1230, 1129, 1536, 1230, 1231, 1230, 1051, 570, 1214, 593, 1050, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 6, 185, 1195, 1230, 231, 593, 382, 992, 424, 151, 151, 510, 1213, 557, 482, 1195, 381, 160, 1195, 1050, 1050, 1052, 1230, 1231, 1230, 422, 593, 447, 84, 84, 593, 217, 141, 142, 93, 593, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 1214, 19, 19, 593, 424, 19, 19, 442, 1063, 442, 19, 19, 1230, 1231, 1230, 515, 445, 458, 1597, 386, 315, 1175, 1685, 556, 1685, 450, 84, 84, 141, 142, 93, 505, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 442, 1147, 454, 1597, 362, 1041, 593, 462, 1460, 1233, 47, 1393, 324, 565, 565, 115, 1148, 449, 7, 460, 459, 307, 375, 354, 593, 113, 593, 329, 584, 19, 19, 1149, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 209, 1173, 563, 19, 19, 19, 19, 49, 424, 944, 1175, 1686, 1046, 1686, 218, 355, 484, 343, 210, 945, 569, 562, 1262, 1233, 1262, 490, 314, 423, 424, 1598, 1206, 388, 141, 142, 93, 440, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 352, 316, 531, 316, 141, 142, 93, 549, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 446, 10, 1598, 274, 388, 915, 281, 299, 383, 534, 378, 533, 269, 593, 1206, 587, 587, 587, 374, 293, 1579, 991, 1173, 302, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 53, 53, 520, 1250, 593, 1147, 1576, 431, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 1148, 301, 593, 1577, 593, 1307, 431, 54, 54, 593, 268, 593, 461, 461, 461, 1149, 347, 492, 424, 135, 132, 463, 1146, 1195, 474, 68, 68, 69, 69, 550, 332, 287, 21, 21, 55, 55, 1195, 581, 424, 1195, 309, 1250, 141, 142, 93, 119, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 593, 237, 480, 1476, 141, 142, 93, 593, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 344, 430, 346, 70, 70, 494, 991, 1132, 1132, 512, 56, 56, 1269, 593, 268, 593, 369, 374, 593, 481, 215, 384, 1624, 481, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 71, 71, 72, 72, 225, 73, 73, 593, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 586, 431, 593, 872, 873, 874, 593, 911, 593, 1602, 74, 74, 593, 7, 1460, 242, 593, 306, 424, 1578, 472, 306, 364, 219, 367, 75, 75, 430, 345, 57, 57, 58, 58, 432, 187, 59, 59, 593, 424, 61, 61, 1475, 141, 142, 93, 123, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 424, 570, 62, 62, 141, 142, 93, 911, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 161, 384, 1624, 1474, 141, 130, 93, 441, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 267, 266, 265, 1460, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 593, 1336, 593, 1269, 1460, 384, 1624, 231, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 593, 163, 593, 76, 76, 77, 77, 593, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 475, 593, 483, 78, 78, 20, 20, 1249, 424, 491, 79, 79, 495, 422, 295, 235, 1574, 38, 511, 896, 422, 335, 240, 422, 147, 147, 112, 593, 424, 593, 101, 222, 991, 142, 93, 455, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 593, 39, 148, 148, 80, 80, 93, 551, 1254, 1254, 1085, 1088, 1075, 1075, 139, 139, 140, 140, 140, 140, 328, 923, 922, 64, 64, 502, 1656, 1005, 933, 896, 124, 422, 121, 254, 593, 1006, 593, 226, 593, 127, 585, 164, 4, 16, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 588, 81, 81, 65, 65, 82, 82, 593, 138, 138, 138, 138, 137, 137, 136, 136, 136, 135, 132, 463, 593, 226, 237, 966, 464, 593, 298, 593, 965, 593, 66, 66, 593, 1170, 593, 411, 582, 353, 469, 115, 593, 471, 169, 173, 173, 593, 44, 991, 174, 174, 89, 89, 67, 67, 593, 85, 85, 150, 150, 1114, 1043, 593, 273, 86, 86, 1062, 593, 503, 171, 171, 593, 125, 125, 497, 593, 273, 336, 152, 152, 126, 1335, 464, 594, 464, 146, 146, 1050, 593, 545, 172, 172, 593, 1054, 165, 165, 256, 339, 156, 156, 127, 585, 1586, 4, 329, 584, 499, 358, 273, 115, 348, 155, 155, 930, 931, 153, 153, 588, 1114, 1050, 1050, 1052, 1053, 35, 1554, 521, 593, 270, 1008, 1009, 9, 593, 372, 593, 115, 593, 168, 593, 115, 593, 1110, 464, 270, 996, 964, 273, 129, 1645, 1214, 154, 154, 1054, 1404, 582, 88, 88, 90, 90, 87, 87, 52, 52, 60, 60, 1405, 504, 537, 559, 1179, 961, 507, 129, 558, 127, 585, 1126, 4, 1126, 1125, 894, 1125, 162, 1062, 963, 359, 129, 1401, 363, 125, 125, 588, 366, 368, 370, 1349, 1334, 126, 1333, 464, 594, 464, 377, 387, 1050, 1391, 1414, 1618, 1459, 1387, 1399, 208, 580, 1464, 1314, 464, 243, 516, 1305, 1293, 1384, 1292, 1294, 1638, 288, 170, 228, 582, 12, 408, 321, 322, 241, 323, 245, 1446, 1050, 1050, 1052, 1053, 35, 559, 304, 350, 351, 501, 560, 127, 585, 1441, 4, 1451, 1434, 310, 1450, 526, 1062, 1332, 415, 380, 232, 1527, 125, 125, 588, 1214, 1396, 356, 1526, 583, 126, 1397, 464, 594, 464, 1641, 535, 1050, 1581, 1395, 1269, 1583, 1582, 213, 402, 277, 214, 227, 464, 1573, 239, 1571, 1266, 1394, 434, 198, 100, 224, 96, 183, 582, 191, 485, 193, 486, 194, 195, 196, 519, 1050, 1050, 1052, 1053, 35, 559, 113, 252, 413, 1447, 558, 493, 13, 1455, 416, 1453, 1452, 14, 202, 1521, 1062, 1532, 508, 258, 106, 514, 125, 125, 99, 1214, 1543, 289, 260, 206, 126, 365, 464, 594, 464, 361, 517, 1050, 261, 448, 1295, 262, 418, 1352, 1351, 108, 1350, 1655, 1654, 1343, 915, 419, 1322, 233, 452, 319, 379, 1321, 453, 1623, 320, 1320, 275, 1653, 544, 276, 1609, 1608, 1342, 1050, 1050, 1052, 1053, 35, 1630, 1218, 466, 385, 456, 300, 1419, 144, 1418, 570, 407, 407, 406, 284, 404, 11, 1508, 881, 396, 120, 127, 585, 394, 4, 1214, 327, 114, 1375, 1374, 220, 247, 400, 338, 401, 554, 42, 1224, 588, 596, 283, 337, 285, 286, 188, 597, 1290, 1285, 175, 1558, 176, 1559, 1557, 1556, 159, 317, 229, 177, 868, 230, 91, 465, 464, 221, 331, 468, 1165, 470, 473, 94, 244, 95, 249, 189, 582, 1124, 1122, 341, 427, 190, 178, 1249, 179, 43, 192, 947, 349, 428, 1138, 197, 251, 180, 181, 436, 102, 182, 438, 103, 104, 199, 248, 1140, 253, 1062, 105, 255, 1137, 166, 24, 125, 125, 257, 1264, 273, 360, 513, 259, 126, 15, 464, 594, 464, 204, 883, 1050, 518, 263, 373, 381, 92, 585, 1130, 4, 203, 205, 426, 107, 522, 25, 26, 329, 584, 913, 572, 527, 376, 588, 926, 530, 109, 184, 318, 167, 110, 27, 538, 1050, 1050, 1052, 1053, 35, 1211, 1091, 17, 476, 111, 1181, 234, 292, 1180, 464, 294, 207, 994, 129, 1201, 272, 1000, 28, 1197, 29, 30, 582, 1199, 1205, 1214, 31, 1204, 32, 1186, 41, 566, 33, 1105, 211, 8, 115, 1092, 1090, 1094, 34, 278, 578, 1095, 117, 122, 118, 1145, 36, 18, 128, 1062, 1055, 895, 957, 37, 589, 125, 125, 279, 186, 280, 1646, 157, 405, 126, 1220, 464, 594, 464, 1218, 466, 1050, 1219, 300, 1281, 1281, 1281, 1281, 407, 407, 406, 284, 404, 1281, 1281, 881, 1281, 300, 1281, 1281, 571, 1281, 407, 407, 406, 284, 404, 1281, 247, 881, 338, 1281, 1281, 1050, 1050, 1052, 1053, 35, 337, 1281, 1281, 1281, 247, 1281, 338, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 337, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1214, 1281, 1281, 1281, 1281, 1281, 1281, 249, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 178, 1281, 1281, 43, 1281, 1281, 249, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 178, 1281, 1281, 43, 1281, 1281, 248, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 248, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 426, 1281, 1281, 1281, 1281, 329, 584, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 426, 1281, 1281, 1281, 1281, 329, 584, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 476, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 1281, 476,
};

const unsigned short int yy_lookahead[] = {
    277, 278, 279, 241, 242, 225, 195, 227, 195, 312, 195, 218, 195, 316, 195, 235, 254, 195, 256, 19, 297, 277, 278, 279, 218, 206, 213, 214, 206, 218, 219, 31, 206, 218, 219, 218, 219, 218, 219, 39, 218, 219, 195, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 19, 241, 242, 195, 241, 242, 195, 255, 241, 242, 195, 255, 237, 238, 254, 255, 256, 254, 255, 256, 264, 254, 207, 256, 43, 44, 45, 264, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 251, 287, 253, 215, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 82, 265, 195, 271, 11, 187, 188, 189, 190, 191, 192, 190, 87, 192, 89, 197, 19, 199, 197, 317, 199, 319, 25, 271, 206, 218, 219, 206, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 60, 139, 140, 241, 242, 289, 241, 242, 309, 310, 294, 70, 47, 48, 49, 50, 254, 77, 256, 254, 195, 256, 55, 56, 57, 58, 59, 221, 88, 109, 90, 269, 240, 93, 269, 107, 108, 109, 110, 111, 112, 113, 114, 215, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 136, 117, 118, 119, 298, 141, 300, 298, 19, 300, 129, 130, 317, 318, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 114, 277, 278, 279, 146, 122, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 218, 277, 278, 279, 19, 19, 195, 286, 23, 68, 218, 219, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 43, 44, 45, 232, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 135, 60, 137, 138, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 82, 281, 206, 195, 109, 110, 111, 112, 113, 114, 195, 195, 195, 205, 22, 207, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 60, 116, 117, 107, 108, 218, 219, 19, 241, 242, 121, 23, 116, 117, 118, 119, 306, 121, 308, 206, 234, 254, 15, 256, 195, 129, 259, 260, 139, 140, 145, 43, 44, 45, 200, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 218, 219, 60, 154, 19, 156, 265, 241, 242, 24, 117, 118, 119, 120, 21, 73, 123, 124, 125, 74, 254, 61, 256, 107, 108, 221, 133, 82, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 317, 318, 117, 118, 119, 22, 120, 195, 22, 123, 124, 125, 19, 20, 284, 22, 128, 81, 288, 133, 195, 195, 218, 219, 277, 278, 279, 139, 140, 36, 195, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 218, 219, 62, 60, 195, 241, 242, 271, 19, 240, 60, 189, 190, 191, 192, 233, 255, 124, 254, 197, 256, 199, 72, 129, 130, 264, 195, 195, 206, 22, 23, 60, 43, 44, 45, 206, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 195, 218, 219, 101, 195, 60, 271, 162, 195, 107, 108, 109, 117, 118, 119, 241, 242, 115, 73, 117, 118, 119, 241, 242, 122, 60, 195, 266, 254, 312, 256, 218, 219, 316, 203, 254, 195, 256, 255, 208, 117, 118, 119, 269, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 154, 155, 156, 157, 158, 102, 117, 118, 119, 19, 242, 144, 255, 23, 206, 24, 298, 195, 300, 206, 195, 264, 254, 206, 256, 240, 117, 118, 119, 183, 22, 22, 23, 43, 44, 45, 151, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 241, 242, 60, 195, 19, 241, 242, 195, 23, 241, 242, 195, 152, 254, 310, 256, 243, 312, 254, 60, 256, 316, 254, 206, 256, 60, 218, 219, 43, 44, 45, 272, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 240, 60, 241, 242, 118, 25, 102, 255, 166, 167, 101, 22, 26, 19, 20, 254, 22, 256, 139, 140, 117, 118, 119, 306, 195, 308, 117, 118, 237, 238, 36, 122, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 195, 60, 218, 219, 60, 109, 195, 19, 217, 60, 25, 23, 77, 117, 118, 119, 225, 233, 154, 155, 156, 72, 312, 218, 219, 90, 316, 22, 93, 303, 304, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 183, 195, 195, 101, 19, 213, 214, 243, 23, 107, 108, 117, 118, 119, 117, 118, 119, 115, 60, 117, 118, 119, 195, 60, 122, 218, 219, 22, 43, 44, 45, 35, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 154, 155, 156, 157, 158, 195, 255, 67, 195, 60, 101, 240, 311, 312, 306, 75, 308, 316, 29, 117, 118, 119, 33, 287, 117, 118, 119, 118, 146, 183, 195, 122, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 215, 195, 77, 60, 25, 195, 122, 144, 19, 218, 219, 66, 23, 88, 246, 90, 132, 25, 93, 154, 155, 156, 117, 118, 119, 257, 195, 131, 218, 219, 195, 265, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 183, 218, 219, 195, 19, 218, 219, 195, 23, 195, 218, 219, 117, 118, 119, 195, 233, 255, 195, 195, 233, 22, 23, 146, 25, 233, 218, 219, 43, 44, 45, 294, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 12, 234, 195, 240, 74, 195, 255, 195, 60, 243, 262, 263, 311, 312, 25, 27, 19, 316, 107, 108, 265, 24, 265, 195, 150, 195, 139, 140, 218, 219, 42, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 233, 102, 67, 218, 219, 218, 219, 243, 19, 64, 22, 23, 23, 25, 195, 128, 129, 130, 233, 74, 233, 86, 154, 118, 156, 130, 265, 208, 19, 306, 95, 308, 43, 44, 45, 266, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 153, 230, 96, 232, 43, 44, 45, 19, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 114, 22, 306, 24, 308, 127, 120, 121, 122, 123, 124, 125, 126, 195, 147, 212, 213, 214, 132, 23, 195, 25, 102, 100, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 218, 219, 19, 60, 195, 12, 210, 211, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 27, 134, 195, 195, 195, 210, 211, 218, 219, 195, 47, 195, 212, 213, 214, 42, 16, 130, 19, 112, 113, 114, 23, 77, 195, 218, 219, 218, 219, 117, 163, 164, 218, 219, 218, 219, 90, 64, 19, 93, 153, 118, 43, 44, 45, 160, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 195, 119, 272, 276, 43, 44, 45, 195, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 78, 116, 80, 218, 219, 116, 144, 128, 129, 130, 218, 219, 61, 195, 47, 195, 16, 132, 195, 263, 195, 314, 315, 267, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 218, 219, 218, 219, 151, 218, 219, 195, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 210, 211, 195, 7, 8, 9, 195, 60, 195, 312, 218, 219, 195, 316, 195, 120, 195, 263, 19, 195, 125, 267, 78, 24, 80, 218, 219, 116, 162, 218, 219, 218, 219, 301, 302, 218, 219, 195, 19, 218, 219, 276, 43, 44, 45, 160, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 19, 146, 218, 219, 43, 44, 45, 118, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 165, 314, 315, 276, 43, 44, 45, 266, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 128, 129, 130, 195, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 228, 195, 61, 195, 314, 315, 25, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 22, 195, 218, 219, 218, 219, 195, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 195, 246, 218, 219, 218, 219, 25, 19, 246, 218, 219, 246, 257, 259, 260, 195, 22, 266, 60, 257, 195, 120, 257, 218, 219, 116, 195, 19, 195, 150, 151, 25, 44, 45, 266, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 195, 54, 218, 219, 218, 219, 45, 145, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 246, 121, 122, 218, 219, 19, 23, 31, 25, 118, 159, 257, 161, 24, 195, 39, 195, 143, 195, 19, 20, 22, 22, 24, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 36, 218, 219, 218, 219, 218, 219, 195, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 195, 143, 119, 136, 60, 195, 22, 195, 141, 195, 218, 219, 195, 23, 195, 25, 72, 23, 131, 25, 195, 134, 23, 218, 219, 195, 82, 144, 218, 219, 218, 219, 218, 219, 195, 218, 219, 218, 219, 60, 23, 195, 25, 218, 219, 101, 195, 117, 218, 219, 195, 107, 108, 23, 195, 25, 195, 218, 219, 115, 228, 117, 118, 119, 218, 219, 122, 195, 19, 218, 219, 195, 60, 218, 219, 142, 195, 218, 219, 19, 20, 195, 22, 139, 140, 23, 23, 25, 25, 195, 218, 219, 7, 8, 218, 219, 36, 118, 154, 155, 156, 157, 158, 195, 23, 195, 25, 84, 85, 49, 195, 23, 195, 25, 195, 23, 195, 25, 195, 23, 60, 25, 23, 23, 25, 25, 142, 183, 218, 219, 118, 195, 72, 218, 219, 218, 219, 218, 219, 218, 219, 218, 219, 195, 195, 146, 86, 98, 23, 195, 25, 91, 19, 20, 154, 22, 156, 154, 23, 156, 25, 101, 23, 195, 25, 195, 195, 107, 108, 36, 195, 195, 195, 195, 228, 115, 195, 117, 118, 119, 195, 195, 122, 261, 195, 321, 195, 195, 195, 258, 238, 195, 195, 60, 299, 291, 195, 195, 258, 195, 195, 195, 290, 244, 216, 72, 245, 193, 258, 258, 299, 258, 299, 274, 154, 155, 156, 157, 158, 86, 247, 295, 248, 295, 91, 19, 20, 270, 22, 274, 270, 248, 274, 222, 101, 227, 274, 221, 231, 221, 107, 108, 36, 183, 262, 247, 221, 283, 115, 262, 117, 118, 119, 198, 116, 122, 220, 262, 61, 220, 220, 251, 247, 142, 251, 245, 60, 202, 299, 202, 38, 262, 202, 22, 152, 151, 296, 43, 72, 236, 18, 239, 202, 239, 239, 239, 18, 154, 155, 156, 157, 158, 86, 150, 201, 248, 275, 91, 248, 273, 236, 248, 275, 275, 273, 236, 248, 101, 286, 202, 201, 159, 63, 107, 108, 296, 183, 293, 202, 201, 22, 115, 202, 117, 118, 119, 292, 223, 122, 201, 65, 202, 201, 223, 220, 220, 22, 220, 226, 226, 229, 127, 223, 220, 166, 24, 285, 220, 222, 114, 315, 285, 220, 202, 220, 307, 92, 320, 320, 229, 154, 155, 156, 157, 158, 0, 1, 2, 223, 83, 5, 268, 149, 268, 146, 10, 11, 12, 13, 14, 22, 280, 17, 202, 159, 19, 20, 251, 22, 183, 282, 148, 252, 252, 250, 30, 249, 32, 248, 147, 25, 13, 36, 204, 196, 40, 196, 6, 302, 194, 194, 194, 209, 215, 209, 215, 215, 215, 224, 224, 216, 209, 4, 216, 215, 3, 60, 22, 122, 19, 122, 19, 125, 22, 15, 22, 71, 16, 72, 23, 23, 140, 305, 152, 79, 25, 131, 82, 143, 20, 16, 305, 1, 143, 145, 131, 131, 62, 54, 131, 37, 54, 54, 152, 99, 117, 34, 101, 54, 24, 1, 5, 22, 107, 108, 116, 76, 25, 162, 41, 142, 115, 24, 117, 118, 119, 116, 20, 122, 19, 126, 23, 132, 19, 20, 69, 22, 69, 22, 134, 22, 68, 22, 22, 139, 140, 60, 141, 68, 24, 36, 28, 97, 22, 37, 68, 23, 150, 34, 22, 154, 155, 156, 157, 158, 23, 23, 22, 163, 25, 23, 142, 23, 98, 60, 23, 22, 144, 25, 76, 34, 117, 34, 89, 34, 34, 72, 87, 76, 183, 34, 94, 34, 23, 22, 24, 34, 23, 25, 44, 25, 23, 23, 23, 22, 22, 25, 11, 143, 25, 143, 23, 22, 22, 22, 101, 23, 23, 136, 22, 25, 107, 108, 142, 25, 142, 142, 23, 15, 115, 1, 117, 118, 119, 1, 2, 122, 1, 5, 322, 322, 322, 322, 10, 11, 12, 13, 14, 322, 322, 17, 322, 5, 322, 322, 141, 322, 10, 11, 12, 13, 14, 322, 30, 17, 32, 322, 322, 154, 155, 156, 157, 158, 40, 322, 322, 322, 30, 322, 32, 322, 322, 322, 322, 322, 322, 322, 40, 322, 322, 322, 322, 322, 322, 322, 322, 322, 183, 322, 322, 322, 322, 322, 322, 71, 322, 322, 322, 322, 322, 322, 322, 79, 322, 322, 82, 322, 322, 71, 322, 322, 322, 322, 322, 322, 322, 79, 322, 322, 82, 322, 322, 99, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 99, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 134, 322, 322, 322, 322, 139, 140, 322, 322, 322, 322, 322, 322, 322, 134, 322, 322, 322, 322, 139, 140, 322, 322, 322, 322, 322, 322, 322, 322, 163, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 163, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 322, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187, 187,
};

const unsigned short int yy_shift_ofst[] = {
    2201, 1973, 2215, 1552, 1552, 33, 368, 1668, 1741, 1814, 726, 726, 726, 265, 33, 33, 33, 33, 33, 0, 0, 216, 1349, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 272, 272, 111, 111, 316, 365, 516, 867, 867, 916, 916, 916, 916, 40, 112, 260, 364, 408, 512, 617, 661, 765, 809, 913, 957, 1061, 1081, 1195, 1215, 1329, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1349, 1369, 1349, 1473, 1493, 1493, 473, 1974, 2082, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 726, 138, 232, 232, 232, 232, 232, 232, 232, 188, 99, 242, 718, 416, 1159, 867, 867, 940, 940, 867, 1103, 417, 574, 574, 574, 611, 139, 139, 2379, 2379, 1026, 1026, 1026, 536, 466, 466, 466, 466, 1017, 1017, 849, 718, 971, 1060, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 261, 712, 712, 867, 108, 1142, 1142, 977, 1108, 1108, 977, 977, 1243, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 641, 789, 789, 635, 366, 721, 673, 782, 494, 787, 829, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 959, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 867, 820, 820, 820, 867, 867, 867, 1136, 867, 867, 867, 1119, 1007, 867, 1169, 867, 867, 867, 867, 867, 867, 867, 867, 1225, 1153, 869, 196, 618, 618, 618, 618, 1491, 196, 196, 91, 339, 1326, 1386, 383, 1163, 1364, 1426, 1364, 1538, 903, 1163, 1163, 903, 1163, 1426, 1538, 1018, 1535, 1241, 1528, 1528, 1528, 1394, 1394, 1394, 1394, 762, 762, 1403, 1466, 1475, 1551, 1746, 1805, 1746, 1746, 1729, 1729, 1840, 1840, 1729, 1730, 1732, 1859, 1842, 1870, 1870, 1870, 1870, 1729, 1876, 1751, 1732, 1732, 1751, 1859, 1842, 1751, 1842, 1751, 1729, 1876, 1760, 1857, 1729, 1876, 1906, 1729, 1876, 1729, 1876, 1906, 1746, 1746, 1746, 1873, 1922, 1922, 1906, 1746, 1822, 1746, 1873, 1746, 1746, 1786, 1929, 1843, 1843, 1906, 1729, 1872, 1872, 1894, 1894, 1831, 1836, 1966, 1729, 1833, 1831, 1851, 1860, 1751, 1983, 1996, 1996, 2009, 2009, 2009, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 2379, 136, 1063, 1196, 530, 636, 1274, 1300, 1443, 1598, 1495, 1479, 967, 1083, 1602, 463, 1625, 1638, 1670, 1541, 1671, 1689, 1696, 1277, 1432, 1693, 808, 1700, 1607, 1657, 1587, 1704, 1707, 1631, 1708, 1733, 1608, 1611, 1743, 1747, 1620, 1592, 2026, 2030, 2013, 1914, 2018, 1916, 2020, 2019, 2021, 1915, 2027, 2029, 2024, 2025, 1909, 1899, 1923, 2028, 2028, 1913, 2037, 1917, 2042, 2059, 1918, 1932, 2028, 1933, 2003, 2031, 2028, 1919, 2012, 2015, 2016, 2022, 1936, 1956, 2040, 2053, 2077, 2074, 2058, 1967, 1924, 2034, 2060, 2036, 2008, 2046, 1946, 1978, 2066, 2075, 2078, 1968, 1972, 2084, 2041, 2086, 2088, 2076, 2089, 2048, 2054, 2093, 2023, 2091, 2099, 2055, 2085, 2101, 2092, 1975, 2105, 2110, 2111, 2112, 2115, 2113, 2043, 1997, 2117, 2120, 2032, 2114, 2122, 2001, 2121, 2116, 2118, 2119, 2124, 2062, 2071, 2068, 2123, 2080, 2065, 2126, 2138, 2140, 2139, 2141, 2143, 2130, 2033, 2035, 2142, 2121, 2146, 2147, 2148, 2150, 2149, 2152, 2156, 2151, 2164, 2158, 2159, 2161, 2162, 2160, 2165, 2163, 2050, 2049, 2051, 2052, 2167, 2172, 2181, 2197, 2204,
};

const short yy_reduce_ofst[] = {
    -67, 345, -64, -178, -181, 143, 435, -78, -183, 163, -185, 284, 384, -174, 189, 352, 440, 444, 493, -23, 227, -277, -1, 305, 561, 755, 759, 764, -189, 839, 857, 354, 484, 859, 631, 67, 734, 780, -187, 616, 581, 730, 891, 449, 588, 795, 836, -238, 287, -238, 287, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, 205, 582, 715, 958, 985, 1003, 1005, 1010, 1012, 1059, 1066, 1092, 1094, 1097, 1122, 1137, 1141, 1143, 1147, 1151, 1172, 1249, 1251, 1269, 1271, 1276, 1290, 1316, 1318, 1337, 1371, 1373, 1375, 1400, 1413, 1418, 1420, 1422, 1425, 1427, 1433, 1438, 1447, 1454, 1459, 1463, 1467, 1480, 1484, 1518, 1523, 1525, 1527, 1529, 1531, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, 155, 210, -220, 86, -130, 943, 996, 402, -256, -113, 981, 1095, 1135, 395, -256, -256, -256, -256, 568, 568, 568, -4, -153, -133, 259, 306, -166, 523, -303, -126, 503, 503, -37, -149, 164, 690, 292, 412, 492, 651, 784, 332, 786, 841, 1149, 833, 1236, 792, 162, 796, 1253, 777, 288, 381, 380, 709, 487, 1027, 972, 1030, 1084, 991, 1120, -152, 1062, 692, 1240, 1247, 1250, 1239, 1306, -207, -194, 57, 180, 74, 315, 355, 376, 452, 488, 630, 693, 965, 1004, 1025, 1099, 1154, 1289, 1305, 1310, 1469, 1489, 984, 1494, 1502, 1516, 1544, 1556, 1557, 1562, 1576, 1578, 1579, 1583, 1584, 1585, 1586, 1217, 1440, 1554, 1589, 1593, 1594, 1530, 1597, 1599, 1600, 1539, 1472, 1601, 1560, 1604, 355, 1605, 1609, 1610, 1612, 1613, 1614, 1503, 1512, 1520, 1567, 1548, 1558, 1559, 1561, 1530, 1567, 1567, 1569, 1596, 1622, 1519, 1521, 1547, 1565, 1581, 1568, 1534, 1582, 1563, 1566, 1591, 1570, 1606, 1536, 1619, 1615, 1616, 1624, 1626, 1633, 1590, 1595, 1603, 1617, 1618, 1621, 1572, 1623, 1628, 1663, 1644, 1577, 1647, 1648, 1673, 1675, 1588, 1627, 1678, 1630, 1629, 1634, 1651, 1650, 1652, 1653, 1654, 1688, 1701, 1655, 1635, 1636, 1658, 1639, 1672, 1661, 1677, 1666, 1715, 1717, 1632, 1642, 1724, 1726, 1712, 1728, 1736, 1737, 1739, 1718, 1722, 1723, 1725, 1719, 1720, 1721, 1727, 1731, 1734, 1735, 1738, 1740, 1742, 1643, 1656, 1669, 1674, 1753, 1759, 1645, 1646, 1711, 1713, 1748, 1744, 1709, 1789, 1716, 1749, 1752, 1755, 1758, 1807, 1816, 1818, 1823, 1824, 1825, 1745, 1754, 1714, 1811, 1806, 1808, 1809, 1810, 1813, 1802, 1803, 1812, 1815, 1817, 1820,
};

const unsigned short int yy_default[] = {
    1691, 1691, 1691, 1516, 1279, 1392, 1279, 1279, 1279, 1279, 1516, 1516, 1516, 1279, 1279, 1279, 1279, 1279, 1279, 1422, 1422, 1568, 1312, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1515, 1279, 1279, 1279, 1279, 1607, 1607, 1279, 1279, 1279, 1279, 1279, 1592, 1591, 1279, 1279, 1279, 1431, 1279, 1279, 1279, 1438, 1279, 1279, 1279, 1279, 1279, 1517, 1518, 1279, 1279, 1279, 1279, 1567, 1569, 1533, 1445, 1444, 1443, 1442, 1551, 1410, 1436, 1429, 1433, 1512, 1513, 1511, 1670, 1518, 1517, 1279, 1432, 1480, 1496, 1479, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1488, 1495, 1494, 1493, 1502, 1492, 1489, 1482, 1481, 1483, 1484, 1303, 1300, 1354, 1279, 1279, 1279, 1279, 1279, 1485, 1312, 1473, 1472, 1471, 1279, 1499, 1486, 1498, 1497, 1575, 1644, 1643, 1534, 1279, 1279, 1279, 1279, 1279, 1279, 1607, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1412, 1607, 1607, 1279, 1312, 1607, 1607, 1308, 1413, 1413, 1308, 1308, 1416, 1587, 1383, 1383, 1383, 1383, 1392, 1383, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1572, 1570, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1388, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1637, 1683, 1279, 1546, 1368, 1388, 1388, 1388, 1388, 1390, 1369, 1367, 1382, 1313, 1286, 1683, 1683, 1448, 1437, 1389, 1437, 1680, 1435, 1448, 1448, 1435, 1448, 1389, 1680, 1329, 1659, 1324, 1422, 1422, 1422, 1412, 1412, 1412, 1412, 1416, 1416, 1514, 1389, 1382, 1279, 1355, 1683, 1355, 1355, 1398, 1398, 1682, 1682, 1398, 1534, 1667, 1457, 1357, 1363, 1363, 1363, 1363, 1398, 1297, 1435, 1667, 1667, 1435, 1457, 1357, 1435, 1357, 1435, 1398, 1297, 1550, 1678, 1398, 1297, 1524, 1398, 1297, 1398, 1297, 1524, 1355, 1355, 1355, 1344, 1279, 1279, 1524, 1355, 1329, 1355, 1344, 1355, 1355, 1625, 1279, 1528, 1528, 1524, 1398, 1617, 1617, 1425, 1425, 1430, 1416, 1519, 1398, 1279, 1430, 1428, 1426, 1435, 1347, 1640, 1640, 1636, 1636, 1636, 1688, 1688, 1587, 1652, 1312, 1312, 1312, 1312, 1652, 1331, 1331, 1313, 1313, 1312, 1652, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1647, 1279, 1279, 1535, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1402, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1593, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1462, 1279, 1282, 1584, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1439, 1440, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1454, 1279, 1279, 1279, 1449, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1403, 1279, 1279, 1279, 1279, 1279, 1279, 1549, 1548, 1279, 1279, 1400, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1327, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1427, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1622, 1417, 1279, 1279, 1279, 1279, 1671, 1279, 1279, 1279, 1279, 1377, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1279, 1663, 1371, 1463, 1279, 1466, 1301, 1279, 1291, 1279, 1279,
};

const unsigned short int yyFallback[] = {
    0, 0, 60, 60, 60, 60, 0, 60, 60, 60, 0, 60, 60, 60, 60, 0, 0, 0, 60, 0, 0, 60, 0, 0, 0, 0, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 0, 0, 0, 0, 60, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

unsigned short int yy_find_shift_action(unsigned short int iLookAhead, unsigned short int stateno) {
  int i;

  if (stateno > 599)
    return stateno;

  do {
    i = yy_shift_ofst[stateno];

    ((void)(0))

        ;

    ((void)(0))

        ;

    ((void)(0))

        ;

    ((void)(0))

        ;

    ((void)(0))

        ;
    i += iLookAhead;

    ((void)(0))

        ;
    if (yy_lookahead[i] != iLookAhead) {

      unsigned short int iFallback;

      ((void)(0))

          ;
      iFallback = yyFallback[iLookAhead];
      if (iFallback != 0) {

        ((void)(0))

            ;
        iLookAhead = iFallback;
        continue;
      }

      {
        int j = i - iLookAhead + 102;

        ((void)(0))

            ;
        if (yy_lookahead[j] == 102 && iLookAhead > 0) {

          return yy_action[j];
        }
      }

      return yy_default[stateno];
    } else {

      ((void)(0))

          ;
      return yy_action[i];
    }
  } while (1);
}

unsigned short int yy_find_reduce_action(unsigned short int stateno, unsigned short int iLookAhead) {
  int i;

  i = yy_reduce_ofst[stateno];

  i += iLookAhead;

  return yy_action[i];
}

const unsigned short int yyRuleInfoLhs[] = {
    191, 191, 190, 192, 193, 193, 193, 193, 192, 192, 192, 192, 192, 197, 199, 201, 201, 200, 200, 198, 198, 205, 205, 207, 207, 208, 210, 210, 210, 211, 215, 216, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 217, 226, 226, 222, 222, 224, 224, 227, 227, 227, 227, 228, 228, 228, 228, 228, 225, 225, 229, 229, 229, 204, 231, 232, 232, 232, 232, 232, 235, 220, 220, 236, 236, 237, 237, 192, 239, 239, 192, 192, 192, 206, 206, 206, 241, 244, 244, 244, 242, 242, 254, 242, 256, 256, 245, 245, 245, 257, 246, 246, 246, 258, 258, 247, 247, 260, 260, 259, 259, 259, 259, 259, 202, 202, 240, 240, 265, 265, 265, 265, 261, 261, 261, 261, 262, 262, 262, 267, 263, 263, 251, 251, 233, 233, 221, 221, 221, 268, 268, 268, 249, 249, 250, 250, 252, 252, 252, 252, 192, 248, 248, 270, 270, 270, 270, 192, 271, 271, 271, 271, 192, 192, 274, 274, 274, 274, 274, 274, 275, 272, 272, 273, 273, 266, 266, 219, 219, 219, 219, 218, 218, 218, 219, 219, 219, 219, 219, 219, 219, 219, 219, 218, 219, 219, 219, 219, 219, 219, 219, 219, 219, 277, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 278, 278, 219, 279, 279, 219, 219, 219, 219, 219, 219, 282, 282, 283, 283, 281, 264, 255, 255, 280, 280, 192, 284, 284, 223, 223, 234, 234, 285, 285, 192, 192, 192, 286, 286, 192, 192, 192, 192, 192, 213, 214, 192, 288, 290, 290, 290, 291, 291, 291, 293, 293, 289, 289, 295, 295, 294, 294, 294, 294, 219, 219, 238, 238, 238, 192, 192, 192, 297, 297, 192, 192, 192, 192, 192, 192, 298, 192, 192, 192, 192, 192, 192, 192, 192, 192, 300, 302, 303, 303, 304, 269, 269, 307, 307, 307, 306, 308, 243, 243, 309, 310, 311, 311, 311, 311, 311, 312, 312, 312, 316, 318, 318, 319, 319, 317, 317, 320, 320, 321, 321, 321, 253, 276, 276, 276, 315, 315, 314, 218, 187, 188, 188, 189, 189, 189, 194, 194, 194, 196, 196, 192, 205, 203, 203, 195, 195, 210, 211, 212, 212, 209, 209, 217, 217, 217, 204, 230, 230, 231, 235, 237, 241, 242, 257, 258, 267, 275, 219, 277, 281, 264, 287, 287, 287, 287, 287, 213, 292, 292, 295, 296, 296, 299, 299, 301, 301, 302, 305, 305, 305, 269, 309, 311,
};

const signed char yyRuleInfoNRhs[] = {
    -1, -3, -1, -3, 0, -1, -1, -1, -2, -2, -2, -3, -5, -6, -1, 0, -3, -1, 0, -5, -2, 0, -3, -2, -1, -2, 0, -4, -6, -2, 0, 0, -2, -3, -4, -4, -4, -3, -3, -5, -2, -4, -4, -1, -2, -3, -4, 0, -1, 0, -2, -2, -3, -3, -3, -2, -2, -1, -1, -2, -3, -2, 0, -2, -2, 0, -1, -2, -7, -5, -5, -10, 0, 0, -3, 0, -2, -1, -1, -4, -2, 0, -9, -4, -1, -3, -4, -1, -3, -1, -2, -1, -9, -10, -4, -1, -5, -5, -1, -1, 0, 0, -5, -3, -5, -2, 0, 0, -2, -2, 0, -5, -6, -8, -6, -6, 0, -2, -1, -3, -1, -3, -3, -5, -1, -2, -3, -4, -2, -4, 0, 0, -3, -2, 0, -3, -5, -3, -1, -1, 0, -2, -2, 0, 0, -3, 0, -2, 0, -2, -4, -4, -6, 0, -2, 0, -2, -2, -4, -9, -5, -7, -3, -5, -7, -8, 0, -2, -12, -9, -5, -8, -2, -2, -1, 0, -3, -3, -1, -3, -1, -3, -5, -1, -1, -1, -1, -3, -6, -5, -8, -4, -6, -9, -5, -1, -5, -3, -3, -3, -3, -3, -3, -3, -3, -2, -3, -5, -2, -3, -3, -4, -6, -5, -2, -2, -2, -3, -1, -2, -5, -1, -2, -5, -3, -5, -5, -4, -5, -5, -4, -2, 0, 0, 0, -3, -1, 0, -3, -12, -1, 0, 0, -3, -5, -3, 0, -2, -4, -2, -3, -2, 0, -3, -5, -6, -5, -6, -2, -2, -5, -11, -1, -2, 0, -1, -1, -3, 0, -2, -3, -2, -3, -2, -9, -8, -6, -3, -4, -6, -1, -1, -1, -4, -6, -3, 0, -2, -1, -3, -1, -3, -6, -2, -7, -6, -8, -6, -9, -10, -11, -9, -1, -4, -8, 0, -1, -3, -1, -2, -3, -1, -2, -3, -6, -1, -1, -3, -3, -5, -5, -6, -4, -5, -2, 0, -3, -6, -1, -1, -2, -1, -2, -2, -2, 0, -2, -2, -2, -1, -2, -2, -1, -1, -4, -2, -5, -1, -1, -2, -1, -1, -2, -3, 0, -1, -2, -1, 0, -2, -1, -4, -2, -1, -1, -1, -1, -1, -1, -2, 0, -2, -4, -2, -2, -3, -1, 0, -1, -1, -1, -1, -2, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -3, 0, -1, 0, 0, -1, -1, -3, -2, 0, -4, -2, 0, -1, -1,
};

const unsigned char aiClass[] = {

    29, 28, 28, 28, 28, 28, 28, 28, 28, 7,  7,  28, 7,  7,  28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 7,  15, 8,  5,  4,  22, 24, 8,  17, 18, 21, 20, 23, 11, 26, 16, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  5,  19, 12, 14, 13, 6,  5,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  2,  2,  9,  28, 28, 28, 2,  8,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  2,  2,  28, 10, 28, 25, 28, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 30, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27

};

const char zKWText[666] = {
    'R', 'E', 'I', 'N', 'D', 'E', 'X', 'E', 'D', 'E', 'S', 'C', 'A', 'P', 'E', 'A', 'C', 'H', 'E', 'C', 'K', 'E', 'Y', 'B', 'E', 'F', 'O', 'R', 'E', 'I', 'G', 'N', 'O', 'R', 'E', 'G', 'E', 'X', 'P', 'L', 'A', 'I', 'N', 'S', 'T', 'E', 'A', 'D', 'D', 'A', 'T', 'A', 'B', 'A', 'S', 'E', 'L', 'E', 'C', 'T', 'A', 'B', 'L', 'E', 'F', 'T', 'H', 'E', 'N', 'D', 'E', 'F', 'E', 'R', 'R', 'A', 'B', 'L', 'E', 'L', 'S', 'E', 'X', 'C', 'L', 'U', 'D', 'E', 'L', 'E', 'T', 'E', 'M', 'P', 'O', 'R', 'A', 'R', 'Y', 'I', 'S', 'N', 'U', 'L', 'L', 'S', 'A', 'V', 'E', 'P', 'O', 'I', 'N', 'T', 'E', 'R', 'S', 'E', 'C', 'T', 'I', 'E', 'S', 'N', 'O', 'T', 'N', 'U', 'L', 'L', 'I', 'K', 'E', 'X', 'C', 'E', 'P', 'T', 'R', 'A', 'N', 'S', 'A', 'C', 'T', 'I', 'O', 'N', 'A', 'T', 'U', 'R', 'A', 'L', 'T', 'E', 'R', 'A', 'I', 'S', 'E', 'X', 'C', 'L', 'U', 'S', 'I', 'V', 'E', 'X', 'I', 'S', 'T', 'S', 'C', 'O', 'N', 'S', 'T', 'R', 'A', 'I', 'N', 'T', 'O', 'F', 'F', 'S', 'E', 'T', 'R', 'I', 'G', 'G', 'E', 'R', 'A', 'N', 'G', 'E', 'N', 'E', 'R', 'A', 'T', 'E', 'D', 'E', 'T', 'A', 'C', 'H', 'A', 'V', 'I', 'N', 'G', 'L', 'O', 'B', 'E', 'G', 'I', 'N', 'N', 'E', 'R', 'E', 'F', 'E', 'R', 'E', 'N', 'C', 'E', 'S', 'U', 'N', 'I', 'Q', 'U', 'E', 'R', 'Y', 'W', 'I', 'T', 'H', 'O', 'U', 'T', 'E', 'R', 'E', 'L', 'E', 'A', 'S', 'E', 'A', 'T', 'T', 'A', 'C', 'H', 'B', 'E', 'T', 'W', 'E', 'E', 'N', 'O', 'T', 'H', 'I', 'N', 'G', 'R', 'O', 'U', 'P', 'S', 'C', 'A', 'S', 'C', 'A', 'D', 'E', 'F', 'A', 'U', 'L', 'T', 'C', 'A', 'S', 'E', 'C', 'O', 'L', 'L', 'A', 'T', 'E', 'C', 'R', 'E', 'A', 'T', 'E', 'C', 'U', 'R', 'R', 'E', 'N', 'T', '_', 'D', 'A', 'T', 'E', 'I', 'M', 'M', 'E', 'D', 'I', 'A', 'T', 'E', 'J', 'O', 'I', 'N', 'S', 'E', 'R', 'T', 'M', 'A', 'T', 'C', 'H', 'P', 'L', 'A', 'N', 'A', 'L', 'Y', 'Z', 'E', 'P', 'R', 'A', 'G', 'M', 'A', 'T', 'E', 'R', 'I', 'A', 'L', 'I', 'Z', 'E', 'D', 'E', 'F', 'E', 'R', 'R', 'E', 'D', 'I', 'S', 'T', 'I', 'N', 'C', 'T', 'U', 'P', 'D', 'A', 'T', 'E', 'V', 'A', 'L', 'U', 'E', 'S', 'V', 'I', 'R', 'T', 'U', 'A', 'L', 'W', 'A', 'Y', 'S', 'W', 'H', 'E', 'N', 'W', 'H', 'E', 'R', 'E', 'C', 'U', 'R', 'S', 'I', 'V', 'E', 'A', 'B', 'O', 'R', 'T', 'A', 'F', 'T', 'E', 'R', 'E', 'N', 'A', 'M', 'E', 'A', 'N', 'D', 'R', 'O', 'P', 'A', 'R', 'T', 'I', 'T', 'I', 'O', 'N', 'A', 'U', 'T', 'O', 'I', 'N', 'C', 'R', 'E', 'M', 'E', 'N', 'T', 'C', 'A', 'S', 'T', 'C', 'O', 'L', 'U', 'M', 'N', 'C', 'O', 'M', 'M', 'I', 'T', 'C', 'O', 'N', 'F', 'L', 'I', 'C', 'T', 'C', 'R', 'O', 'S', 'S', 'C', 'U', 'R', 'R', 'E', 'N', 'T', '_', 'T', 'I', 'M', 'E', 'S', 'T', 'A', 'M', 'P', 'R', 'E', 'C', 'E', 'D', 'I', 'N', 'G', 'F', 'A', 'I', 'L', 'A', 'S', 'T', 'F', 'I', 'L', 'T', 'E', 'R', 'E', 'P', 'L', 'A', 'C', 'E', 'F', 'I', 'R', 'S', 'T', 'F', 'O', 'L', 'L', 'O', 'W', 'I', 'N', 'G', 'F', 'R', 'O', 'M', 'F', 'U', 'L', 'L', 'I', 'M', 'I', 'T', 'I', 'F', 'O', 'R', 'D', 'E', 'R', 'E', 'S', 'T', 'R', 'I', 'C', 'T', 'O', 'T', 'H', 'E', 'R', 'S', 'O', 'V', 'E', 'R', 'E', 'T', 'U', 'R', 'N', 'I', 'N', 'G', 'R', 'I', 'G', 'H', 'T', 'R', 'O', 'L', 'L', 'B', 'A', 'C', 'K', 'R', 'O', 'W', 'S', 'U', 'N', 'B', 'O', 'U', 'N', 'D', 'E', 'D', 'U', 'N', 'I', 'O', 'N', 'U', 'S', 'I', 'N', 'G', 'V', 'A', 'C', 'U', 'U', 'M', 'V', 'I', 'E', 'W', 'I', 'N', 'D', 'O', 'W', 'B', 'Y', 'I', 'N', 'I', 'T', 'I', 'A', 'L', 'L', 'Y', 'P', 'R', 'I', 'M', 'A', 'R', 'Y',
};

const unsigned char aKWHash[127] = {
    84, 92, 134, 82, 105, 29, 0, 0, 94, 0, 85, 72, 0, 53, 35, 86, 15, 0, 42, 97, 54, 89, 135, 19, 0, 0, 140, 0, 40, 129, 0, 22, 107, 0, 9, 0, 0, 123, 80, 0, 78, 6, 0, 65, 103, 147, 0, 136, 115, 0, 0, 48, 0, 90, 24, 0, 17, 0, 27, 70, 23, 26, 5, 60, 142, 110, 122, 0, 73, 91, 71, 145, 61, 120, 74, 0, 49, 0, 11, 41, 0, 113, 0, 0, 0, 109, 10, 111, 116, 125, 14, 50, 124, 0, 100, 0, 18, 121, 144, 56, 130, 139, 88, 83, 37, 30, 126, 0, 0, 108, 51, 131, 128, 0, 34, 0, 0, 132, 0, 98, 38, 39, 0, 20, 45, 117, 93,
};

const unsigned char aKWNext[148] = {
    0, 0, 0, 0, 0, 4, 0, 43, 0, 0, 106, 114, 0, 0, 0, 2, 0, 0, 143, 0, 0, 0, 13, 0, 0, 0, 0, 141, 0, 0, 119, 52, 0, 0, 137, 12, 0, 0, 62, 0, 138, 0, 133, 0, 0, 36, 0, 0, 28, 77, 0, 0, 0, 0, 59, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 69, 0, 0, 0, 0, 0, 146, 3, 0, 58, 0, 1, 75, 0, 0, 0, 31, 0, 0, 0, 0, 0, 127, 0, 104, 0, 64, 66, 63, 0, 0, 0, 0, 0, 46, 0, 16, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 101, 0, 112, 21, 7, 67, 0, 79, 96, 118, 0, 0, 68, 0, 0, 99, 44, 0, 55, 0, 76, 0, 95, 32, 33, 57, 25, 0, 102, 0, 0, 87,
};

const unsigned char aKWLen[148] = {
    0, 7, 7, 5, 4, 6, 4, 5, 3, 6, 7, 3, 6, 6, 7, 7, 3, 8, 2, 6, 5, 4, 4, 3, 10, 4, 7, 6, 9, 4, 2, 6, 5, 9, 9, 4, 7, 3, 2, 4, 4, 6, 11, 6, 2, 7, 5, 5, 9, 6, 10, 4, 6, 2, 3, 7, 5, 9, 6, 6, 4, 5, 5, 10, 6, 5, 7, 4, 5, 7, 6, 7, 7, 6, 5, 7, 3, 7, 4, 7, 6, 12, 9, 4, 6, 5, 4, 7, 6, 12, 8, 8, 2, 6, 6, 7, 6, 4, 5, 9, 5, 5, 6, 3, 4, 9, 13, 2, 2, 4, 6, 6, 8, 5, 17, 12, 7, 9, 4, 4, 6, 7, 5, 9, 4, 4, 5, 2, 5, 8, 6, 4, 9, 5, 8, 4, 3, 9, 5, 5, 6, 4, 6, 2, 2, 9, 3, 7,
};

const unsigned short int aKWOffset[148] = {
    0, 0, 2, 2, 8, 9, 14, 16, 20, 23, 25, 25, 29, 33, 36, 41, 46, 48, 53, 54, 59, 62, 65, 67, 69, 78, 81, 86, 90, 90, 94, 99, 101, 105, 111, 119, 123, 123, 123, 126, 129, 132, 137, 142, 146, 147, 152, 156, 160, 168, 174, 181, 184, 184, 187, 189, 195, 198, 206, 211, 216, 219, 222, 226, 236, 239, 244, 244, 248, 252, 259, 265, 271, 277, 277, 283, 284, 288, 295, 299, 306, 312, 324, 333, 335, 341, 346, 348, 355, 359, 370, 377, 378, 385, 391, 397, 402, 408, 412, 415, 424, 429, 433, 439, 441, 444, 453, 455, 457, 466, 470, 476, 482, 490, 495, 495, 495, 511, 520, 523, 527, 532, 539, 544, 553, 557, 560, 565, 567, 571, 579, 585, 588, 597, 602, 610, 610, 614, 623, 628, 633, 639, 642, 645, 648, 650, 655, 659,
};

const unsigned char aKWCode[148] = {
    0, 99, 117, 162, 39, 59, 41, 125, 68, 33, 133, 63, 64, 48, 2, 66, 164, 38, 24, 139, 16, 119, 160, 11, 132, 161, 92, 129, 21, 21, 43, 51, 83, 13, 138, 95, 52, 19, 67, 122, 48, 137, 6, 28, 116, 119, 163, 72, 9, 20, 120, 152, 70, 69, 131, 78, 90, 96, 40, 148, 48, 5, 119, 126, 124, 3, 26, 82, 119, 14, 32, 49, 153, 93, 147, 35, 31, 121, 158, 114, 17, 101, 8, 144, 128, 47, 4, 30, 71, 98, 7, 141, 45, 130, 140, 81, 97, 159, 150, 73, 27, 29, 100, 44, 134, 88, 127, 15, 50, 36, 61, 10, 37, 119, 101, 101, 86, 89, 42, 85, 167, 74, 84, 87, 143, 119, 149, 18, 146, 75, 94, 166, 151, 119, 12, 77, 76, 91, 135, 145, 79, 80, 165, 62, 34, 65, 136, 123,
};

i64 keywordCode(const char *z, i64 n, int *pType) {
  i64 i, j;
  const char *zKW;

  i = ((sqlite3UpperToLower[(unsigned char)z[0]] * 4) ^ (sqlite3UpperToLower[(unsigned char)z[n - 1]] * 3) ^ n * 1) % 127;
  for (i = (int)aKWHash[i]; i > 0; i = aKWNext[i]) {
    if (aKWLen[i] != n)
      continue;
    zKW = &zKWText[aKWOffset[i]];

    if ((z[0] & ~0x20) != zKW[0])
      continue;
    if ((z[1] & ~0x20) != zKW[1])
      continue;
    j = 2;
    while (j < n && (z[j] & ~0x20) == zKW[j]) {
      j++;
    }

    if (j < n)
      continue;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    *pType = aKWCode[i];
    break;
  }
  return n;
}

int getToken(const unsigned char **pz) {
  const unsigned char *z = *pz;
  int t;
  do {
    z += sqlite3GetToken(z, &t);
  } while (t == 184 || t == 185);
  if (t == 60 || t == 118 || t == 119 || t == 165 || t == 166 || sqlite3ParserFallback(t) == 60) {
    t = 60;
  }
  *pz = z;
  return t;
}

int analyzeWindowKeyword(const unsigned char *z) {
  int t;
  t = getToken(&z);
  if (t != 60)
    return 60;
  t = getToken(&z);
  if (t != 24)
    return 60;
  return 165;
}

int analyzeOverKeyword(const unsigned char *z, int lastToken) {
  if (lastToken == 23) {
    int t = getToken(&z);
    if (t == 22 || t == 60)
      return 166;
  }
  return 60;
}

int analyzeFilterKeyword(const unsigned char *z, int lastToken) {
  if (lastToken == 23 && getToken(&z) == 22) {
    return 167;
  }
  return 60;
}

int binCollFunc(void *NotUsed, int nKey1, const void *pKey1, int nKey2, const void *pKey2) {
  int rc, n;
  (void)(NotUsed);
  n = nKey1 < nKey2 ? nKey1 : nKey2;

  rc = memcmp(pKey1, pKey2, n);
  if (rc == 0) {
    rc = nKey1 - nKey2;
  }
  return rc;
}

int rtrimCollFunc(void *pUser, int nKey1, const void *pKey1, int nKey2, const void *pKey2) {
  const u8 *pK1 = (const u8 *)pKey1;
  const u8 *pK2 = (const u8 *)pKey2;
  while (nKey1 && pK1[nKey1 - 1] == ' ')
    nKey1--;
  while (nKey2 && pK2[nKey2 - 1] == ' ')
    nKey2--;
  return binCollFunc(pUser, nKey1, pKey1, nKey2, pKey2);
}

int nocaseCollatingFunc(void *NotUsed, int nKey1, const void *pKey1, int nKey2, const void *pKey2) {
  int r = sqlite3_strnicmp((const char *)pKey1, (const char *)pKey2, (nKey1 < nKey2) ? nKey1 : nKey2);
  (void)(NotUsed);
  if (0 == r) {
    r = nKey1 - nKey2;
  }
  return r;
}

int sqliteDefaultBusyCallback(void *ptr, int count) {

  static const u8 delays[] = {1, 2, 5, 10, 15, 20, 25, 25, 25, 50, 50, 100};
  static const u8 totals[] = {0, 1, 3, 8, 18, 33, 53, 78, 103, 128, 178, 228};

  sqlite3 *db = (sqlite3 *)ptr;
  int tmout = db->busyTimeout;
  int delay, prior;

  if (count < ((int)(sizeof(delays) / sizeof(delays[0])))) {
    delay = delays[count];
    prior = totals[count];
  } else {
    delay = delays[((int)(sizeof(delays) / sizeof(delays[0]))) - 1];
    prior = totals[((int)(sizeof(delays) / sizeof(delays[0]))) - 1] + delay * (count - (((int)(sizeof(delays) / sizeof(delays[0]))) - 1));
  }
  if (prior + delay > tmout) {
    delay = tmout - prior;
    if (delay <= 0)
      return 0;
  }
  sqlite3OsSleep(db->pVfs, delay * 1000);
  return 1;
}

const int aHardLimit[] = {
    1000000000, 1000000000, 2000, 1000, 500, 250000000, 1000, 10, 50000, 32766, 1000, 8, 2500,
};

const char *uriParameter(const char *zFilename, const char *zParam) {
  zFilename += sqlite3Strlen30(zFilename) + 1;
  while ((zFilename != 0) && zFilename[0]) {
    int x = strcmp(zFilename, zParam);
    zFilename += sqlite3Strlen30(zFilename) + 1;
    if (x == 0)
      return zFilename;
    zFilename += sqlite3Strlen30(zFilename) + 1;
  }
  return 0;
}

int openDatabase(const char *zFilename, sqlite3 **ppDb, unsigned int flags, const char *zVfs) {
  sqlite3 *db;
  int rc;
  int isThreadsafe;
  char *zOpen = 0;
  char *zErrMsg = 0;
  int i;

  *ppDb = 0;

  rc = sqlite3_initialize();
  if (rc)
    return rc;

  if (sqlite3Config.bCoreMutex == 0) {
    isThreadsafe = 0;
  } else if (flags & 0x00008000) {
    isThreadsafe = 0;
  } else if (flags & 0x00010000) {
    isThreadsafe = 1;
  } else {
    isThreadsafe = sqlite3Config.bFullMutex;
  }

  if (flags & 0x00040000) {
    flags &= ~0x00020000;
  } else if (sqlite3Config.sharedCacheEnabled) {
    flags |= 0x00020000;
  }

  flags &= ~(0x00000008 | 0x00000010 | 0x00000100 | 0x00000200 | 0x00000400 | 0x00000800 | 0x00001000 | 0x00002000 | 0x00004000 | 0x00008000 | 0x00010000 | 0x00080000);

  db = sqlite3MallocZero(sizeof(sqlite3));
  if (db == 0)
    goto opendb_out;
  if (isThreadsafe

  ) {
    db->mutex = sqlite3MutexAlloc(1);
    if (db->mutex == 0) {
      sqlite3_free(db);
      db = 0;
      goto opendb_out;
    }
    if (isThreadsafe == 0) {
      ;
    }
  }
  sqlite3_mutex_enter(db->mutex);
  db->errMask = (flags & 0x02000000) != 0 ? 0xffffffff : 0xff;
  db->nDb = 2;
  db->eOpenState = 0x6d;
  db->aDb = db->aDbStatic;
  db->lookaside.bDisable = 1;
  db->lookaside.sz = 0;
  db->nFpDigit = 17;

  memcpy(db->aLimit, aHardLimit, sizeof(db->aLimit));
  db->aLimit[11] = 0;
  db->autoCommit = 1;
  db->nextAutovac = -1;
  db->szMmap = sqlite3Config.szMmap;
  db->nextPagesize = 0;
  db->init.azInit = sqlite3StdType;

  db->flags |= 0x00000040 | 0x00040000 | 0x80000000 | 0x00000020 | ((u64)(0x00010) << 32) | ((u64)(0x00020) << 32) | ((u64)(0x00040) << 32)

               | 0x00000080

               | 0x40000000

               | 0x20000000

               | 0x00008000

      ;
  sqlite3HashInit(&db->aCollSeq);

  sqlite3HashInit(&db->aModule);

  createCollation(db, sqlite3StrBINARY, 1, 0, binCollFunc, 0);
  createCollation(db, sqlite3StrBINARY, 3, 0, binCollFunc, 0);
  createCollation(db, sqlite3StrBINARY, 2, 0, binCollFunc, 0);
  createCollation(db, "NOCASE", 1, 0, nocaseCollatingFunc, 0);
  createCollation(db, "RTRIM", 1, 0, rtrimCollFunc, 0);
  if (db->mallocFailed) {
    goto opendb_out;
  }

  db->openFlags = flags;

  ;
  ;
  ;
  if (((1 << (flags & 7)) & 0x46) == 0) {
    rc = sqlite3MisuseError(190964);
  } else {
    if (zFilename == 0)
      zFilename = ":memory:";
    rc = sqlite3ParseUri(zVfs, zFilename, &flags, &db->pVfs, &zOpen, &zErrMsg);
  }
  if (rc != 0) {
    if (rc == 7)
      sqlite3OomFault(db);
    sqlite3ErrorWithMsg(db, rc, zErrMsg ? "%s" : 0, zErrMsg);
    sqlite3_free(zErrMsg);
    goto opendb_out;
  }

  rc = sqlite3BtreeOpen(db->pVfs, zOpen, db, &db->aDb[0].pBt, 0, flags | 0x00000100);
  if (rc != 0) {
    if (rc == (10 | (12 << 8))) {
      rc = 7;
    }
    sqlite3Error(db, rc);
    goto opendb_out;
  }
  sqlite3BtreeEnter(db->aDb[0].pBt);
  db->aDb[0].pSchema = sqlite3SchemaGet(db, db->aDb[0].pBt);
  if (!db->mallocFailed) {
    sqlite3SetTextEncoding(db, ((db)->aDb[0].pSchema->enc));
  }
  sqlite3BtreeLeave(db->aDb[0].pBt);
  db->aDb[1].pSchema = sqlite3SchemaGet(db, 0);

  db->aDb[0].zDbSName = "main";
  db->aDb[0].safety_level = 2 + 1;
  db->aDb[1].zDbSName = "temp";
  db->aDb[1].safety_level = 0x01;

  db->eOpenState = 0x76;
  if (db->mallocFailed) {
    goto opendb_out;
  }

  sqlite3Error(db, 0);
  sqlite3RegisterPerConnectionBuiltinFunctions(db);
  rc = sqlite3_errcode(db);

  for (i = 0; rc == 0 && i < ((int)(sizeof(sqlite3BuiltinExtensions) / sizeof(sqlite3BuiltinExtensions[0]))); i++) {
    rc = sqlite3BuiltinExtensions[i](db);
  }

  if (rc == 0) {
    sqlite3AutoLoadExtensions(db);
    rc = sqlite3_errcode(db);
    if (rc != 0) {
      goto opendb_out;
    }
  }

  if (rc)
    sqlite3Error(db, rc);

  setupLookaside(db, 0, sqlite3Config.szLookaside, sqlite3Config.nLookaside);

  sqlite3_wal_autocheckpoint(db, 1000);

opendb_out:
  if (db) {

    ((void)(0))

        ;
    sqlite3_mutex_leave(db->mutex);
  }
  rc = sqlite3_errcode(db);

  if ((rc & 0xff) == 7) {
    sqlite3_close(db);
    db = 0;
  } else if (rc != 0) {
    db->eOpenState = 0xba;
  }
  *ppDb = db;

  sqlite3_free_filename(zOpen);
  return rc;
}

const char *databaseName(const char *zName) {
  while (zName[-1] != 0 || zName[-2] != 0 || zName[-3] != 0 || zName[-4] != 0) {
    zName--;
  }
  return zName;
}

char *appendText(char *p, const char *z) {
  size_t n = strlen(z);
  memcpy(p, z, n + 1);
  return p + n + 1;
}

const char *const jsonbType[] = {"null", "true", "false", "integer", "integer", "real", "real", "text", "text", "text", "text", "array", "object", "", "", "", ""};

const char jsonIsSpace[] = {

    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

};

const char jsonSpaces[] = "\011\012\015\040";

const char jsonIsOk[256] = {

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1

};

void jsonPrintf(int N, JsonString *p, const char *zFormat, ...) {
  va_list ap;
  if ((p->nUsed + N >= p->nAlloc) && jsonStringGrow(p, N))
    return;

  va_start(

      ap, zFormat

  )

      ;
  sqlite3_vsnprintf(N, p->zBuf + p->nUsed, zFormat, ap);

  va_end(

      ap

  )

      ;
  p->nUsed += (int)strlen(p->zBuf + p->nUsed);
}

u8 jsonHexToInt(int h) {

  h += 9 * (1 & (h >> 6));

  return (u8)(h & 0xf);
}

u32 jsonHexToInt4(const char *z) {
  u32 v;
  v = (jsonHexToInt(z[0]) << 12) + (jsonHexToInt(z[1]) << 8) + (jsonHexToInt(z[2]) << 4) + jsonHexToInt(z[3]);
  return v;
}

int jsonIs2Hex(const char *z) { return (sqlite3CtypeMap[(unsigned char)(z[0])] & 0x08) && (sqlite3CtypeMap[(unsigned char)(z[1])] & 0x08); }

int jsonIs4Hex(const char *z) { return jsonIs2Hex(z) && jsonIs2Hex(&z[2]); }

int json5Whitespace(const char *zIn) {
  int n = 0;
  const u8 *z = (u8 *)zIn;
  while (1) {
    switch (z[n]) {
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x20: {
      n++;
      break;
    }
    case '/': {
      if (z[n + 1] == '*' && z[n + 2] != 0) {
        int j;
        for (j = n + 3; z[j] != '/' || z[j - 1] != '*'; j++) {
          if (z[j] == 0)
            goto whitespace_done;
        }
        n = j + 1;
        break;
      } else if (z[n + 1] == '/') {
        int j;
        char c;
        for (j = n + 2; (c = z[j]) != 0; j++) {
          if (c == '\n' || c == '\r')
            break;
          if (0xe2 == (u8)c && 0x80 == (u8)z[j + 1] && (0xa8 == (u8)z[j + 2] || 0xa9 == (u8)z[j + 2])) {
            j += 2;
            break;
          }
        }
        n = j;
        if (z[n])
          n++;
        break;
      }
      goto whitespace_done;
    }
    case 0xc2: {
      if (z[n + 1] == 0xa0) {
        n += 2;
        break;
      }
      goto whitespace_done;
    }
    case 0xe1: {
      if (z[n + 1] == 0x9a && z[n + 2] == 0x80) {
        n += 3;
        break;
      }
      goto whitespace_done;
    }
    case 0xe2: {
      if (z[n + 1] == 0x80) {
        u8 c = z[n + 2];
        if (c < 0x80)
          goto whitespace_done;
        if (c <= 0x8a || c == 0xa8 || c == 0xa9 || c == 0xaf) {
          n += 3;
          break;
        }
      } else if (z[n + 1] == 0x81 && z[n + 2] == 0x9f) {
        n += 3;
        break;
      }
      goto whitespace_done;
    }
    case 0xe3: {
      if (z[n + 1] == 0x80 && z[n + 2] == 0x80) {
        n += 3;
        break;
      }
      goto whitespace_done;
    }
    case 0xef: {
      if (z[n + 1] == 0xbb && z[n + 2] == 0xbf) {
        n += 3;
        break;
      }
      goto whitespace_done;
    }
    default: {
      goto whitespace_done;
    }
    }
  }
whitespace_done:
  return n;
}

const struct NanInfName {
  char c1;
  char c2;
  char n;
  char eType;
  char nRepl;
  char *zMatch;
  char *zRepl;
} aNanInfName[] = {
    {'i', 'I', 3, 5, 7, "inf", "9.0e999"}, {'i', 'I', 8, 5, 7, "infinity", "9.0e999"}, {'n', 'N', 3, 0, 4, "NaN", "null"}, {'q', 'Q', 4, 0, 4, "QNaN", "null"}, {'s', 'S', 4, 0, 4, "SNaN", "null"},
};

int jsonIs4HexB(const char *z, int *pOp) {
  if (z[0] != 'u')
    return 0;
  if (!jsonIs4Hex(&z[1]))
    return 0;
  *pOp = 8;
  return 1;
}

int jsonBlobOverwrite(u8 *aOut, const u8 *aIns, u32 nIns, u32 d) {
  u32 szPayload;
  u32 i;
  u8 szHdr;

  static const u8 aType[] = {0xc0, 0xd0, 0, 0xe0, 0, 0, 0, 0xf0};

  if ((aIns[0] & 0x0f) <= 2)
    return 0;
  switch (aIns[0] >> 4) {
  default: {
    if (((1 << d) & 0x116) == 0)
      return 0;
    i = d + 1;
    szHdr = 1;
    break;
  }
  case 12: {
    if (((1 << d) & 0x8a) == 0)
      return 0;
    i = d + 2;
    szHdr = 2;
    break;
  }
  case 13: {
    if (d != 2 && d != 6)
      return 0;
    i = d + 3;
    szHdr = 3;
    break;
  }
  case 14: {
    if (d != 4)
      return 0;
    i = 9;
    szHdr = 5;
    break;
  }
  case 15: {
    return 0;
  }
  }

  aOut[0] = (aIns[0] & 0x0f) | aType[i - 2];
  memcpy(&aOut[i], &aIns[szHdr], nIns - szHdr);
  szPayload = nIns - szHdr;
  while (1) {
    i--;
    aOut[i] = szPayload & 0xff;
    if (i == 1)
      break;
    szPayload >>= 8;
  }

  return 1;
}

u32 jsonBytesToBypass(const char *z, u32 n) {
  u32 i = 0;
  while (i + 1 < n) {
    if (z[i] != '\\')
      return i;
    if (z[i + 1] == '\n') {
      i += 2;
      continue;
    }
    if (z[i + 1] == '\r') {
      if (i + 2 < n && z[i + 2] == '\n') {
        i += 3;
      } else {
        i += 2;
      }
      continue;
    }
    if (0xe2 == (u8)z[i + 1] && i + 3 < n && 0x80 == (u8)z[i + 2] && (0xa8 == (u8)z[i + 3] || 0xa9 == (u8)z[i + 3])) {
      i += 4;
      continue;
    }
    break;
  }
  return i;
}

u32 jsonUnescapeOneChar(const char *z, u32 n, u32 *piOut) {

  if (n < 2) {
    *piOut = 0x99999;
    return n;
  }
  switch ((u8)z[1]) {
  case 'u': {
    u32 v, vlo;
    if (n < 6) {
      *piOut = 0x99999;
      return n;
    }
    v = jsonHexToInt4(&z[2]);
    if ((v & 0xfc00) == 0xd800 && n >= 12 && z[6] == '\\' && z[7] == 'u' && ((vlo = jsonHexToInt4(&z[8])) & 0xfc00) == 0xdc00) {
      *piOut = ((v & 0x3ff) << 10) + (vlo & 0x3ff) + 0x10000;
      return 12;
    } else {
      *piOut = v;
      return 6;
    }
  }
  case 'b': {
    *piOut = '\b';
    return 2;
  }
  case 'f': {
    *piOut = '\f';
    return 2;
  }
  case 'n': {
    *piOut = '\n';
    return 2;
  }
  case 'r': {
    *piOut = '\r';
    return 2;
  }
  case 't': {
    *piOut = '\t';
    return 2;
  }
  case 'v': {
    *piOut = '\v';
    return 2;
  }
  case '0': {

    *piOut = (n > 2 && (sqlite3CtypeMap[(unsigned char)(z[2])] & 0x04)) ? 0x99999 : 0;

    return 2;
  }
  case '\'':
  case '"':
  case '/':
  case '\\': {
    *piOut = z[1];
    return 2;
  }
  case 'x': {
    if (n < 4) {
      *piOut = 0x99999;
      return n;
    }
    *piOut = (jsonHexToInt(z[2]) << 4) | jsonHexToInt(z[3]);
    return 4;
  }
  case 0xe2:
  case '\r':
  case '\n': {
    u32 nSkip = jsonBytesToBypass(z, n);
    if (nSkip == 0) {
      *piOut = 0x99999;
      return n;
    } else if (nSkip == n) {
      *piOut = 0;
      return n;
    } else if (z[nSkip] == '\\') {
      return nSkip + jsonUnescapeOneChar(&z[nSkip], n - nSkip, piOut);
    } else {
      int sz = sqlite3Utf8ReadLimited((u8 *)&z[nSkip], n - nSkip, piOut);
      return nSkip + sz;
    }
  }
  default: {
    *piOut = 0x99999;
    return 2;
  }
  }
}

__attribute__((noinline)) int jsonLabelCompareEscaped(const char *zLeft, u32 nLeft, int rawLeft, const char *zRight, u32 nRight, int rawRight) {
  u32 cLeft, cRight;

  while (1) {
    if (nLeft == 0) {
      cLeft = 0;
    } else if (rawLeft || zLeft[0] != '\\') {
      cLeft = ((u8 *)zLeft)[0];
      if (cLeft >= 0xc0) {
        int sz = sqlite3Utf8ReadLimited((u8 *)zLeft, nLeft, &cLeft);
        zLeft += sz;
        nLeft -= sz;
      } else {
        zLeft++;
        nLeft--;
      }
    } else {
      u32 n = jsonUnescapeOneChar(zLeft, nLeft, &cLeft);
      zLeft += n;

      ((void)(0))

          ;
      nLeft -= n;
    }
    if (nRight == 0) {
      cRight = 0;
    } else if (rawRight || zRight[0] != '\\') {
      cRight = ((u8 *)zRight)[0];
      if (cRight >= 0xc0) {
        int sz = sqlite3Utf8ReadLimited((u8 *)zRight, nRight, &cRight);
        zRight += sz;
        nRight -= sz;
      } else {
        zRight++;
        nRight--;
      }
    } else {
      u32 n = jsonUnescapeOneChar(zRight, nRight, &cRight);
      zRight += n;

      ((void)(0))

          ;
      nRight -= n;
    }
    if (cLeft != cRight)
      return 0;
    if (cLeft == 0)
      return 1;
  }
}

int jsonLabelCompare(const char *zLeft, u32 nLeft, int rawLeft, const char *zRight, u32 nRight, int rawRight) {
  if (rawLeft && rawRight) {

    if (nLeft != nRight)
      return 0;
    return memcmp(zLeft, zRight, nLeft) == 0;
  } else {
    return jsonLabelCompareEscaped(zLeft, nLeft, rawLeft, zRight, nRight, rawRight);
  }
}

int jsonAllAlphanum(const char *z, int n) {
  int i;
  for (i = 0; i < n && ((sqlite3CtypeMap[(unsigned char)(z[i])] & 0x06) || z[i] == '_'); i++) {
  }
  return i == n;
}
