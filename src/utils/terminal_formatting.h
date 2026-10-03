#pragma once

static inline const char *color(const char *code)
{
	return code;
}

#define COLOR_RESET color("\x1b[0m")

// Styles
#define BOLD color("\x1b[1m")
#define DIM color("\x1b[2m")
#define ITALIC color("\x1b[3m")
#define UNDERLINE color("\x1b[4m")
#define BLINK color("\x1b[5m")
#define REVERSE color("\x1b[7m")
#define HIDDEN color("\x1b[8m")
#define STRIKETHROUGH color("\x1b[9m")

// Normal colors
#define BLACK color("\x1b[30m")
#define RED color("\x1b[31m")
#define GREEN color("\x1b[32m")
#define YELLOW color("\x1b[33m")
#define BLUE color("\x1b[34m")
#define MAGENTA color("\x1b[35m")
#define CYAN color("\x1b[36m")
#define WHITE color("\x1b[37m")

// Bright colors
#define BRIGHT_BLACK color("\x1b[90m")
#define BRIGHT_RED color("\x1b[91m")
#define BRIGHT_GREEN color("\x1b[92m")
#define BRIGHT_YELLOW color("\x1b[93m")
#define BRIGHT_BLUE color("\x1b[94m")
#define BRIGHT_MAGENTA color("\x1b[95m")
#define BRIGHT_CYAN color("\x1b[96m")
#define BRIGHT_WHITE color("\x1b[97m")

// BOLD
#define BOLD_BLACK color("\x1b[1;30m")
#define BOLD_RED color("\x1b[1;31m")
#define BOLD_GREEN color("\x1b[1;32m")
#define BOLD_YELLOW color("\x1b[1;33m")
#define BOLD_BLUE color("\x1b[1;34m")
#define BOLD_MAGENTA color("\x1b[1;35m")
#define BOLD_CYAN color("\x1b[1;36m")
#define BOLD_WHITE color("\x1b[1;37m")

// Backgrounds
#define BG_BLACK color("\x1b[40m")
#define BG_RED color("\x1b[41m")
#define BG_GREEN color("\x1b[42m")
#define BG_YELLOW color("\x1b[43m")
#define BG_BLUE color("\x1b[44m")
#define BG_MAGENTA color("\x1b[45m")
#define BG_CYAN color("\x1b[46m")
#define BG_WHITE color("\x1b[47m")

// Bright Backgrounds
#define BG_BRIGHT_BLACK color("\x1b[100m")
#define BG_BRIGHT_RED color("\x1b[101m")
#define BG_BRIGHT_GREEN color("\x1b[102m")
#define BG_BRIGHT_YELLOW color("\x1b[103m")
#define BG_BRIGHT_BLUE color("\x1b[104m")
#define BG_BRIGHT_MAGENTA color("\x1b[105m")
#define BG_BRIGHT_CYAN color("\x1b[106m")
#define BG_BRIGHT_WHITE color("\x1b[107m")
