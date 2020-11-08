/* ReC98
 * -----
 * C++ redeclarations and ReC98-specific extensions for master.lib.
 * Definitions that require types from `pc98.h` are guarded via `#ifdef`,
 * allowing this header to be used on its own if those aren't required.
 */

/// Types
/// -----

// A version of master.lib's Point without the constructor, even in C++
struct point_t {
	int x, y;
};

#ifdef PC98_H
	struct screen_point_t {
		screen_x_t x;
		screen_y_t y;
	};

	#if defined(__cplusplus)
	// master.lib palettes use twice the bits per RGB component for more
	// toning precision
	typedef RGB<uint8_t, 256> RGB8;
	typedef Palette<RGB8> Palette8;
	#endif
#endif
/// -----

/// Memory model definitions (adapted from master.h)
/// ------------------------------------------------
#if !defined(MASTER_NEAR) && !defined(MASTER_FAR) && !defined(MASTER_COMPACT) && !defined(MASTER_MEDIUM)
	#if defined(__SMALL__) || defined(__TINY__) || defined(M_I86SM) || defined(M_I86TM)
		#define MASTER_NEAR
	#elif defined(__COMPACT__) || defined(M_I86CM)
		#define MASTER_COMPACT
	#elif defined(__MEDIUM__) || defined(M_I86MM)
		#define MASTER_MEDIUM
	#elif defined(__LARGE__) || defined(__HUGE__) || defined(M_I86LM) || defined(M_I86HM)
		#define MASTER_FAR
	#endif
#endif

#if defined(MASTER_NEAR)
	#define MASTER_RET near pascal
	#define MASTER_CRET near cdecl
	#define MASTER_PTR near
#elif defined(MASTER_FAR)
	#define MASTER_RET far pascal
	#define MASTER_CRET far cdecl
	#define MASTER_PTR far
#elif defined(MASTER_COMPACT)
	#define MASTER_RET near pascal
	#define MASTER_CRET near cdecl
	#define MASTER_PTR far
#elif defined(MASTER_MEDIUM)
	#define MASTER_RET far pascal
	#define MASTER_CRET far cdecl
	#define MASTER_PTR near
#endif

#ifndef MASTER_RET
	#error Memory model for master.lib could not be determined?
#endif
/// ------------------------------------------------

/// Original functions (only contains those actually called from ZUN code)
/// ----------------------------------------------------------------------
/// TODO: Remove the `!defined(__MASTER_H)` branches once we've gotten rid of
/// of master.h.

// EGC
// ---

#if !defined(__MASTER_H)
#define EGC_ACTIVEPLANEREG	0x04a0
#define EGC_READPLANEREG  	0x04a2
#define EGC_MODE_ROP_REG  	0x04a4
#define EGC_FGCOLORREG    	0x04a6
#define EGC_MASKREG       	0x04a8
#define EGC_BGCOLORREG    	0x04aa
#define EGC_ADDRRESSREG   	0x04ac
#define EGC_BITLENGTHREG  	0x04ae

#define EGC_COMPAREREAD	0x2000
#define EGC_WS_PATREG  	0x1000	/* WS = write source */
#define EGC_WS_ROP     	0x0800	/* parren reg, ans of rop, cpu data */
#define EGC_WS_CPU     	0x0000
#define EGC_SHIFT_CPU  	0x0400	/* input to shifter */
#define EGC_SHIFT_VRAM 	0x0000	/* cpu write, vram read */
#define EGC_RL_MEMWRITE	0x0200	/* RL = pattern Register Load */
#define EGC_RL_MEMREAD 	0x0100	/* ^at mem write, <-at mem read */
#define EGC_RL_NONE    	0x0000	/* no touch */
#endif

void MASTER_RET egc_on(void);
void MASTER_RET egc_off(void);
void MASTER_RET egc_start(void);

#if !defined(__MASTER_H)
#define egc_selectpat() \
	outport(EGC_READPLANEREG, 0x00ff)

// TODO: Document and add helpful macros for the EGC raster ops
#define egc_setrop(mode_rop) \
	outport(EGC_MODE_ROP_REG, mode_rop)
#endif
// ---

// Graphics
// --------

#if !defined(__MASTER_H)
#define graph_showpage(p) \
	outportb(0xA4, p)

#define graph_accesspage(p) \
	outportb(0xA6, p)
#endif

extern unsigned graph_VramZoom;

void MASTER_RET graph_400line(void);
void MASTER_RET graph_200line(int tail);
void MASTER_RET graph_clear(void);
void MASTER_RET graph_show(void);
void MASTER_RET graph_hide(void);
void MASTER_RET graph_start(void);
int MASTER_RET graph_copy_page(int to_page);

#if !defined(__MASTER_H) && defined(PC98_H) && defined(__cplusplus)
	void MASTER_RET graph_gaiji_putc(int x, int y, int c, int color);
	void MASTER_RET graph_gaiji_puts(
		int x, int y, int step, const char MASTER_PTR *str, int color
	);

	// Clipping
	int MASTER_RET grc_setclip(int xl, int yt, int xr, int yb);
	int MASTER_RET grc_clip_polygon_n(
		screen_point_t MASTER_PTR *dest,
		int ndest,
		const screen_point_t MASTER_PTR *src,
		int nsrc
	);
#endif
// --------

// GRCG-accelerated graphics
// -------------------------

#if !defined(__MASTER_H)
/* grcg_setcolor()や vgc_setcolor()に指定するアクセスプレーン指定 */
#define GC_B	0x0e	/* 青プレーンをアクセスする */
#define GC_R	0x0d
#define GC_BR	0x0c	/*	:  */
#define GC_G	0x0b	/*	:  */
#define GC_BG	0x0a	/*	:  */
#define GC_RG	0x09
#define GC_BRG	0x08	/*	:  */
#define GC_I	0x07
#define GC_BI	0x06
#define GC_RI	0x05
#define GC_BRI	0x04
#define GC_GI	0x03
#define GC_BGI	0x02
#define GC_RGI	0x01	/*	:  */
#define GC_BRGI	0x00	/* 全プレーンをアクセスする */

/* grcg_setcolor()の modeに設定する値 */
#define GC_OFF	0
#define GC_TDW	0x80	/* 書き込みﾃﾞｰﾀは無視して、ﾀｲﾙﾚｼﾞｽﾀの内容を書く */
#define GC_TCR	0x80	/* ﾀｲﾙﾚｼﾞｽﾀと同じ色のﾋﾞｯﾄが立って読み込まれる */
#define GC_RMW	0xc0	/* 書き込みﾋﾞｯﾄが立っているﾄﾞｯﾄにﾀｲﾙﾚｼﾞｽﾀから書く */

void MASTER_RET grcg_setcolor(int mode, int color);
void MASTER_RET grcg_settile_1line(int mode, long tile);
void MASTER_RET grcg_off(void);

#define grcg_setmode(mode) \
	outportb(0x7C, mode)
#define grcg_off() \
	outportb(0x7C, 0)
#endif

#if defined(PC98_H) && defined(__cplusplus)
	// Trapezoids
	void MASTER_RET grcg_trapezoid(
		int y1, int x11, int x12, int y2, int x21, int x22
	);

	// Polygons
	void MASTER_RET grcg_polygon_c(
		const struct Point MASTER_PTR *pts, int npoint
	);
	void MASTER_RET grcg_polygon_cx(
		const struct Point MASTER_PTR *pts, int npoint
	);

	// Triangles
	void MASTER_RET grcg_triangle(
		int x1, int y1, int x2, int y2, int x3, int y3
	);

	// Straight lines
	void MASTER_RET grcg_hline(int x1, int x2, int y);
	void MASTER_RET grcg_vline(int x, int y1, int y2);
	void MASTER_RET grcg_line(int x1, int y1, int x2, int y2);

	// Rectangles
	void MASTER_RET grcg_boxfill(int x1, int y1, int x2, int y2);
	void MASTER_RET grcg_pset(int x, int y);
	void MASTER_RET grcg_fill(void);
	void MASTER_RET grcg_round_boxfill(
		int x1, int y1, int x2, int y2, unsigned r
	);
	void MASTER_RET grcg_byteboxfill_x(
		vram_x_t left, vram_y_t top, vram_x_t right, vram_y_t bottom
	);

	// Circles
	void MASTER_RET grcg_circle(int x, int y, unsigned r);
	void MASTER_RET grcg_circlefill(int x, int y, unsigned r);
#endif
// -------------------------

// Heap
// ----

void MASTER_RET mem_assign(unsigned top_seg, unsigned parasize);
void MASTER_RET mem_assign_all(void);
int MASTER_RET mem_unassign(void);
int MASTER_RET mem_assign_dos(unsigned parasize);

// Regular
unsigned MASTER_RET hmem_allocbyte(unsigned bytesize);
void MASTER_RET hmem_free(unsigned memseg);
// Fast
unsigned MASTER_RET smem_wget(unsigned bytesize);
void MASTER_RET smem_release(unsigned memseg);
// ----

// Math
// ----

extern const short SinTable8[256], CosTable8[256];
extern long random_seed;

#define Sin8(t) SinTable8[(t) & 0xff]
#define Cos8(t) CosTable8[(t) & 0xff]

int MASTER_RET iatan2(int y, int x);
int MASTER_RET isqrt(long x);
int MASTER_RET ihypot(int x, int y);

#define irand_init(seed) \
	(random_seed = (seed))
int MASTER_RET irand(void);

#define srand(s) irand_init(s)
#define rand()   irand()
// ----

// Optionally buffered single-file I/O
// -----------------------------------

int MASTER_RET file_ropen(const char MASTER_PTR *filename);
int MASTER_RET file_read(void far *buf, unsigned wsize);
long MASTER_RET file_size(void);
int MASTER_RET file_create(const char MASTER_PTR *filename);
int MASTER_RET file_append(const char MASTER_PTR *filename);
int MASTER_RET file_write(const void far *buf, unsigned wsize);
void MASTER_RET file_seek(long pos, int dir);
void MASTER_RET file_close(void);
int MASTER_RET file_exist(const char MASTER_PTR *filename);
int MASTER_RET file_delete(const char MASTER_PTR *filename);
// -----------------------------------

// Palette
// -------

void MASTER_RET palette_show(void);

#if !defined(__MASTER_H)
#define palette_settone(tone) \
	(PaletteTone = (tone), palette_show())

#define palette_100() \
	palette_settone(100)

#define palette_black() \
	palette_settone(0)

#define palette_white() \
	palette_settone(200)
#endif

#if !defined(__MASTER_H) && defined(PC98_H) && defined(__cplusplus)
	#define palette_set(col, r, g, b) (\
		Palettes[col].v[0] = (uint8_t)(r), \
		Palettes[col].v[1] = (uint8_t)(g), \
		Palettes[col].v[2] = (uint8_t)(b) \
	)

	#define palette_set_all(m) \
		memcpy(Palettes, (m), sizeof(Palette8))

	extern Palette8 Palettes;
#endif

extern unsigned int PaletteTone;

int MASTER_RET palette_entry_rgb(const char MASTER_PTR *);
void MASTER_RET palette_black_in(unsigned speed);
void MASTER_RET palette_black_out(unsigned speed);
void MASTER_RET palette_white_in(unsigned speed);
void MASTER_RET palette_white_out(unsigned speed);
// -------

// .PI
// ---

#if defined(__cplusplus) && !defined(__MASTER_H) && defined(PC98_H)
	struct PiHeader {
		char far *comment;	// graph_pi_load.*() sets this to NULL
		uint16_t commentlen;	//
		uint8_t mode;	//
		uint8_t n;	// aspect
		uint8_t m;	// aspect
		uint8_t plane;	// usually 4
		char machine[4];	//
		uint16_t maexlen;	// machine extend data length
		void far * maex;	// machine extend data
		uint16_t xsize;
		uint16_t ysize;
		Palette4 palette;
	};

	int MASTER_RET graph_pi_load_pack(
		const char MASTER_PTR *filename,
		struct PiHeader MASTER_PTR *header,
		void far *MASTER_PTR *bufptr
	);
	void MASTER_RET graph_pi_free(
		struct PiHeader MASTER_PTR *header, const void far* image
	);
	void MASTER_RET graph_pack_put_8(
		int x, int y, const void far *linepat, int len
	);
#endif
// ---

// Text RAM
// --------

#define text_width()  80
void MASTER_RET text_boxfilla(
	unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned atrb
);
void MASTER_RET text_clear(void);
void MASTER_RET text_fillca(unsigned ch, unsigned atrb);
void MASTER_RET text_putca(unsigned x, unsigned y, unsigned ch, unsigned atrb);
void MASTER_RET text_puts(unsigned x, unsigned y, const char MASTER_PTR *str);
void MASTER_RET text_putsa(
	unsigned x, unsigned y, const char MASTER_PTR *str, unsigned atrb
);

void MASTER_RET text_cursor_hide(void);
void MASTER_RET text_cursor_show(void);
void MASTER_RET text_systemline_hide(void);
void MASTER_RET text_systemline_show(void);
void MASTER_RET text_show(void);
void MASTER_RET text_hide(void);

// Attributes
#define TX_BLACK    	0x01
#define TX_BLUE     	0x21
#define TX_RED      	0x41
#define TX_MAGENTA  	0x61
#define TX_GREEN    	0x81
#define TX_CYAN     	0xa1
#define TX_YELLOW   	0xc1
#define TX_WHITE    	0xe1
#define TX_BLINK    	2
#define TX_REVERSE  	4
#define TX_UNDERLINE	8
// --------

// VSync
// -----

extern unsigned volatile int vsync_Count1, vsync_Count2;

#define vsync_reset1()	(vsync_Count1 = 0)
#define vsync_reset2()	(vsync_Count2 = 0)

void MASTER_RET vsync_start(void);
void MASTER_RET vsync_end(void);
// -----
/// ----------------------------------------------------------------------

/// Inlined extensions
/// ------------------
#define grcg_boxfill_8(left, top, right, bottom) \
	grcg_byteboxfill_x( \
		((left) / BYTE_DOTS), top, ((right) / BYTE_DOTS), bottom \
	)

#define palette_entry_rgb_show(fn) \
	palette_entry_rgb(fn); \
	palette_show();
/// ------------------
