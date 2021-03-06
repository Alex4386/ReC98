#define CFG_FN "reiiden.cfg"
#define CFG_ID "REIIDEN"

struct cfg_options_t {
	int8_t rank;
	int8_t bgm_mode;
	int8_t bombs;
	int8_t lives_extra;	// Add 2 for the actual number of lives
};

#define CFG_RANK_DEFAULT (RANK_NORMAL)
#define CFG_BGM_MODE_DEFAULT (true)
#define CFG_BOMBS_DEFAULT (1)
#define CFG_LIVES_EXTRA_DEFAULT (2)

#define CFG_LIVES_EXTRA_MAX (4)

struct cfg_t {
	char id[sizeof(CFG_ID) - 1];
	cfg_options_t opts;
};
