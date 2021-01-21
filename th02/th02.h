/* ReC98
 * -----
 * Include file for TH02
 */

#include "ReC98.h"
#include "th01/ranks.h"

#undef grcg_off

// Formats
// -------
typedef struct {
	char magic[4]; // = "MPTN"
	char count;
	char unused;
} mptn_header_t;

#define MPTN_SIZE (8 * 16)

extern char mptn_show_palette_on_load;
extern unsigned char mptn_count;
extern int *mptn_buffer;
extern char mptn_palette[16 * 3];

int pascal mptn_load(const char *fn);
void pascal mptn_palette_show(void);
void pascal mptn_free(void);

// "東方封魔.録" in Shift-JIS
#define PF_FN "\x93\x8C\x95\xFB\x95\x95\x96\x82\x2E\x98\x5E"
#define PF_KEY 0x12
// -------

// Hardware
// -------
#define graph_clear_both() \
	graph_accesspage(1);	graph_clear(); \
	graph_accesspage(0);	graph_clear(); \
	graph_accesspage(0);	graph_showpage(0);

#include "th01/hardware/grppsafx.h"
// -------

// Music Room
#define MUSIC_CMT_FILE "MUSIC.TXT"
#define MUSIC_CMT_LINE_LEN 42
#define MUSIC_CMT_LINE_COUNT 20

extern char unused_op_2_3; // Maybe debug mode?
extern char rank;
extern char lives;
extern char bombs;

#define SHOTTYPE_COUNT 3

// Highscores
// ----------
#define SCOREDAT_PLACES 10
#define SCOREDAT_NAME_LEN 6 /* excluding the terminating 0 */
#define EXTRA_CLEAR_FLAGS {1, 2, 4}
#define GAME_CLEAR_CONSTANTS {318, 118, 218}
#define STAGE_ALL 127

typedef struct {
	/* For ranks (and therefore, structure instances) #0, #1 and #2 (Easy,
	 * Normal and Hard), this is either GAME_CLEAR_CONSTANTS[rank] or 0,
	 * and indicates whether the main 5 stages have been cleared with the
	 * *shot type* associated with the rank's index, in any difficulty.
	 * Yes, ZUN uses a field in a rank-specific structure to store a
	 * shot type-specific value.
	 *
	 * For rank #3, this is instead interpreted as a bit field using the
	 * EXTRA_CLEAR_FLAGS to indicate whether the Extra Stage has been
	 * cleared with the respective shot type.
	 * Yes, ZUN stores what is technically information about the Extra
	 * rank in the structure of the Lunatic rank.
	 *
	 * For rank #4, this field is unused.
	 */
	int16_t cleared;

	int32_t points[SCOREDAT_PLACES];
	int32_t points_sum;
	unsigned char g_name[SCOREDAT_PLACES][SCOREDAT_NAME_LEN + 1];
	unsigned char g_name_first_sum;
	unsigned char stage[SCOREDAT_PLACES];
	unsigned char stage_sum;
	struct date date[SCOREDAT_PLACES];
	unsigned char shottype[SCOREDAT_PLACES];
} scoredat_t;

typedef struct {
	scoredat_t score;
	int32_t score_sum; // Sum of all bytes in [score], pre-encraption
} scoredat_section_t;

extern char cleared_game_with[SHOTTYPE_COUNT];
extern char cleared_extra_with[SHOTTYPE_COUNT];

int cleardata_load(void);
// ----------

