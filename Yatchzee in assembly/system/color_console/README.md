# MiniAT Color Console

The **MiniAT** Color Console is a system simulation of a raw, 80x24 character color terminal.  The terminal size is fixed, and the terminal must be at least that large or the simulation will pause until the client is resized appropriately; a notice will be shown so you'll be aware the resize is necessary.

## Running the Example

From your root **MiniAT** directory, run the command...

	miniat_color_console -c out/bin/color_console_demo.bin

The `-c` is optional and prints the number of **MiniAT** clock cycles executed prior to the system exiting/shutting down.

## Color

Eight basic colors are supported.  This small number was chosen because these eight colors are portable to a great many terminal clients and platforms through which this system may run.  Each color can be used arbitrarily for background or foreground.  The colors and their respective values are shown in the table below:

Color | Value
------|-------
BLACK   | 0
RED     | 1
GREEN   | 2
YELLOW  | 3
BLUE    | 4
MAGENTA | 5
CYAN    | 6
WHITE   | 7

## Programmable Devices

### Peripheral Address Space

** 0x4000 -- _Keyboard Buffer_**

LOAD returns the oldest key in the input buffer. -1 is returned if the buffer is empty.

** 0x4001 & 0x4002 -- _Terminal_ & _Fixed Terminal_** (respectively)

STOR writes given character to terminal at current cursor position with colors and attributes determined by the use of peripherals in the 0x4020s.

LOAD reads the character at the current cursor position, returning a space (0x20) if there is no visible character.

STOR/LOAD using the **_Terminal_** increments the cursor position (first by X, then by Y), potentially wrapping.  The **_Fixed Terminal_** does not alter the cursor position.  _**NOTE:**_ Changes to the terminal are actually made to the terminal buffer, which is only rendered to the screen after the terminal is refreshed.

See the section on "Character data" for further details.

** 0x4003 & 0x4004 -- _Raw Terminal_ & _Raw Fixed Terminal_**

STOR writes a character with colors and attributes all passed simultaneously on the bus.  The expected encoding is:

- Bits 0--15: The character to print.  See the section on "Character data" for further details.
- Bits 16--20: Printing attributes (0:off, 1:on)...
	- Bit 16: Underline
	- Bit 17: Reverse FG and BG colors
	- Bit 18: Blink
	- Bit 19: Bold
	- Bit 20: Invisible
- Bits 21--23: _Reserved_
- Bits 24--27: FG color indicator
- Bits 28--31: BG color indicator

LOAD reads the character with colors and attributes with that same encoding at the current cursor position, returning a space (0x20) if there is no visible character.

STOR/LOAD using the **_Raw Terminal_** increments the cursor position (first by X, then by Y), potentially wrapping.  The **_Raw Fixed Terminal_** does not alter the cursor position.  _**NOTE:**_ Changes to the terminal are actually made to the terminal buffer, which is only rendered to the screen after the terminal is refreshed.

** 0x4005 -- _Refresh_**

STOR causes a refresh of the screen from the terminal buffer.

----------------
** 0x4010 & 0x4011 -- _Cursor X_ & _Cursor Y Positions_** (respectively)

STOR relocates the cursor to the given location for valid positions (0 <= X < 80 and 0 <= Y < 24) or does nothing for invalid locations.

LOAD reads the cursor's X and Y positions.

** 0x4012 -- _Cursor X,Y Position_**

STOR relocates the cursor to the given X,Y location for valid positions (0 <= X < 80 and 0 <= Y < 24) or does nothing for invalid locations.

The expected encoding is:

- Bits 0--7: X position
- Bits 8--15: Y position

To read the X,Y positions simultaneously, consider using the Cursor Position port (Port A)

** 0x4013 -- _Foreground Color_**

STOR sets the foreground color for new characters rendered.  See the section on "Color Values" for details.  Invalid color values are ignored.

LOAD reads the current foreground color.

** 0x4014 -- _Background Color_**

STOR sets the background color for new characters rendered.  See the section on "Color Values" for details.  Invalid color values are ignored.

LOAD reads the current background color.

** 0x4015 -- _Underline_**

STOR non-zero value underlines new characters. 0 disables this attribute.

** 0x4016 -- _Reverse_**

STOR non-zero value switches the background and foreground colors of new characters. 0 disables this attribute.

** 0x4017 -- _Blink_**

STOR non-zero value causes new characters to blink. 0 disables this attribute.

** 0x4018 -- _Bold_**

STOR non-zero value emphasizes new characters. 0 disables this attribute.

** 0x4019 -- _Invisible_**

STOR non-zero value prints new characters with a foreground color matching the background color. 0 disables this attribute.  Basically, you can use this to print text you can't normally see while still being selectable and not _actually_ changing the foreground color.

** 0x401A -- _All Attributes_**

STOR sets colors and attributes all passed simultaneously on the bus.  The expected encoding is:

- Bits 0--15: _Reserved_
- Bits 16--20: Printing attributes (0:off, 1:on)...
	- Bit 16: Underline
	- Bit 17: Reverse FG and BG colors
	- Bit 18: Blink
	- Bit 19: Bold
	- Bit 20: Invisible
- Bits 21--23: _Reserved_
- Bits 24--27: FG color indicator
- Bits 28--31: BG color indicator

The FG and BG color indexes must be 0--7.  If one is not, that color is left unchanged.  This way, the "All Attributes" peripheral can be used reset only the printing attributes, for example.

LOAD reads the colors and attributes used for new characters into a word with that same encoding.

** 0x401B -- _Attribute Stack_**

STOR records the current printing attributes and colors on an internal attribute stack.

LOAD restores the printing attributes and colors from the attribute stack.

You should not STOR more than 64 attribute records.  STORing when full or LOADing when empty has no effect.

** 0x401C -- _Coordinate Stack_**

STOR records the current cursor position on an internal coordinate stack.

LOAD restores the cursor position from the coordinate stack.

You should not STOR more than 64 coordinate records.  STORing when full or LOADing when empty has no effect.



------------------
** 0x4020 -- _Flush Keyboard Buffer_**

STOR empties the keyboard buffer.  Previously buffered keys in the buffer are NOT recoverable after a flush.

** 0x4021 -- _Show Cursor_**

STOR 0 hides the cursor (makes it invisible), otherwise the cursor is shown.

** 0x4022 -- _Beep_**

STOR attempts to make an audible beep.  If not, it attempts to flash the screen.  Otherwise, nothing happens.

** 0x4023 -- _Erase_**

STOR erases all screen content, filling the screen with the background color.  The cursor position is reset to (0,0).

** 0x4024 -- _Tab Size_**

STOR sets tab size, where 1 < tab size < 24.  Does nothing otherwise.

LOAD reads current tab size.

** 0x4025 -- _Auto Refresh_**

STOR 0 requires manual refreshing of the terminal, otherwise the screen will refresh from the terminal buffer each cycle.

------------------
** 0x4500 -- _Exit_**

STOR terminates the program and shuts down the console system.

### Ports

** Port A (0x1B1A) -- _Cursor Position_** (input)

LOAD reads the cursor's X and Y positions simultaneously with the following encoding:

- Bits 0--7: X position
- Bits 8--15: Y position

** Port B (0x1B1B) -- _Read Character_ ** (input)

LOAD reads the character value from the terminal buffer at the current cursor position.  _**NOTE:**_ The character value is read from the terminal buffer which may not match what is shown on the screen if the terminal had not been refreshed since the character was last updated.

** Port C (0x1B1C) -- _Color_** (output)

STOR Sets the character foreground and background colors simultaneously.  The encoding is:

- Bits 0--2: FG color
- Bits 3--5: BG color
- Bits 6--14: _Reserved_
- Bit 15: Trigger

The "Trigger" bit is used to signal the terminal to adopt the updated character foreground and background colors whenever the bit transitions (i.e., from 0 to 1 or from 1 to 0).

The FG and BG colors are each set using values described in the section on "Color Values". _**NOTE:**_ This port many not be safely used for reading the current colors as they may have been changed using other peripheral devices.

## Character Data

Outside of the basic ASCII printable characters, a number of other character codes (all negative) can be used for drawing purposes.  These characters and their respective values are shown in the table below:

Drawing Character | Value
---------------|-------
Upper Left Corner |  -2
Lower Left Corner |  -3
Upper Right Corner |  -4
Lower Right Corner |  -5
Left-facing T |  -6
Right-facing T |  -7
Bottom-facing T |  -8
Top-facing T |  -9
Horizontal Line | -10
Vertical Line | -11
Plus (Cross) | -12
S1 | -13
S9 | -14
Diamond | -15
Checkerboard | -16
Degree | -17
Plus/Minus | -18
Bullet | -19
Left Arrow | -20
Right Arrow | -21
Down Arrow | -22
Up Arrow | -23
Board | -24
Lantern | -25
Block | -26
S3 | -27
S7 | -28
Less Than or Equal | -29
Greater Than or Equal | -30
Pi | -31
Not Equal | -32
Sterling | -33

## Miscelania

Blinking text is not always supported by terminal clients.  Sometimes, when your blinking text does _not_ blink, the issue may not be that the client doesn't support it.  Instead, check your client preferences to see whether blinking has been disabled.
