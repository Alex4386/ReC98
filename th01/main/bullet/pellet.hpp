/// Constants
/// ---------
static const int PELLET_COUNT = 100;

static const unsigned char PELLET_SLING_DELTA_ANGLE = +0x08;
static const unsigned char PELLET_SPIN_DELTA_ANGLE = +0x04;
#define PELLET_SPIN_CIRCLE_RADIUS to_sp(16.0f)
#define PELLET_CHASE_TOP_MAX to_sp(PLAYFIELD_BOTTOM - 80.0f)
/// ---------

enum pellet_motion_t {
	// No velocity change during regular pellet updates.
	PM_NORMAL = 0,

	// Accelerates the Y velocity of the pellet by its [speed] every frame.
	PM_GRAVITY = 1,

	// Slings the pellet in a (counter-)clockwise motion around its spawn
	// point, with a sling radius of [speed] and a rotational speed of
	// [PELLET_SLING_DELTA_ANGLE]° per frame. Once the pellet completed one
	// full revolution, it is fired towards the player's position on that
	// frame, in a straight line at the same [speed].
	PM_SLING_AIMED = 2,

	// Lets the pellet bounce off the top of the playfield once, negating its
	// X velocity, and zeroing its Y velocity. The pellet then switches to
	// PM_GRAVITY.
	// Unused in the original game. Should have bounced the bullets off the
	// left and right edge of the playfield as well, but that code doesn't
	// actually work in ZUN's original code.
	PM_BOUNCE_FROM_TOP_THEN_GRAVITY = 3,

	// Lets the pellet bounce off the top of the playfield once, zeroing its
	// X velocity, and setting its Y velocity to [speed]. The pellet then
	// switches to PM_NORMAL.
	PM_FALL_STRAIGHT_FROM_TOP_THEN_NORMAL = 4,

	// Spins the pellet on a circle around a [spin_center] point, which moves
	// at [spin_velocity], with [PELLET_SPIN_CIRCLE_RADIUS] and a rotational
	// speed of [PELLET_SPIN_DELTA_ANGLE]° per frame. Since both the center of
	// rotation and the angle are changed every frame, this will result in a
	// swerving motion for individual pellets. Use this type for a ring of
	// pellets to create moving multi-bullet circles, as seen in the Kikuri
	// fight.
	PM_SPIN = 5,

	// For every frame this pellet is above [PELLET_CHASE_TOP_MAX], its
	// velocity is adjusted by -1, 0, or +1 in both coordinates, depending on
	// the location of the player relative to this pellet.
	PM_CHASE = 6,

	_pellet_motion_t_FORCE_INT16 = 0x7FFF
};

enum pellet_sling_direction_t {
	PSD_NONE = 0,
	PSD_CLOCKWISE = 1,
	PSD_COUNTERCLOCKWISE = 2,

	_pellet_sling_direction_t_FORCE_INT16 = 0x7FFF
};

// Types for predefined multi-pellet patterns. In TH01, individual bullets of
// such a pattern only differ in their angle.
//
// The PP_?_ number indicates the number of pellets created by this pattern.
//
// All patterns (including the random-angle ones) are symmetrical around
// • a (0, 1) vector, pointing downwards (for static patterns)
// • a vector from the pellet's origin to the player (for aimed patterns)
// Odd-numbered spreads always contain a pellet in the center, which moves
// along this axis of symmetry; even-numbered spreads don't.
//
// For aimed patterns, this means that:
// • all odd-numbered spreads are aimed *at* the player, while
// • all even-numbered spreads are aimed *around* the player.
enum pellet_pattern_t {
	// Does not actually work, due to a ZUN bug in pattern_velocity_set()!
	PP_NONE = 0,

	PP_1 = 1,
	PP_1_AIMED = 12,

	PP_2_SPREAD_WIDE,
	PP_2_SPREAD_NARROW,
	PP_3_SPREAD_WIDE,
	PP_3_SPREAD_NARROW,
	PP_4_SPREAD_WIDE,
	PP_4_SPREAD_NARROW,
	PP_5_SPREAD_WIDE,
	PP_5_SPREAD_NARROW,

	// Aimed versions of the n-way spreads above. Expected to have enum values
	// >= this one!
	PP_AIMED_SPREADS,

	PP_2_SPREAD_WIDE_AIMED = PP_AIMED_SPREADS,
	PP_2_SPREAD_NARROW_AIMED,
	PP_3_SPREAD_WIDE_AIMED,
	PP_3_SPREAD_NARROW_AIMED,
	PP_4_SPREAD_WIDE_AIMED,
	PP_4_SPREAD_NARROW_AIMED,
	PP_5_SPREAD_WIDE_AIMED,
	PP_5_SPREAD_NARROW_AIMED,

	// -11.25 deg to +11.25 deg, around the player
	PP_1_RANDOM_NARROW_AIMED = 29,
	// -45 deg to +45 deg, facing down
	PP_1_RANDOM_WIDE = 30,

	_pellet_pattern_t_FORCE_INT16 = 0x7FFF
};

struct pellet_t {
protected:
	friend class CPellets;

	// Why, I don't even?!?
	void sloppy_wide_unput(void) {
		egc_copy_rect_1_to_0_16(
			prev_left.to_pixel(), prev_top.to_pixel(), 16, PELLET_H
		);
	}

	void sloppy_wide_unput_at_cur_pos(void) {
		egc_copy_rect_1_to_0_16(
			cur_left.to_pixel(), cur_top.to_pixel(), 16, PELLET_H
		);
	}

public:
	bool moving;
	unsigned char motion_type;

	// Automatically calculated every frame for PM_SPIN
	Subpixel cur_left;
	Subpixel cur_top;

	SPPoint spin_center; // only used for PM_SPIN
	Subpixel prev_left;
	Subpixel prev_top;
	pellet_pattern_t from_pattern;
	SPPoint velocity; // unused for PM_SPIN
	SPPoint spin_velocity; // only used for PM_SPIN

	// Used only for poor attempts at reducing flickering. Does not disable
	// hit testing if true.
	bool16 not_rendered;

	int age;
	Subpixel speed; // for recalculating velocities with certain motion types
	int decay_frame;
	int cloud_frame;
	screen_x_t cloud_left;	// Not subpixels!
	screen_y_t cloud_top; 	// Not subpixels!
	int angle;	// for recalculating velocities with certain motion types

	// Direction for PM_SLING_AIMED. Not reset when a pellet is destroyed -
	// and therefore effectively only calculated once for every instance of
	// this structure.
	pellet_sling_direction_t sling_direction;
};

class CPellets {
	pellet_t near pellets[PELLET_COUNT];
	int alive_count; // only used for one single optimization
	int unknown_zero[10];
public:
	int unknown_seven;

	// Rendering pellets at odd or even indices this frame?
	bool16 interlace_field;
	bool spawn_with_cloud;

protected:
	pellet_t near* iteration_start(void) {
		return static_cast<pellet_t near *>(pellets);
	}

	// Updates the velocity of the currently iterated pellet, depending on its
	// [motion_type].
	void motion_type_apply_for_cur(void);

	void decay_tick_for_cur(void);
	bool16 hittest_player_for_cur(void);

	// Processes any collision between the currently iterated pellet, the Orb,
	// or a deflecting player. (Regular, life-losing hit testing between
	// pellets and the player is done elsewhere!)
	// Returns true if the bullet remains visible.
	//
	// (And yes, this function does operate on the currently iterated pellet,
	// despite taking separate position parameters!)
	bool16 visible_after_hittests_for_cur(
		screen_x_t pellet_left, screen_y_t pellet_top
	);

	void clouds_unput_update_render(void);

public:
	CPellets(void);

	// Spawns a number of bullets according to the given [pattern], with their
	// corresponding velocities, at (left, top). [speed_base] is tuned
	// according to the currently played difficulty and the resident
	// [pellet_speed]. The [motion_type] for the new pellets is PM_NORMAL.
	void add_pattern(
		screen_x_t left,
		screen_y_t top,
		pellet_pattern_t pattern,
		subpixel_t speed_base
	);

	// Spawns a single new pellet with a customizable [motion_type].
	// [speed_base] is tuned according to the currently played difficulty and
	// the resident [pellet_speed]; [speed_for_motion_fixed] is never tuned.
	//
	// [spin_center_x] and [spin_center_y] are only used with PM_SPIN,
	// while [speed_base] is *ignored* for PM_SPIN.
	void add_single(
		screen_x_t left,
		screen_y_t top,
		int angle,
		subpixel_t speed_base,
		pellet_motion_t motion_type,
		subpixel_t speed_for_motion_fixed = to_sp(0.0f),
		screen_x_t spin_center_x = 0,
		screen_y_t spin_center_y = 0
	);

	// Transitions all living pellets into their decay state, awarding points
	// for each one.
	void decay_all(void);

	// Also calls Shots.unput_update_render()!
	void unput_update_render(void);

	void unput_and_reset_all(void);
	void reset_all(void);
};

/// Globals
/// -------
extern CPellets Pellets;

// If true, CPellets::unput_update_render() performs
// • rendering,
// • and hit testing against the Orb and the player
// for only half of the [pellets] each frame, alternating between even and odd
// [pellets] array indices. However,
// • motion updates,
// • and hit testing against *player shots*
// are still done for all pellets every frame.
//
// Probably not really meant to save CPU and/or VRAM accesses, but rather to
// reduce flicker in busy boss battles, due to all the sloppy unblitting...
extern bool pellet_interlace;

extern unsigned int pellet_destroy_score_delta;
/// -------
