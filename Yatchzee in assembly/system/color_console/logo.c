#include <stdio.h>
#include <stdint.h>

#define HEIGHT 21

#define TERM_COLOR_MIN     0

#define TERM_COLOR_BLACK   0
#define TERM_COLOR_RED     1
#define TERM_COLOR_GREEN   2
#define TERM_COLOR_YELLOW  3
#define TERM_COLOR_BLUE    4
#define TERM_COLOR_MAGENTA 5
#define TERM_COLOR_CYAN    6
#define TERM_COLOR_WHITE   7

#define TERM_COLOR_MAX     7
#define TERM_COLOR_CNT (TERM_COLOR_MAX - TERM_COLOR_MIN + 1)


typedef union raw_terminal_word raw_terminal_word;


union raw_terminal_word {
	struct {
		uint32_t character: 16;
		uint32_t underline:  1;
		uint32_t reverse:    1;
		uint32_t blink:      1;
		uint32_t bold:       1;
		uint32_t invisible:  1;
		uint32_t reserved:   3;
		uint32_t fg:         4;
		uint32_t bg:         4;
	};
	uint32_t all;
};



char *img[HEIGHT] = { "", "", "",
	"                        ,....                      ,....",
	"                    _d^^^^^^^^^b_              _d^^^^^^^^^b_",
	"                 .d''           ``b.        .d''           ``b.",
	"               .p' --------------- `q.    .p' --------------- `q.",
	"              .d'                   `b.  .d'                   `b.",
	"             .d' ___________________ `b  d' ___________________ `b.",
	"            .HH                        \\/                        HH.",
	"            dHH                                                  HHb",
	"            9HH ----------- . -------------------- . ----------- HHP",
	"            `pp.           |p.                    .q|           .qq'",
	"             `pp. ________ |`p. ________________ .q'| ________ .qq'",
	"              `bb.         | `b.                .d' |         .dd'",
	"                `qqbo...   |   `q..          ..p'   |   ,..odpp'",
	"                  `qqqqq:+-+      ^q........p^      +-+:ppppp^",
	"                      `'q             ''''             p'^",
	"",
	"",
	"                           M    I    N    I    A    T"
};

char *map[HEIGHT] = { "", "", "",
	"                        44444                      44444",
	"                    4111111111114              4111111111114",
	"                 4111000000000001114        4111000000000001114",
	"               41100000000000000000114    41100000000000000000114",
	"              4113333333333333333333114  4113333333333333333333114",
	"             41133333333333333333333314  41333333333333333333333114",
	"            41100000000000000000000000011000000000000000000000000114",
	"            41100000000000000000000000000000000000000000000000000114",
	"            41133333333333331333333333333333333333313333333333333114",
	"            41113333333333321133333333333333333333112333333333331114",
	"             411100000000002411000000000000000000114200000000001114",
	"              41110000000002 4110000000000000000114 20000000001114",
	"                411111110002   411100000000001114   200011111114",
	"                  4111111222      411111111114      2221111114",
	"                      444             4444             444",
	"",
	"",
	"                           5    5    5    5    5    5"
};

int main(int argc, char *argv[]) {

	int line;
	for (line = 0; line < HEIGHT; line++) {
		char *img_str = img[line];
		char *map_str = map[line];
		while (*img_str) {
			char i = *img_str;
			char m = *map_str;
			raw_terminal_word raw = {.all = 0};
			raw.character = i;

			switch (m) {
			case ' ':
				raw.bg = TERM_COLOR_BLACK;
				raw.fg = TERM_COLOR_BLACK;
				break;
			case '0':
				raw.bg = TERM_COLOR_GREEN;
				raw.fg = TERM_COLOR_RED;
				break;
			case '1':
				raw.bg = TERM_COLOR_GREEN;
				raw.fg = TERM_COLOR_BLACK;
				raw.bold = 1;
				raw.underline = 1;
				break;
			case '2':
				raw.bg = TERM_COLOR_WHITE;
				raw.fg = TERM_COLOR_YELLOW;
				raw.bold = 1;
				break;
			case '3':
				raw.bg = TERM_COLOR_YELLOW;
				raw.fg = TERM_COLOR_RED;
				break;
			case '4':
				raw.bg = TERM_COLOR_BLACK;
				raw.fg = TERM_COLOR_GREEN;
				raw.bold = 1;
				break;
			case '5':
				raw.bg = TERM_COLOR_BLACK;
				raw.fg = TERM_COLOR_RED;
				raw.bold = 1;
				break;
			default:
				break;
			}

			printf("0x%08X ", raw.all);
			img_str++;
			map_str++;
		}
		printf("%d\n", (int)'\n');
	}

	return 0;
}