#ifndef MBUFFER_H_INCLUDED
#define MBUFFER_H_INCLUDED


// permanent RAM buffers 
#define AUDIO_HEAP_SIZE	235520
#define MUSIC_DATA_SIZE	100000


// music labels 
extern char _wbank1SegmentRomStart[];
extern char _wbank1SegmentRomEnd[];
extern char _wbank2SegmentRomStart[];
extern char _wbank2SegmentRomEnd[];

extern char _pbank1SegmentRomStart[];
extern char _pbank1SegmentRomEnd[];
extern char _pbank2SegmentRomStart[];
extern char _pbank2SegmentRomEnd[];


extern char _mBank1SegmentRomStart[];
extern char _mBank1SegmentRomEnd[];
extern char _mBank2SegmentRomStart[];
extern char _mBank2SegmentRomEnd[];
extern char _mBank3SegmentRomStart[];
extern char _mBank3SegmentRomEnd[];
extern char _mBank4SegmentRomStart[];
extern char _mBank4SegmentRomEnd[];
extern char _mBank5SegmentRomStart[];
extern char _mBank5SegmentRomEnd[];
extern char _mBank6SegmentRomStart[];
extern char _mBank6SegmentRomEnd[];
extern char _mBank7SegmentRomStart[];
extern char _mBank7SegmentRomEnd[];
extern char _mBank8SegmentRomStart[];
extern char _mBank8SegmentRomEnd[];
extern char _mBank9SegmentRomStart[];
extern char _mBank9SegmentRomEnd[];
extern char _mBank10SegmentRomStart[];
extern char _mBank10SegmentRomEnd[];
extern char _mBank11SegmentRomStart[];
extern char _mBank11SegmentRomEnd[];
extern char _mBank12SegmentRomStart[];
extern char _mBank12SegmentRomEnd[];
extern char _mBank13SegmentRomStart[];
extern char _mBank13SegmentRomEnd[];
extern char _mBank14SegmentRomStart[];
extern char _mBank14SegmentRomEnd[];
extern char _mBank15SegmentRomStart[];
extern char _mBank15SegmentRomEnd[];
extern char _mBank16SegmentRomStart[];
extern char _mBank16SegmentRomEnd[];
extern char _mBank17SegmentRomStart[];
extern char _mBank17SegmentRomEnd[];
extern char _mBank18SegmentRomStart[];
extern char _mBank18SegmentRomEnd[];
extern char _mBank19SegmentRomStart[];
extern char _mBank19SegmentRomEnd[];
extern char _mBank20SegmentRomStart[];
extern char _mBank20SegmentRomEnd[];


// 'dream' generate labels 
extern char  BaseNote [];
extern float Detune   [];
extern char  *fxs     [];
extern int   Priority [];


#define WBANK_START	_wbank1SegmentRomStart
#define PBANK_START _pbank1SegmentRomStart
#define PBANK_END	_pbank1SegmentRomEnd

#define WBANK2_START _wbank2SegmentRomStart
#define PBANK2_START _pbank2SegmentRomStart
#define PBANK2_END	 _pbank2SegmentRomEnd


/*
// ROM addresses 
#define WBANK_START	_wbankSegmentRomStart
#define PBANK_START _pbankSegmentRomStart
#define PBANK_END	_pbankSegmentRomEnd
#define MUSIC_START	_tuneSegmentRomStart
#define MUSIC_END	_tuneSegmentRomEnd
*/


extern unsigned char	audio_memory[];
extern musConfig musicPlayer;

/*
extern unsigned char	pointer_buf	[];
extern unsigned int		tune_buf[];
*/


//enum
//{
/*
#define	SFX_FOOTSTEP1		FX_NULL
#define	SFX_FOOTSTEP2		FX_NULL
#define	SFX_SPLASH1			FX_NULL
#define	SFX_SPLASH2			FX_NULL
#define	SFX_HANDBOUNCE		FX_BOUNCE
#define	SFX_HANDJUMP		FX_HAND_JUMP
#define	SFX_HANDWHACK		FX_WHACK
#define	SFX_HANDLAND		FX_HAND_LAND
#define	SFX_SWITCH			FX_NULL
#define	SFX_SPIKESWITCH		FX_NULL
#define	SFX_GATEOPEN		FX_NULL
#define	SFX_HEAVYLAND		FX_NULL
#define	SFX_SPIKE			FX_NULL
#define	SFX_HANDDROWN		FX_NULL

#define	SFX_LOBSTER			FX_NULL
#define	SFX_CHEST			FX_NULL
#define	SFX_CUTLASS			FX_NULL
#define	SFX_BIGBALL			FX_NULL

#define	SFX_SWISH			FX_NULL
#define	SFX_MOTOR			FX_NULL
#define	SFX_SQUASH			FX_NULL
#define	SFX_SNAPPY			FX_NULL
#define	SFX_TELEPORT		FX_NULL
#define	SFX_BALLBURST		FX_BURST
#define	SFX_COLLECTABLE		FX_NULL
*///};





#endif