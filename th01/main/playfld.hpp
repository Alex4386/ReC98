static const screen_x_t PLAYFIELD_LEFT = 0;
static const screen_y_t PLAYFIELD_TOP = 64;
static const screen_x_t PLAYFIELD_RIGHT = RES_X;
static const screen_y_t PLAYFIELD_BOTTOM = RES_Y;

static const pixel_t PLAYFIELD_W = (PLAYFIELD_RIGHT - PLAYFIELD_LEFT);
static const pixel_t PLAYFIELD_H = (PLAYFIELD_BOTTOM - PLAYFIELD_TOP);

static const screen_x_t PLAYFIELD_CENTER_X = (
	((PLAYFIELD_RIGHT - PLAYFIELD_LEFT) / 2) + PLAYFIELD_LEFT
);
