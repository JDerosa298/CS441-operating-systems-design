#!/usr/bin/env python

from miniat import *
import argparse
import copy
import sys

SIMPLE_TERM_ADDY=16384
SIMPLE_KB_ADDY=16386

terminal_bus = None
kb_bus = None

def term_set_bus(bus):
	global terminal_bus
	terminal_bus = bus

def term_get_bus():
	global terminal_bus
	return terminal_bus

def term_clock():
	if(terminal_bus.req == 1 and (terminal_bus.address == SIMPLE_TERM_ADDY)
		and terminal_bus.rW == 1 and terminal_bus.ack == 0):
		terminal_bus.ack = 1
		sys.stdout.write(chr(terminal_bus.data))
		sys.stdout.flush()
	elif((terminal_bus.address == SIMPLE_TERM_ADDY) and terminal_bus.ack == 1):
		terminal_bus.ack = 0
	return

def kb_set_bus(bus):
	global kb_bus
	kb_bus = bus

def kb_get_bus():
	global kb_bus
	return kb_bus

def kb_clock():
        if(kb_bus.req == 1 and (kb_bus.address == SIMPLE_KB_ADDY) and kb_bus.rW == 0 and kb_bus.ack == 0):
                kb_bus.ack = 1;
		c = sys.stdin.read(1)
                kb_bus.data = ord(c);
        elif(kb_bus.ack == 1 and (kb_bus.address == SIMPLE_KB_ADDY)):
                kb_bus.ack = 0;
        return;


def main():

	parser = argparse.ArgumentParser(description='Simple Console and Terminal for MiniAT, from Python!')

	parser.add_argument('bin_file', help='The binary file to execute in the MiniAT')

	parser.add_argument('--cache-file', default=None, help='The cache file to use for the MiniAT')

	args = parser.parse_args()

	m = Miniat(args.bin_file, args.cache_file)

	while True:
		m.clock()
		mbus = m.getBus()

		term_set_bus(mbus)
		kb_set_bus(mbus)

		term_clock()
		kb_clock()

		pbus = term_get_bus()
		if(pbus.address == SIMPLE_TERM_ADDY):
			m.setBus(pbus)
			continue

		pbus = kb_get_bus()
		if(pbus.address == SIMPLE_KB_ADDY):
			m.setBus(pbus)
			continue

		if(pbus.address == 0):
			continue

		print "Unkown bus address, skipping: " + str(pbus.address)

if __name__ == "__main__":
	main()
