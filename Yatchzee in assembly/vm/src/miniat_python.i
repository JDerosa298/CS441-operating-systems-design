%module miniat
%{
/* Put header files here or function declarations like below */
#include "miniat/miniat.h"
#include "miniat/miniat_error.h"

%}

%include exception.i
%include stdint.i
%include miniat/miniat.h

%rename(Byte)  m_byte;
%rename(Wyde)  m_wyde;
%rename(Word)  m_word;
%rename(DWord) m_dword;
%rename(Bus)   m_bus;

%constant int GPIOA     = m_gpio_id_A;
%constant int GPIOB     = m_gpio_id_B;
%constant int GPIOC     = m_gpio_id_C;
%constant int GPIOD     = m_gpio_id_D;
%constant int GPIOCNT   = m_gpio_id_cnt;

%constant int TIMER0     = m_timer_id_0;
%constant int TIMER1     = m_timer_id_1;
%constant int TIMER2     = m_timer_id_2;
%constant int TIMER3     = m_timer_id_3;
%constant int TIMERCNT   = m_timer_id_cnt;

%constant int XINT0   = m_xint_id_0;
%constant int XINT1   = m_xint_id_1;
%constant int XINT2   = m_xint_id_2;
%constant int XINT3   = m_xint_id_3;
%constant int XINT4   = m_xint_id_4;
%constant int XINT5   = m_xint_id_5;
%constant int XINT6   = m_xint_id_6;
%constant int XINT7   = m_xint_id_7;
%constant int XINT8   = m_xint_id_8;
%constant int XINT9   = m_xint_id_9;
%constant int XINT10  = m_xint_id_10;
%constant int XINT11  = m_xint_id_11;
%constant int XINT12  = m_xint_id_12;
%constant int XINT13  = m_xint_id_13;
%constant int XINT14  = m_xint_id_14;
%constant int XINT15  = m_xint_id_15;
%constant int XINTCNT = m_xint_id_cnt;

%exception {
    TRY {
        $function
    } CATCHALL {
        SWIG_exception(SWIG_RuntimeError,"Exception");
    } FINALLY {
    }
    ETRY;
}

struct Miniat {};

%newobject miniat_file_new;
%delobject miniat_free;

%extend Miniat {
    Miniat(char *bin_file_path) {
        return (void *)miniat_file_new(bin_file_path, NULL);
    }
    Miniat(char *bin_file_path, char *cache_path) {
        return (void *)miniat_file_new(bin_file_path, cache_path);
    }
    ~Miniat() {
        miniat_free((miniat *)$self);
    }
    void reset() {
        miniat_reset((miniat *)$self);
    }
    void clock() {
        miniat_clock((miniat *)$self);
    }
    m_wyde getGpio(m_gpio_id id) {
        return miniat_pins_get_gpio_port((miniat *)$self, id);
    }
    void setGpio(m_gpio_id id, m_wyde pins) {
        miniat_pins_set_gpio_port((miniat *)$self, id, pins);
    }
    m_timers_io getTimers(m_timer_id id) {
        return miniat_pins_get_timers((miniat *)$self, id);
    }
    void setTimers(m_timer_id id, m_timers_io pins) {
        miniat_pins_set_timers((miniat *)$self, id, pins);
    }
    m_byte getXint(m_xint id) {
        return miniat_pins_get_xint((miniat *)$self, id);
    }
    void setGpio(m_xint id, m_byte pins) {
        miniat_pins_set_xint((miniat *)$self, id, pins);
    }
    m_bus getBus() {
        return miniat_pins_bus_get((miniat *)$self);
    }
    void setBus(m_bus bus) {
        miniat_pins_bus_set((miniat *)$self, bus);
    }
};
