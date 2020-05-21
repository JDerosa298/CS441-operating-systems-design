#ifndef _MINIAT_PRIV_UNIONS_H_
#define _MINIAT_PRIV_UNIONS_H_

#include "miniat/miniat.h"
#include "miniat_defines.h"
#include "miniat_typedefs.h"

union m_csr {
	struct {
		m_uword ie :1;
		m_uword se :1;
		m_uword u :1;
		m_uword fi :1;
		m_uword fs :1;
		m_uword reserved :27;
	};
	m_uword all;
};

union m_watchdog_control {
	struct {
		m_uword count :27;
		m_uword reserved :3;
		m_uword reset :1;
		m_uword enable :1;
	};
	m_uword all;
};

union m_watchdog_compare {
	struct {
		m_uword value :27;
		m_uword CPS :5;
	};
	m_uword all;
};

union m_timer_control {
	struct {
		m_ubyte CPS :4;
		m_ubyte enable :1;
		m_ubyte mode :2;
		m_ubyte reset :1;
	};
	m_ubyte all;
};

union m_timer_compare {
	struct {
		m_uword duty :16;
		m_uword wavelength :16;
	};
	m_uword all;
};

union m_low_word_layout {

	/* GCC does little end first*/
	struct {
		m_uword rC :8;
		m_uword rB :8;
		m_uword rA :8;
		m_uword opcode :5;
		m_uword unused :1;
		m_uword hint :1;
		m_uword predicate :1;
	};

	m_uword all;
};

#endif
