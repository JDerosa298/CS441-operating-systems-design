#ifndef _MINIAT_H_
#define _MINIAT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <miniat/miniat_error.h>

#ifdef __cplusplus
extern "C" {
#endif

#define M_LOW         0
#define M_HIGH        1

/** standard signed and unsigned 8 bit m_byte types */
typedef int8_t  m_sbyte;	/* signed 8 bit */
typedef uint8_t m_ubyte;	/* unsigned 8 bit */

/** standard signed and unsigned 16 bit m_wyde types */
typedef int16_t  m_swyde;	/* signed 16 bit */
typedef uint16_t m_uwyde;	/* unsigned 16 bit */

/** standard signed and unsigned 32 bit m_word types */
typedef int32_t  m_sword;	/* signed 32 bit */
typedef uint32_t m_uword;	/* unsigned 32 bit */

typedef int64_t  m_sdword; /* signed 64 bit */
typedef uint64_t m_udword; /* unsigned 64 bit */

typedef struct miniat miniat;
typedef struct m_bus m_bus;

typedef union m_byte m_byte;
typedef union m_wyde m_wyde;
typedef union m_word m_word;
typedef union m_dword m_dword;
typedef union m_timers_io m_timers_io;

/**
 * The bus structure, this represents all the wires
 * that can be used to connect peripherals over the bus
 *
 * Remember, a bus is just some wires, and here we expose those.
 */
struct m_bus {

	m_ubyte req;       //!< request wire
	m_ubyte ack;       //!< acknowledgment wire
	m_ubyte rW;        //!< read/write wire
	m_uword address;   //!< Address wires
	m_uword data;      //!< Data wires
};

/**
 * This is a nice union on a byte that lets you get
 * a signed or unsigned value as well as get the individual
 * bits.
 * ex:
 *   m_byte x = 0xFF
 *   Signed value is accessed by typing 'x.s'
 *   Unsigned value is accessed by typing 'x.u'
 *   To get a bit type 'x.bit0'
 *   Set a bit type 'x.bit3 = 1;'
 */
union m_byte {
	struct {
		m_ubyte bit0 : 1;
		m_ubyte bit1 : 1;
		m_ubyte bit2 : 1;
		m_ubyte bit3 : 1;
		m_ubyte bit4 : 1;
		m_ubyte bit5 : 1;
		m_ubyte bit6 : 1;
		m_ubyte bit7 : 1;
	} bits;
	m_sbyte s;
	m_ubyte u;
};

/**
* This is a nice union on a wyde (16 bits) that lets you get
* a signed or unsigned value as well as get the individual
* bits.
*/
union m_wyde {
	struct {
		m_uwyde bit0  : 1;
		m_uwyde bit1  : 1;
		m_uwyde bit2  : 1;
		m_uwyde bit3  : 1;
		m_uwyde bit4  : 1;
		m_uwyde bit5  : 1;
		m_uwyde bit6  : 1;
		m_uwyde bit7  : 1;
		m_uwyde bit8  : 1;
		m_uwyde bit9  : 1;
		m_uwyde bit10 : 1;
		m_uwyde bit11 : 1;
		m_uwyde bit12 : 1;
		m_uwyde bit13 : 1;
		m_uwyde bit14 : 1;
		m_uwyde bit15 : 1;
	} bits;
	m_swyde s;
	m_uwyde u;
};

/**
* This is a nice union on a word (32 bits) that lets you get
* a signed or unsigned value as well as get the individual
* bits.
*/
union m_word {
	struct {
		m_uword bit0  : 1;
		m_uword bit1  : 1;
		m_uword bit2  : 1;
		m_uword bit3  : 1;
		m_uword bit4  : 1;
		m_uword bit5  : 1;
		m_uword bit6  : 1;
		m_uword bit7  : 1;
		m_uword bit8  : 1;
		m_uword bit9  : 1;
		m_uword bit10 : 1;
		m_uword bit11 : 1;
		m_uword bit12 : 1;
		m_uword bit13 : 1;
		m_uword bit14 : 1;
		m_uword bit15 : 1;
		m_uword bit16 : 1;
		m_uword bit17 : 1;
		m_uword bit18 : 1;
		m_uword bit19 : 1;
		m_uword bit20 : 1;
		m_uword bit21 : 1;
		m_uword bit22 : 1;
		m_uword bit23 : 1;
		m_uword bit24 : 1;
		m_uword bit25 : 1;
		m_uword bit26 : 1;
		m_uword bit27 : 1;
		m_uword bit28 : 1;
		m_uword bit29 : 1;
		m_uword bit30 : 1;
		m_uword bit31 : 1;
	} bits;
	m_sword s; //!< signed word value
	m_uword u; //!< unsigned word value
};

/**
* This is a nice union on a double word (64 bits) that lets you get
* a signed or unsigned value.
*/
union m_dword {
	m_sdword s; //!< signed double word value
	m_udword u; //!< unsigned double word value
};

/**
 * Union for input and output on the timer io pins
 */
union m_timers_io {
	struct {
		m_ubyte output  :1; //!< The output pin
		m_ubyte input	:1; //!< The input pin
	} line;
	m_ubyte all;
};

/*
 * Do not add the ENUM and raw C interfaces into the SWIG
 * build, just use the swig interface file for that.
 *
 * Note that changes to anything here likely will require
 * changes to the swig interface file.
 */
#ifndef SWIG

/**
 * The id of the gpio port you would like to access
 */
enum m_gpio_id {
	m_gpio_id_A = 0, //!< m_gpio_id_A
	m_gpio_id_B,     //!< m_gpio_id_B
	m_gpio_id_C,     //!< m_gpio_id_C
	m_gpio_id_D,     //!< m_gpio_id_D

	/*
	 * Count must go last
	 */
	m_gpio_id_cnt   //!< m_gpio_id_cnt
};
typedef enum m_gpio_id m_gpio_id;

/**
 * The id of the timer you would like to access
 */
enum m_timer_id {
	m_timer_id_0 = 0,//!< m_timer_id_0
	m_timer_id_1,    //!< m_timer_id_1
	m_timer_id_2,    //!< m_timer_id_2
	m_timer_id_3,    //!< m_timer_id_3

	/*
	 * Count must go last
	 */
	m_timer_id_cnt   /** m_timer_id_cnt */
};
typedef enum m_timer_id m_timer_id;

/**
 * The id of the external interrupt you would like to access
 */
enum m_xint_id {
	m_xint_id_0 = 0,//!< m_xint_id_0
	m_xint_id_1,    //!< m_xint_id_1
	m_xint_id_2,    //!< m_xint_id_2
	m_xint_id_3,    //!< m_xint_id_3
	m_xint_id_4,    //!< m_xint_id_4
	m_xint_id_5,    //!< m_xint_id_5
	m_xint_id_6,    //!< m_xint_id_6
	m_xint_id_7,    //!< m_xint_id_7
	m_xint_id_8,    //!< m_xint_id_8
	m_xint_id_9,    //!< m_xint_id_9
	m_xint_id_10,   //!< m_xint_id_10
	m_xint_id_11,   //!< m_xint_id_11
	m_xint_id_12,   //!< m_xint_id_12
	m_xint_id_13,   //!< m_xint_id_13
	m_xint_id_14,   //!< m_xint_id_14
	m_xint_id_15,   //!< m_xint_id_15

	/*
	 * Count must go last
	 */
	m_xint_id_cnt   //!< m_xint_id_cnt
};
typedef enum m_xint_id m_xint;

/**
 * Create a miniat instance
 *
 * @param bin_file
 * 	The compiled asm file you would like to run
 *
 * @param (optional) lua_cache_file_path
 * 	The filesystem path to the lua script for cache implementation, may be NULL.
 *
 * @return
 * 	In instance of a miniat
 *
 * @throw
 *  EIO
 *
 * @note
 *  panics on any issues initializing the miniat subsystems and then
 *  throws EIO.
 */
extern miniat *miniat_new (FILE *bin_file, const char *lua_cache_file_path);

/**
 * Create a miniat instance
 *
 * @param bin_file_path
 * 	The filesystem path to the compiled asm file you would like to run
 *
 * @param (optional) lua_cache_file_path
 * 	The filesystem path to the lua script for cache implementation, may be NULL.
 *
 * @return
 * 	In instance of a miniat
 *
 * @throw
 *  EINVAL
 *
 * @note
 *  This is just a wrapper around miniat_new
 *
 *  panics on any issues initializing the miniat subsystems and then
 *  throws EIO.
 */
extern miniat *miniat_file_new (const char *bin_file_path, const char *lua_cache_file_path);

/**
 * Used to free a miniat created by miniat_new
 *
 * @param m
 * 	the miniat to free
 *
 * 	@throw
 * 	 EINVAL
 */
extern void miniat_free (miniat *m);

/**
 * Resets a miniat. This is equivalent to a free and new except it reuses the memory
 * allocated by miniat_new.
 * @param m
 * 	The miniat you wish to reset
 *
 * @throw
 *  EINVAL
 *
 * @note
 *  panics on any other error, which causes the application to exit.
 *  This function should never fail.
 */
extern void miniat_reset(miniat *m);

/*
 * miniat_clock will provide a "clock tick" to the miniat instance specified through the pointer. By providing a
 * clock tick, the miniat should execute one clock cycle. An error code is returned.
 */
/** provide a clock tick to a miniat
 *
 * This function is the workhorse of the miniat. By calling this function you cause a clock cycle to
 * occur inside the miniat given.
 *
 * @param m
 * 	The miniat you wish to give a clock cycle too
 *
 * @throw
 *  EINVAL
 *
 * @note
 *  panics on any other error, which causes the application to exit.
 *  This function should never fail.
 */
extern void miniat_clock (miniat *m);

/**
 * Gets the pin values of a gpio port
 *
 * @param m
 *  The miniat whose gpio port you want
 *
 * @param port
 *  The port to get the values of
 *
 * @return
 *  The value of the port
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern m_wyde miniat_pins_get_gpio_port(miniat *m, m_gpio_id port);

/**
 * Sets the pins on a gpio port
 *
 * @param m
 *  The miniat whose gpio port you want to set
 *
 * @param port
 *  The port to set
 *
 * @param pins
 *  The value to set on the pins
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern void miniat_pins_set_gpio_port(miniat *m, m_gpio_id port, m_wyde pins);

/**
 * Gets the value of a timer's io pins
 *
 * @param m
 *  The miniat whose timer pins you would like to get
 *
 * @param timer
 *  The timer to get
 *
 * @return
 *  The value of the pins, both Timer pins input and output
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern m_timers_io miniat_pins_get_timers(miniat *m, m_timer_id timer);

/**
 * Sets the value of a timer's io pins
 *
 * @param m
 *  The miniat whose timer pins you want to set
 *
 * @param timer
 *  The timer to set
 *
 * @param pins
 *  The value of the ^input^ pins to set, output pin values are ignored.
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern void miniat_pins_set_timers(miniat *m, m_timer_id timer, m_timers_io pins);

/**
 * Gets the value of an external interrupt pin.
 *
 * @param m
 *  The miniat whose xint value you would like to get.
 *
 * @param xint
 *  The xint you would like to get.
 *
 * @return
 *  The value of that xint pin.
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern m_byte miniat_pins_get_xint(miniat *m, m_xint xint);

/**
 * Sets an xint pin.
 *
 * @param m
 *  The miniat whose xint you would like to set.
 *
 * @param xint
 *  The xint to set.
 *
 * @param data
 *  The value of the pin to set, the value is from bit0.
 *
 *  @throw
 *   EINVAL
 *   ERANGE
 */
extern void miniat_pins_set_xint(miniat *m, m_xint xint, m_byte data);

/**
 * Gets the value of a miniat bus.
 *
 * @param m
 *  The miniat's bus you would like to get.
 *
 * @return
 *  The values of the bus pins.
 *
 * @throw
 *  EINVAL
 *
 */
extern m_bus miniat_pins_bus_get(miniat *m);

/**
 * Sets the value of the bus. Note that read only
 * pins cannot be adjusted through this interface.
 *
 * @param m
 *  The miniat whose bus you would like to set.
 *
 * @param bus
 *  The bus values to set.
 *
 * @throw
 *  EINVAL
 */
extern void miniat_pins_bus_set(miniat *m, m_bus bus);

/** Gets a miniat bus connector
 *
 * Bus connector objects a
 *
 * @param m
 * 	The miniat who's bus connector you would like.
 *
 * @return
 * 	The bus connector object or NULL on failure.
 *
 * @throw
 *  EINVAL
 */
extern m_bus *miniat_conector_bus_get(miniat *m);

/*
 * Verify the types are the correct m_byte size.  the "* 2) - 1" ensures
 * a 0 (false) condition results in an impossible negative size.
 * A >=1 (true) condition results in a size greater than 0... critical
 * as some compilers will not allow size 0 arrays, so "- 1" will
 * not be enough for this test in a portable sense.
 */
typedef int WRONG_BYTE_SIZE_SBYTE [((sizeof (m_sbyte) == 1) * 2) - 1];
typedef int WRONG_BYTE_SIZE_UBYTE [((sizeof (m_ubyte) == 1) * 2) - 1];

typedef int WRONG_BYTE_SIZE_SWYDE [((sizeof (m_swyde) == 2) * 2) - 1];
typedef int WRONG_BYTE_SIZE_UWYDE [((sizeof (m_uwyde) == 2) * 2) - 1];

typedef int WRONG_BYTE_SIZE_SWORD [((sizeof (m_sword) == 4) * 2) - 1];
typedef int WRONG_BYTE_SIZE_UWORD [((sizeof (m_uword) == 4) * 2) - 1];

typedef int WRONG_BYTE_SIZE_SDWORD [((sizeof (m_sdword) == 8) * 2) - 1];
typedef int WRONG_BYTE_SIZE_UDWORD [((sizeof (m_udword) == 8) * 2) - 1];

#endif /* SWIG */

#ifdef __cplusplus
}
#endif

#endif /* MINIAT_H_ */

