#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqlite3_file sqlite3_file;
typedef struct sqlite3_io_methods sqlite3_io_methods;

struct sqlite3_file {
  const struct sqlite3_io_methods *pMethods;
};

#ifdef __cplusplus
}
#endif