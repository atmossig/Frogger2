#ifndef MBUFFER_H_INCLUDED
#define MBUFFER_H_INCLUDED


// permanent RAM buffers 
#define AUDIO_HEAP_SIZE	300000


// music labels 
extern char _wbank1SegmentRomStart[];
extern char _wbank1SegmentRomEnd[];
extern char _pbank1SegmentRomStart[];
extern char _pbank1SegmentRomEnd[];

extern char _wbank2SegmentRomStart[];
extern char _wbank2SegmentRomEnd[];
extern char _pbank2SegmentRomStart[];
extern char _pbank2SegmentRomEnd[];


extern char _mBank1SegmentRomStart[];
extern char _mBank1SegmentRomEnd[];
/*extern char _mBank2SegmentRomStart[];
extern char _mBank2SegmentRomEnd[];
extern char _mBank3SegmentRomStart[];
extern char _mBank3SegmentRomEnd[];
extern char _mBank4SegmentRomStart[];
extern char _mBank4SegmentRomEnd[];
extern char _mBank5SegmentRomStart[];
extern char _mBank5SegmentRomEnd[];
extern char _mBank6SegmentRomStart[];
extern char _mBank6SegmentRomEnd[];
/*extern char _tbank7SegmentRomStart[];
extern char _tbank7SegmentRomEnd[];
extern char _tbank8SegmentRomStart[];
extern char _tbank8SegmentRomEnd[];
extern char _tbank9SegmentRomStart[];
extern char _tbank9SegmentRomEnd[];
extern char _tbank10SegmentRomStart[];
extern char _tbank10SegmentRomEnd[];
extern char _tbank11SegmentRomStart[];
extern char _tbank11SegmentRomEnd[];
extern char _tbank12SegmentRomStart[];
extern char _tbank12SegmentRomEnd[];
extern char _tbank13SegmentRomStart[];
extern char _tbank13SegmentRomEnd[];
extern char _tbank14SegmentRomStart[];
extern char _tbank14SegmentRomEnd[];
extern char _tbank15SegmentRomStart[];
extern char _tbank15SegmentRomEnd[];
extern char _tbank16SegmentRomStart[];
extern char _tbank16SegmentRomEnd[];
extern char _tbank17SegmentRomStart[];
extern char _tbank17SegmentRomEnd[];
extern char _tbank18SegmentRomStart[];
extern char _tbank18SegmentRomEnd[];
extern char _tbank19SegmentRomStart[];
extern char _tbank19SegmentRomEnd[];
extern char _tbank20SegmentRomStart[];
extern char _tbank20SegmentRomEnd[];
extern char _tbank21SegmentRomStart[];
extern char _tbank21SegmentRomEnd[];
extern char _tbank22SegmentRomStart[];
extern char _tbank22SegmentRomEnd[];
extern char _tbank23SegmentRomStart[];
extern char _tbank23SegmentRomEnd[];
extern char _tbank24SegmentRomStart[];
extern char _tbank24SegmentRomEnd[];
extern char _tbank25SegmentRomStart[];
extern char _tbank25SegmentRomEnd[];
extern char _tbank26SegmentRomStart[];
extern char _tbank26SegmentRomEnd[];
extern char _tbank27SegmentRomStart[];
extern char _tbank27SegmentRomEnd[];
extern char _tbank28SegmentRomStart[];
extern char _tbank28SegmentRomEnd[];
extern char _tbank29SegmentRomStart[];
extern char _tbank29SegmentRomEnd[];
extern char _tbank30SegmentRomStart[];
extern char _tbank30SegmentRomEnd[];
extern char _tbank31SegmentRomStart[];
extern char _tbank31SegmentRomEnd[];
extern char _tbank32SegmentRomStart[];
extern char _tbank32SegmentRomEnd[];
extern char _tbank33SegmentRomStart[];
extern char _tbank33SegmentRomEnd[];
extern char _tbank34SegmentRomStart[];
extern char _tbank34SegmentRomEnd[];
extern char _tbank35SegmentRomStart[];
extern char _tbank35SegmentRomEnd[];
extern char _tbank36SegmentRomStart[];
extern char _tbank36SegmentRomEnd[];
extern char _tbank37SegmentRomStart[];
extern char _tbank37SegmentRomEnd[];
extern char _tbank38SegmentRomStart[];
extern char _tbank38SegmentRomEnd[];
extern char _tbank39SegmentRomStart[];
extern char _tbank39SegmentRomEnd[];
extern char _tbank40SegmentRomStart[];
extern char _tbank40SegmentRomEnd[];
extern char _tbank41SegmentRomStart[];
extern char _tbank41SegmentRomEnd[];
extern char _tbank42SegmentRomStart[];
extern char _tbank42SegmentRomEnd[];
extern char _tbank43SegmentRomStart[];
extern char _tbank43SegmentRomEnd[];
extern char _tbank44SegmentRomStart[];
extern char _tbank44SegmentRomEnd[];
extern char _tbank45SegmentRomStart[];
extern char _tbank45SegmentRomEnd[];
extern char _tbank46SegmentRomStart[];
extern char _tbank46SegmentRomEnd[];
extern char _tbank47SegmentRomStart[];
extern char _tbank47SegmentRomEnd[];
extern char _tbank48SegmentRomStart[];
extern char _tbank48SegmentRomEnd[];
extern char _tbank49SegmentRomStart[];
extern char _tbank49SegmentRomEnd[];
extern char _tbank50SegmentRomStart[];
extern char _tbank50SegmentRomEnd[];
extern char _tbank51SegmentRomStart[];
extern char _tbank51SegmentRomEnd[];
extern char _tbank52SegmentRomStart[];
extern char _tbank52SegmentRomEnd[];
extern char _tbank53SegmentRomStart[];
extern char _tbank53SegmentRomEnd[];
extern char _tbank54SegmentRomStart[];
extern char _tbank54SegmentRomEnd[];
extern char _tbank55SegmentRomStart[];
extern char _tbank55SegmentRomEnd[];
extern char _tbank56SegmentRomStart[];
extern char _tbank56SegmentRomEnd[];
extern char _tbank57SegmentRomStart[];
extern char _tbank57SegmentRomEnd[];
extern char _tbank58SegmentRomStart[];
extern char _tbank58SegmentRomEnd[];
extern char _tbank59SegmentRomStart[];
extern char _tbank59SegmentRomEnd[];
extern char _tbank60SegmentRomStart[];
extern char _tbank60SegmentRomEnd[];
extern char _tbank61SegmentRomStart[];
extern char _tbank61SegmentRomEnd[];

  */


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