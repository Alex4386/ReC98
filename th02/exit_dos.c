#pragma codeseg seg2

#include "libs/master.lib/master.h"
#include "th02/initexit.h"

void game_exit_to_dos(void)
{
	game_exit();
	key_beep_on();
	text_systemline_on();
	text_cursor_on();
}