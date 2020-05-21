#ifndef _MINIAT_ERROR_H_
#define _MINIAT_ERROR_H_

#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Throw catch mechanism was adopted from http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html
 * and is licensed under the MIT License, the code in-between the START / END MIT license is the adapted code
 * from the link above.
 *
 * You can have nested TRY blocks, start threads, and it should all work as expected with the following
 * caveats:
 * 1. You cannot jump (ie goto) out of a TRY/CATCH block, doing so will cause the FINALLY block not to execute.
 * 2. If starting a thread from within a TRY block, if the new thread THROW's an error, and no TRY block
 *    was encountered in that thread, the resulting exception will not end up where the thread was
 *    created from. The exception will propagate back to the global runtime exception handler, and
 *    a panic will ensue.
 * 3. You cannot call THROW from within a CATCH, you must use CTHROW if you would like to do that. This
 *    is really just a more specific version of 1.
 *
 * For implementation details, scroll down some more.
 *
 * A large thanks goes to Francesco for his help in incorporating this into our project and for
 * the code itself. The code below is not Francesco's original implementation, but rather a heavily
 * modified version of it.
 *
 * EXAMPLE TRY BLOCK
 * void func1(miniat *m) {
 *
 *	TRY {
 *		TRY {
 *			TRY {
 *				THROW(EXCEPTION1);
 *
 *			}
 *			CATCH(EXCEPTION1) {
 *				miniat_dump_error();
 *				CTHROW(EXCEPTION1); //Notice the CTHROW since it's in a catch
 *			}
 *			FINALLY {}
 *			ETRY;
 *		}
 *		CATCH(EXCEPTION1) {
 *			miniat_dump_error();
 *			CTHROW(EXCEPTION1); //Notice the CTHROW since its in a catch
 *		}
 *		FINALLY {}
 *		ETRY;
 *	}
 *	CATCH(EXCEPTION1) {
 *		miniat_dump_error();
 *	}
 *	FINALLY { } //Needed even though empty
 *	ETRY
 *	return;
 * }
 *
 */

/* START MIT License */

/**
 * TRY's some code out, on a THROW, the function returns to the nearest CATCH block.
 * @param m
 *  The Miniat to TRY some code on
 */
#define TRY \
	{ int _e_; m_error_stack __env; __env.next = NULL; __error_push(&__env); switch ( _e_ = setjmp(__env.jbuf) ){ case 0: for (;;){

/**
 * CATCH catches an exception
 * @param x
 *  The exception value to catch, one of m_error_num, corresponds with THROW(e)
 *  You can get the current value through _e_, declared as int.
 */
#define CATCH(x) break; case x:

/* End MIT License */

/**
 * CATCH block that catches ALL exceptions (up to INT_MAX)
 * @warning
 *  Requires GNUC as it depends on a ranged case statement.
 */
#define CATCHALL break; case INT_MIN ... -1: case 1 ... INT_MAX:

/* Start MIT License */

/**
 * Always runs after the TRY/CATCH blocks, assuming no one exits the program
 * @note Must be included
 */
#define FINALLY break; } default: { __error_pop(m_error_stack_pop); }

/**
 * Ends a TRY BLOCK, the whole thing
 *
 */
#define ETRY } }

/**
 * Throws an exception to be caught by a CATCH block.
 * @param m
 *  The miniat to "THROW" the error on.
 * @param e
 *  The exception number to throw, one of m_error_num
 */
#define THROW(e, fmt, ...) \
		do { __set_active_error(__LINE__, __FILE__, __func__, e, fmt, ##__VA_ARGS__); \
		longjmp(__error_pop(m_error_stack_peek)->jbuf, e); \
		} while (0)

/* END MIT License */

/**
 * Throws an exception to be caught by a CATCH block FROM INSIDE A CATCH BLOCK!!
 * @param m
 *  The miniat to "THROW" the error on.
 * @param e
 *  The exception number to throw, one of m_error_num
 */
#define CTHROW(e, fmt, ...) \
		do { __error_pop(m_error_stack_pop); \
		__set_active_error(__LINE__, __FILE__, __func__, e, fmt, ##__VA_ARGS__); \
		longjmp(__error_pop(m_error_stack_peek)->jbuf, e); \
		} while (0)

/**
 * Dumps the last error seen by the system to stdout and
 * clears the last seen error. Ie two consecutive calls without
 * a THROW in-between will cause NO output on the second call.
 *
 */
void miniat_dump_error();

/**
 * A call to this function results in the program terminating through
 * a call to exit(). Exit is passed the argument to this function.
 * Before exit() is called, the currently active error is printed, as
 * if you called miniat_dump_error() prior to calling exit().
 *
 * TODO Panic should be configurable, perhaps call a user registered
 * function with some userdata pointer.
 *
 * @param rc
 *  The return code for exit() to use.
 */
void miniat_panic(int rc);

/*
 * Begins needed declarations for the TRY/CATCH mechanism that should not be called
 * or used in any way. Using these will result in undefined behavior. With that said,
 * lets discuss the implementation:
 *
 * The TRY is a setjmp with some extra logic, the THROW is longjmp, and the others are some
 * sugar to make it all meld together.
 *
 * Their are 2 global variables used to maintain 2 stacks. One stack uses a thread specific
 * variable to store the head pointer and currently thrown error. The other stack, is a program
 * wide (ie all threads) variable that is used to hold the global error information. The split
 * stacks allows the protected runtime to set the jmp point for any thread created outside of it
 * back to the initial try catch in main().
 *
 * Each TRY pushes a jmpbuf (for setjmp/longjmp) onto the stack. If the stack head for the thread
 * is NULL, the push goes to the program scoped stack. This ensures that any thread created from
 * miniat_start() will have an associated error handler. Once the program global stack is initialized,
 * subsequent TRY's push a jumpbuf to the thread local stack, ensuring each thread can have it's own
 * error handler.
 *
 * Since each thread is building its own stack, using automatic variables (stack based data) along the way,
 * their needs to be a mechanism for cleaning up the stack and keeping it balanced. Using a THROW to pop
 * the data and call longjmp seems like a valid approach, but has one issue, if code in the TRY block
 * never executes a THROW, the stack never gets updated. On a subsequent THROW, the head is now pointing
 * to an invalid location and a sigsegv will ensue. Instead of this approach, the FINALLY block does
 * the resulting pop to balance the stack. This means that the FINALLY block must always run, this is
 * why limitations of 1 and 3 exist. Ie code cannot exit the CATCH or the FINALLY will never run. This
 * means you cannot use your own setjmp, longjump calls if you cannot guarntee that the return point is
 * in the CATCH block. You calso cannot use goto and leave the CATCH block that way. This is also why if
 * you would like to THROW, you must use CTHROW, wich pops from the stack.
 *
 * The macro functionality is something like this:
 *
 * TRY      --> Push jmpbuf to stack
 * CATCH    --> Switch statement (each CATCH(e) being a case on e.
 * CATCHALL --> Using a gnu C extension, captures the range of 1...MAX_INT
 * FINALLY  --> Default case in the switch that all jmps fall through too. Pops and dicards the
 *              head of the stack, keeping it balanced.
 * CTHROW   --> Pop from the stack, then peek and use that as the result to longjmp
 * THROW    --> Peek the stack and use that as the result to longjmp.
 *
 *
 */
typedef struct m_error m_error;
typedef struct m_error_stack m_error_stack;

/**
 * PEEK or POP from the error stack
 */
enum m_error_stack_action {
	m_error_stack_peek = 0, //!< used in __error_pop to denote a "peek" operation
	m_error_stack_pop       //!< used in __error_pop to denote a "pop" operation
};

typedef enum m_error_stack_action m_error_stack_action;

/**
 * Nodes for a stack of jmp_buf's from setjmp. Used to implement the
 * try catch finally macros.
 */
struct m_error_stack {
	jmp_buf jbuf;
	m_error_stack *next;
};

/**
 * Pushes a jump_buf node to the head of the stack
 * @param env
 *  The environment (ie jump buf) to push
 * @warning
 *  DO NOT CALL THIS FUNCTION DIRECTLY ... don't do it..hey.. I said don't
 */
extern m_error_stack *__error_pop(m_error_stack_action pop);

/**
 * Pop's or peeks a jump buf from the stack
 * @param pop
 *  Whether or not to pop or peek
 * @return
 *  The pointer to the jump buf
 * @warning
 *  DO NOT CALL THIS FUNCTION DIRECTLY .. if you're looking at these it better be for
 *  information only. If you mess up the error handling stack (or the actual call stack)
 *  you're on your own champ :-P
 */
extern void __error_push(m_error_stack *env);

/**
 * Sets the current error to the proper stack
 * @param line
 *  Current line number
 * @param file
 *  Current file
 * @param func
 *  Current function
 * @param e
 *  The error code
 * @param msg
 *  A printf style format specifier
 *
 */
extern void __set_active_error(int line, const char *file, const char *func,
		int e, const char *fmt, ...) __attribute__ ((format (printf, 5, 6)));

#ifdef __cplusplus
}
#endif

#endif /* MINIAT_ERROR_H */
