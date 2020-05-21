#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"

#include "miniat_dso.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"

/*
 * Stolen from:
 * http://stackoverflow.com/questions/18298280/how-to-declare-a-variable-as-thread-local-portably
 */
#ifndef thread_local
# if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && ( \
       defined _MSC_VER || \
       defined __ICL || \
       defined __DMC__ || \
       defined __BORLANDC__ )
#  define thread_local __declspec(thread)
/* note that ICC (linux) and Clang are covered by __GNUC__ */
# elif defined __GNUC__ || \
       defined __SUNPRO_C || \
       defined __xlC__
#  define thread_local __thread
# else
#  error "Cannot define thread_local"
# endif
#endif

#ifndef MINIAT_ERROR_MSG_LEN
#  define MINIAT_ERROR_MSG_LEN 256
#endif

# if MINIAT_ERROR_MSG_LEN <= 0
#  error MINIAT_ERROR_MSG_LEN must be greater than 0
#endif

/**
 * Details of the last active error
 */
typedef struct m_error_details m_error_details;
struct m_error_details {
	int active;
	int num;
	int line;
	const char *file;
	const char *func;
	char msg[MINIAT_ERROR_MSG_LEN];
};

/**
 * m_error structure defines all of the information that is captured when an
 * error is thrown or handled.
 */
typedef struct m_error m_error;
struct m_error {
	m_error_stack *head;      //!< Head pointer to stack of jump bufs
	m_error_details curr_err; //!< Current error being thrown
};

static thread_local m_error __m_err__;
static m_error __m_g_err__;

PUBLIC void __error_push(m_error_stack *env) {

	m_error_stack **head =
			__m_g_err__.head ? &(__m_err__.head) : &(__m_g_err__.head);

	if (head && *head) {
		env->next = *head;
	}
	*head = env;

	return;
}

PUBLIC m_error_stack *__error_pop(m_error_stack_action pop) {

	m_error_stack **head =
			__m_err__.head ? &(__m_err__.head) : &(__m_g_err__.head);
	m_error_stack *tmp = NULL;
	if (head && *head) {
		tmp = *head;
		if (pop == m_error_stack_pop) {
			*head = (*head)->next;
		}
	}

	return tmp;
}

PUBLIC void miniat_dump_error() {

	m_error *err = __m_err__.head ? &(__m_err__) : &(__m_g_err__);
	if (!err->curr_err.active) {
		return;
	}

	const char *msg = err->curr_err.msg[0] ? err->curr_err.msg : "(no message)";
	fprintf(stderr,
			"Error %d -- \"%s\" in file: \"%s\" in function: %s on line %d\n",
			err->curr_err.num, msg,
			err->curr_err.file, err->curr_err.func, err->curr_err.line);

	err->curr_err.active = 0;
}

PUBLIC void miniat_panic(int rc) {
	fprintf(stderr, "PANIC -- Uncaught Exception\n");
	miniat_dump_error();
	exit(rc);
}

PUBLIC void __set_active_error(int line, const char *file, const char *func,
		int e, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	m_error *err = __m_err__.head ? &(__m_err__) : &(__m_g_err__);
	err->curr_err.active = 1;
	err->curr_err.line = line;
	err->curr_err.func = func;
	err->curr_err.file = file;
	err->curr_err.num = e;
	if (fmt) {
		vsnprintf(err->curr_err.msg, sizeof(err->curr_err.msg), fmt, args);
	} else {
		err->curr_err.msg[0] = '\0';
	}

	va_end(args);
}
