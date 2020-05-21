%option noyywrap
%option case-insensitive

%{
/*
 * Name        : sunyat-asm.lex
 * Author      : William "Amos" Confer
 * 
 * License     : Copyright (c) 2008--2014 William "Amos" Confer
 *              
 *    Permission is hereby granted, free of charge, to any person obtaining a 
 *    copy of this software and associated documentation files (the "Software"),
 *    to deal in the Software without restriction, including without limitation
 *    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *    and/or sell copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.;  
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *    DEALINGS IN THE SOFTWARE.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "sunyat-asm.h"

extern void *ParseAlloc(void *(*mallocProc)(size_t));
extern void Parse(void *yyp,int yymajor, Token yyminor);
extern void ParseFree(void *p, void (*freeProc)(void*));
extern void ParseTrace(FILE *TraceFILE, char *zTracePrompt);

void *parser;
Token T;
char *tmp_str;

/* Requires two passes... (1) to build symbol, address, and value tables for
 * variables, constants and labels, and (2) to generate output ROM file if 
 * stage (1) passes without error.
 */
int assembler_pass; 
int line_num;
int char_pos;

char * filename;

char ascii_to_value (char *s, char base) {
	const char table [] = "0123456789abcdef";
	char result = 0;
	while (*s != '\0') {
		if (*s != '_') {
			result *= base;
			result += (strchr (table, tolower (*s)) - table) / sizeof (char);
		}
		s++;
	}
	return result;
}

char *get_yystring () {
	char * s = malloc (sizeof (char) * (1 << 16));
	memcpy (s, yytext, yyleng);
	s [yyleng] = '\0';
	s = realloc (s, sizeof (char) * (strlen (s) + 1));
	return s;
}

char *pack_token (Token *t) {
	t->token_str = t->long_str = get_yystring();
	t->line_num = line_num;
	t->char_pos = char_pos;
	char_pos += yyleng;
	return t->long_str;
}

%}

HEX_DIGIT [a-f0-9]
DEC_DIGIT [0-9]
BIN_DIGIT [01]

ALPHA    [a-z]
ALPHANUM [a-z0-9]

GPREG r[0-7]

%%

parse_trace_on	{ ParseTrace (stdout, "TRACE:\t"); }
parse_trace_off { ParseTrace (NULL, NULL); }

nop	{ pack_token (&T); Parse (parser, TOKEN_NOP, T); /* is replaced with "jmp (to_next_line" by parser*/ }
not	{ pack_token (&T); Parse (parser, TOKEN_NOT, T); /* is replaced with "xor reg, 0x_FF" by parser*/ }
mov	{ pack_token (&T); Parse (parser, TOKEN_MOV, T); }
add	{ pack_token (&T); Parse (parser, TOKEN_ADD, T); }
sub	{ pack_token (&T); Parse (parser, TOKEN_SUB, T); }
mul	{ pack_token (&T); Parse (parser, TOKEN_MUL, T); }
div	{ pack_token (&T); Parse (parser, TOKEN_DIV, T); }
cmp	{ pack_token (&T); Parse (parser, TOKEN_CMP, T); }
jmp	{ pack_token (&T); Parse (parser, TOKEN_JMP, T); }
jeq	{ pack_token (&T); Parse (parser, TOKEN_JEQ, T); }
jne	{ pack_token (&T); Parse (parser, TOKEN_JNE, T); }
jgr	{ pack_token (&T); Parse (parser, TOKEN_JGR, T); }
jls	{ pack_token (&T); Parse (parser, TOKEN_JLS, T); }
call	{ pack_token (&T); Parse (parser, TOKEN_CALL, T); }
ret	{ pack_token (&T); Parse (parser, TOKEN_RET, T); }
and	{ pack_token (&T); Parse (parser, TOKEN_AND, T); }
or	{ pack_token (&T); Parse (parser, TOKEN_OR, T); }
xor	{ pack_token (&T); Parse (parser, TOKEN_XOR, T); }
neg	{ pack_token (&T); Parse (parser, TOKEN_NEG, T); }
load	{ pack_token (&T); Parse (parser, TOKEN_LOAD, T); }
loadp	{ pack_token (&T); Parse (parser, TOKEN_LOADP, T); }
stor	{ pack_token (&T); Parse (parser, TOKEN_STOR, T); }
storp	{ pack_token (&T); Parse (parser, TOKEN_STORP, T); }
push	{ pack_token (&T); Parse (parser, TOKEN_PUSH, T); }
pop	{ pack_token (&T); Parse (parser, TOKEN_POP, T); }

\.(message|msg)[ \t]+.*	{
	/* message for application */
	pack_token (&T);
	tmp_str = strpbrk (T.long_str, " \t"); /* get past directive */
	if (tmp_str != NULL) {
		while ((*tmp_str == ' ') || (*tmp_str == '\t')) tmp_str++; /* skip whitespace */
	}
	T.token_str = tmp_str;
	Parse (parser, TOKEN_MESSAGE, T);
}

\.(address|addr)[ \t]	{
	/* address to write to */
	pack_token (&T);
	Parse (parser, TOKEN_ADDR_DIRECTIVE, T);
}

\.(variable|var)[ \t]	{
	/* variables definition */
	pack_token (&T);
	Parse (parser, TOKEN_VAR_DIRECTIVE, T);
}

\.(constant|const)[ \t]	{
	/* constant definition */
	pack_token (&T);
	Parse (parser, TOKEN_CONST_DIRECTIVE, T);
}

0x(_?{HEX_DIGIT})+ {
	/* unsigned hex number */
	tmp_str = pack_token (&T);
	tmp_str += 2;
	T.data = ascii_to_value (tmp_str, 16);
	Parse (parser, TOKEN_IMMEDIATE, T);
}

("+"|"-")?(0d)?(_?{DEC_DIGIT})+ {
	/* signed decimal number */
	tmp_str = pack_token (&T);
	/* skip optional sign */
	if ((*tmp_str == '+') || (*tmp_str == '-')) {
		tmp_str++;
	}
	/* skip optional "0d" prefix */
	if (tmp_str [1] == 'd') {
		tmp_str += 2;
	}
	T.data = ascii_to_value (tmp_str, 10);
	/* negate based on optional sign */
	if (*T.token_str == '-') {
		T.data = -T.data;
	}
	Parse (parser, TOKEN_IMMEDIATE, T);
}

0b(_?{BIN_DIGIT})+ {
	/* unsigned hex number */
	tmp_str = pack_token (&T);
	tmp_str += 2;
	T.data = ascii_to_value (tmp_str, 2);
	Parse (parser, TOKEN_IMMEDIATE, T);
}

{GPREG} {
	/* general purpose register */
	pack_token (&T);
	T.data = ascii_to_value (T.long_str + 1, 10);
	Parse (parser, TOKEN_REGISTER, T);
}

{ALPHA}(_?{ALPHANUM})* {
	/* identifier */
	pack_token (&T);
	Parse (parser, TOKEN_IDENTIFIER, T);
}

\'.\' {
	/* character immediate */
	pack_token (&T);
	T.data = yytext [1];
	Parse (parser, TOKEN_CHARACTER, T);
}

"!" {
	/* label */
	pack_token (&T);
	Parse (parser, TOKEN_LABEL_DIRECTIVE, T);
}

\n	{
	/* EOL */
	pack_token (&T);
	Parse (parser, TOKEN_EOL, T);
	line_num++;
	char_pos = 1;
}

[ \t]	{
	/* whitespace */
	pack_token (&T);
}

";"[^\n]*	{
	/* line comment */
	pack_token (&T);
}


.	{
	/* ERROR!!!! */
	pack_token (&T);
	if (assembler_pass == 1) {
		fprintf (stderr, "Error, line %d:%d - unexcpected character\"%s\"\n", T.line_num, T.char_pos, get_yystring ());
	}
}

%%

int main (int argc, char **argv) {
	extern int errors_found;        /* sunyat-asm.y */
	extern unsigned char address;   /* sunyat-asm.y */
	extern char *variables[];
	extern char *constants[];
	extern char *labels[];
	extern unsigned char variable_addrs [];
	extern unsigned char constant_values [];
	extern unsigned char label_addrs [];
	extern int variable_cnt;
	extern int constant_cnt;
	extern int label_cnt;
	
	int i;

	argv++; argc--;

	if (argc == 1) {
		if (strcmp (argv [0] + strlen (argv [0]) - 4, ".asm") != 0) {
			fprintf (stderr, "Error - \".asm\" file expected\n");
			exit (1);
		}
		filename = (char*) malloc (sizeof (char) * (strlen (argv[0]) + 1));
		if (filename == NULL) {
			fprintf (stderr, "Error - could not allocate memory sufficient for this application\n");
			exit (1);
		}
		/* strcpy(filename + strlen (filename) - 3, "rom"); */
		strcpy (filename, argv [0]);		
		strcpy (strrchr (filename, '.'), ".rom");
		yyin = fopen (argv [0], "r");
		if (yyin != NULL) {
			parser = ParseAlloc (malloc);
			/* first pass of the assembler. see comment at the
			* top of file for detatils.
			*/
			assembler_pass = 1;
			line_num = 1;
			char_pos = 1;
			yylex ();
			Parse (parser, TOKEN_EOL, T);

			Parse (parser, 0, T);
			if (errors_found == 0) {
				/* second pass of the assembler. see comment at the
				* top of file for detatils.
				*/
				assembler_pass = 2;
				line_num = 1;
				char_pos = 1;
				address = 0;
				rewind (yyin);
				yylex ();
				Parse (parser, TOKEN_EOL, T);
				Parse (parser, 0, T);
			}
			ParseFree (parser, free);
			fclose (yyin);
		}
		free (filename);		
	}
	else {
		fprintf (stderr, "no input file given\n");
	}
	return errors_found;
}
