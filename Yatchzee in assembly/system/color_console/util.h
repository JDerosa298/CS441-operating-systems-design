#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, (int)'/') ? (strrchr(__FILE__, (int)'/') + 1) : \
	(strrchr(__FILE__, (int)'\\') ? (strrchr(__FILE__, (int)'\\') + 1) : \
	__FILE__) \
)


typedef enum util_print_level util_print_level;


enum util_print_level {
	util_print_level_error = 0,
	util_print_level_impossible
};


/**
 * prints an error message to a specified stream
 * @param lvl
 *  the error level
 * @param out
 *  file to output the message too
 * @param filename
 *  The filename of the current file (not always used)
 * @param lineno
 *  The line number the error occurred on (not always used)
 * @param fmt
 *  The format specifier consistent with printf
 */
extern void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...)
                       __attribute__ ((format (printf, 5, 6)));


/*
 * ERROR macro
 *
 * prints error message and exits.
 */
#define ERROR(fmt, ...) \
	do { \
		if (isendwin()) { \
			endwin(); \
		} \
		util_print(util_print_level_error, stderr, NULL, 0, fmt, ##__VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0)


/*
 * IMPOSSIBLE macro
 *
 * prints error message and exits.
 */
#define IMPOSSIBLE(fmt, ...) \
	do { \
		if (isendwin()) { \
			endwin(); \
		} \
		util_print(util_print_level_impossible, stderr, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0)


/*
 * SAFE_MALLOC macro
 *
 * Allocate, cast, and clear memory request or exit.
 */
#define SAFE_MALLOC(ptr, ptr_type, size) { \
	\
	ptr = (ptr_type)malloc(size); \
	if (ptr == NULL) { \
		ERROR("Unable to allocate memory from \"%s\" on line %d", __FILENAME__, __LINE__); \
	} \
	memset(ptr, 0, size); \
}

/*
 * SAFE_REALLOC macro
 *
 * Reallocate and cast, or exit.
 */
#define SAFE_REALLOC(ptr, ptr_type, size) { \
	\
	void *tmp = (ptr_type)realloc(ptr, size); \
	if (tmp == NULL) { \
		ERROR("Unable to allocate memory from \"%s\" on line %d", __FILENAME__, __LINE__); \
	} \
	else { \
		ptr = tmp; \
	} \
}


#endif /* __UTIL_H__ */
