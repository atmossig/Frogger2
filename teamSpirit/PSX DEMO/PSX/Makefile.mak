######################################################################
#
# FROGGER PS MAKEFILE
#
######################################################################

#
# MACROS
#


# the "unused" warning flags our PRINTF macros, so I've disabled it
# (as well as reporting loads of unused variables, byt WTH?)

#OPT =  -comments-c++ -Wall -Wno-unused -DMAKE_GOLDCD=NO -DWHACK -c -O2 -g -mgpopt -G32 -X0$18000
#OPT0 =  -comments-c++ -Wall -Wno-unused -DMAKE_GOLDCD=NO -DWHACK -c -O2 -g -mgpopt -G16 -X0$18000


# sony examples use 8-high for whatever reason (both are i-cache ok)
# the extra 8000 is only necessary for coverdisk demos

#OPT =  -comments-c++ -Wall -Wno-unused -DMAKE_GOLDCD=NO -c -O2 -g -mgpopt -G32 -X0$80010000
#OPT =  -comments-c++ -Wall -Wno-unused -DMAKE_GOLDCD=NO -DPALMODE -c -O0 -g -mgpopt -G32 -X0$80010000
#OPT =  -comments-c++ -Wall -Wno-unused -DMAKE_GOLDCD=NO -DPALMODE=1 -c -O2 -g -mgpopt -G32 -X0$80010000

#pal master
!if $(PAL) == 1
!if $(MASTER) == 1
OPT = -DPSX_VERSION -DPALMODE=1 -DGOLDCD=1 -c -O2 -mgpopt -G0 -X0$00018000 -comments-c++ 
LIBISL = c:\ps\psx\library\lib\release
!endif
!endif

#pal debug
!if $(PAL) == 1
!if $(MASTER) == 0
#OPT = -DPSX_VERSION -DPALMODE=1 -DGOLDCD=0 -c -O2 -mgpopt -G32 -g -X0$00018000 -comments-c++ -Wall -Wno-unused 
OPT = -D_DEBUG -DPSX_VERSION -DPALMODE=1 -DGOLDCD=0 -c -O2 -mgpopt -G0 -g -X0$00018000 -comments-c++ -Wall -Wno-unused
LIBISL = c:\ps\psx\library\lib\debug
!endif
!endif

#ntsc master
!if $(PAL) == 0
!if $(MASTER) == 1
OPT = -DPSX_VERSION -DPALMODE=0 -DGOLDCD=1 -c -O2 -mgpopt -G0 -X0$00018000 -comments-c++
LIBISL = c:\ps\psx\library\lib\release
!endif
!endif

#ntsc debug	
!if $(PAL) == 0
!if $(MASTER) == 0
#OPT = -DPSX_VERSION -DDEBUG -DPALMODE=0 -DGOLDCD=0 -c -O0 -mgpopt -G0 -g -X0$00018000 -comments-c++ -Wall -Wno-unused
#OPT = -DPSX_VERSION -DPALMODE=0 -O0 -mgpopt -c -G0 -g -X0$00018000 -comments-c++ -Wall -Wno-unused
#OPT = -DPSX_VERSION -DPALMODE=0 -DE3_DEMO -O0 -mgpopt -c -G0 -g -X0$00018000 -comments-c++ -Wno-unused
#OPT = -DPSX_VERSION -DPALMODE=0 -DGOLDCD=0 -c -O2 -mgpopt -G0 -X0$00018000 -comments-c++
OPT = -D_DEBUG -DPSX_VERSION -DPALMODE=0 -DGOLDCD=0 -c -g -O2 -mgpopt -G0 -X0$00010000 -comments-c++
#OPT = -DPSX_VERSION -DPALMODE=0 -DGOLDCD=0 -c -O2 -mgpopt -G0 -g -X0$00018000 -comments-c++
LIBISL = c:\ps\psx\library\lib\debug
!endif
!endif


#
# DEFAULT BUILD RULES
#

.c.obj:
	ccpsx32 $(OPT) $&.c -o$&.obj
	dmpsx -b $&.obj			

#
# MAIN TARGET
# 

frogger.cpe:    actor2.obj anim.obj babyfrog.obj bff_load.obj cam.obj collect.obj\
				custom.obj custom2.obj audio.obj\
				defdata.obj enemies.obj event.obj evenfunc.obj fixed.obj frogger.obj frogmove.obj \
				frontend.obj game.obj general.obj hud.obj language.obj layout.obj \
				levplay.obj maths.obj map_draw.obj memload.obj menus.obj multi.obj overlays.obj \
				path.obj platform.obj psiactor.obj script.obj sndview.obj specfx.obj sprite.obj textover.obj \
				tongue.obj types.obj newpsx.obj map.obj objects.obj textures.obj main.obj world.obj water.obj \
				bbtimer.obj backdrop.obj eff_draw.obj temp_psx.obj world_eff.obj acostab.obj particle.obj \
				psxsprite.obj ptexture.obj cr_lang.obj story.obj timer.obj psxtongue.obj \
				e3_demo.obj bbpsiani.obj quatern.obj asm.obj loadingbar.obj font.obj \
				objviewer.obj options.obj snapshot.obj training.obj fadeout.obj isltex.obj savegame.obj \
				memcard.obj flatpack.obj islvideo.obj frogger.lnk \
				waterbin.obj islmem.obj psi.obj islfile.obj

			
#                slink /psx /c /strip /nostriplib @frogger.lnk,frogger.cpe,frogger.sym,frogger.map
#                slink /l $(LIBISL) /psx /c /nostriplib @frogger.lnk,frogger.cpe,frogger.sym,frogger.map
#				 slink /l $(LIBISL) /psx /c /wo /v /strip /nostriplib @frogger.lnk,frogger.cpe



#				 slink /l $(LIBISL) /psx /c /wo /v /nostriplib @frogger.lnk,frogger.cpe,frogger.sym,frogger.map

				 slink /l $(LIBISL) /psx /c /wo /v /strip @frogger.lnk,frogger.cpe,frogger.sym,frogger.map



#
# SECONDARY TARGET - download and run
#

#run: whack.cpe
#	resetps /p
#	run whack.cpe
#	testmess

#
# SECONDARY TARGET - download and debug
#
#debug: whack.cpe
#	resetps /p
#        dbugpsx whack /e


#
# GAME COMPONENTS
#

waterbin.obj:	waterbin.s water.spt
		asmpsx /zd /l waterbin.s,waterbin.obj

acostab.obj:	acostab.s acostab.bin mpl0.pal mpl1.pal mpl2.pal mpl3.pal mpl4.pal mpl5.pal mpl6.pal mpl7.pal
		asmpsx /zd /l acostab.s,acostab.obj

#incbins.obj:	incbins.s
#		asmpsx /zd /l incbins.s,incbins.obj

asm.obj:	asm.s
		asmpsx /zd /l asm.s,asm.obj

BackDrop.obj:	BackDrop.c Shell.h SonyLibs.h Frontend.h BackDrop.h 
bff_load.obj:	bff_load.c bff_load.h flatpack.h 
custom.obj:	custom.c sonylibs.h shell.h types.h islpsi.h 
custom2.obj:	custom2.c sonylibs.h shell.h islpsi.h enemies.h timer.h psxsprite.h 
Eff_Draw.obj:	Eff_Draw.c particle.h specfx.h Eff_Draw.h cam.h enemies.h platform.h collect.h frogmove.h main.h tongue.h map_draw.h actor2.h fixed.h layout.h 
Main.obj:	Main.c shell.h islfile.h sonylibs.h shell.h islpsi.h Layout.h Main.h Sprite.h frogger.h frontend.h Game.h Actor2.h tongue.h timer.h Water.h BackDrop.h menus.h Eff_Draw.h World_Eff.h World.h audio.h ptexture.h cr_lang.h cam.h menus.h objviewer.h platform.h snapshot.h psxsprite.h memcard.h lang.h fadeout.h objects.h story.h 
map.obj:	map.c Define.h LayOut.h MemLoad.h NewPsx.h map.h cam.h maths.h flatpack.h main.h 
map_draw.obj:	map_draw.c sonylibs.h shell.h types.h map_draw.h main.h actor2.h timer.h world_eff.h layout.h frogger.h psxsprite.h 
Objects.obj:	Objects.c shell.h sonylibs.h islpsi.h Layout.h bff_load.h Objects.h world.h bff_load.h 
psiactor.obj:	psiactor.c actor2.h Main.h Shell.h layout.h bbpsiani.h frogmove.h enemies.h babyfrog.h audio.h game.h timer.h temp_psx.h frogger.h 
Textures.obj:	Textures.c Ultra64.h isltex.h LayOut.h Textures.h Main.h memload.h shell.h flatpack.h 
water.obj:	water.c sonylibs.h shell.h types.h Main.h Map_Draw.h Water.h 
world.obj:	world.c map_draw.h world.h 
actor2.obj:	actor2.c types.h islpsi.h define.h general.h actor2.h frontend.h maths.h frogger.h frogmove.h game.h sprite.h crc32.h Main.h cam.h platform.h tongue.h psiactor.h bff_load.h timer.h bbpsiani.h temp_psx.h 
anim.obj:	anim.c Types.h islpsi.h event.h evenfunc.h anim.h main.h maths.h specfx.h frogger.h frogmove.h platform.h layout.h audio.h 
babyfrog.obj:	babyfrog.c types.h Main.h enemies.h babyfrog.h cam.h game.h map.h frogger.h frogmove.h maths.h specfx.h newpsx.h layout.h hud.h islpsi.h audio.h 
bbtimer.obj:	bbtimer.c bbtimer.h 
block.obj:	block.c fixed.h block.h 
cam.obj:	cam.c types.h islpsi.h frogmove.h cam.h maths.h frogger.h main.h game.h newpsx.h frontend.h layout.h multi.h 
collect.obj:	collect.c NewPsx.h Types.h islpsi.h collect.h maths.h frogger.h hud.h types.h layout.h frontend.h Main.h tongue.h game.h multi.h frogmove.h lang.h audio.h Textures.h 
crc32.obj:	crc32.c crc32.h string.h 
DefData.obj:	DefData.c defdata.h 
enemies.obj:	enemies.c types.h islpsi.h General.h ultra64.h enemies.h frogger.h maths.h babyfrog.h frogmove.h platform.h multi.h anim.h game.h frontend.h map.h newpsx.h cam.h sonylibs.h Main.h maths.h layout.h audio.h textures.h timer.h psiactor.h x:\teamspirit\pcversion\nmehack.txt x:/teamspirit/pcversion/nmehack.txt x:\teamspirit\pcversion\reactive.txt x:/teamspirit/pcversion/reactive.txt 
evenfunc.obj:	evenfunc.c sonylibs.h shell.h islpsi.h evenfunc.h script.h enemies.h frogmove.h platform.h layout.h anim.h game.h map.h Main.h types.h ptexture.h cam.h audio.h 
event.obj:	event.c event.h types.h Main.h script.h 
fixed.obj:	fixed.c fixed.h 
frogger.obj:	frogger.c sonylibs.h shell.h islpsi.h cam.h frogger.h tongue.h anim.h frogmove.h game.h overlays.h platform.h layout.h particle.h 
frogmove.obj:	frogmove.c sonylibs.h shell.h islpsi.h frogmove.h maths.h collect.h cam.h anim.h platform.h tongue.h enemies.h babyfrog.h frontend.h game.h main.h newpsx.h layout.h multi.h menus.h audio.h 
frontend.obj:	frontend.c sonylibs.h shell.h islpsi.h frontend.h game.h define.h layout.h textover.h backdrop.h menus.h frogger.h frogmove.h overlays.h main.h newpsx.h cam.h collect.h hud.h multi.h story.h cam.h levplay.h e3_demo.h babyfrog.h types.h lang.h training.h fadeout.h options.h audio.h temp_psx.h psxsprite.h memcard.h 
game.obj:	game.c map_draw.h world.h world_eff.h water.h audio.h audio.h timer.h map.h game.h types2d.h frogger.h levplay.h frogmove.h cam.h tongue.h hud.h frontend.h menus.h textover.h multi.h layout.h platform.h enemies.h babyfrog.h event.h main.h newpsx.h Main.h actor2.h maths.h story.h fadeout.h specfx.h particle.h 
general.obj:	general.c maths.h map.h main.h frogger.h enemies.h memload.h audio.h general.h 
hud.obj:	hud.c ultra64.h hud.h frogger.h game.h textover.h layout.h overlays.h main.h multi.h collect.h enemies.h babyfrog.h lang.h frontend.h story.h menus.h textures.h audio.h 
language.obj:	language.c ultra64.h language.h 
layout.obj:	layout.c ultra64.h layout.h types.h sprite.h platform.h collect.h map.h textover.h enemies.h babyfrog.h event.h tongue.h game.h cam.h hud.h frontend.h script.h Main.h maths.h newpsx.h menus.h specfx.h e3_demo.h levplay.h frogmove.h fadeout.h backdrop.h story.h particle.h multi.h training.h ptexture.h options.h flatpack.h actor2.h Bff_Load.h Objects.h Map_Draw.h Water.h temp_psx.h audio.h backdrop.h loadingbar.h eff_draw.h temp_psx.h lang.h 
levplay.obj:	levplay.c frontend.h define.h layout.h game.h textover.h levplay.h overlays.h hud.h fadeout.h main.h 
LevProg.obj:	LevProg.c ultra64.h levprog.h hud.h 
maths.obj:	maths.c ultra64.h maths.h newpsx.h layout.h main.h 
memload.obj:	memload.c ultra64.h memload.h evenfunc.h script.h path.h enemies.h babyfrog.h platform.h collect.h Cam.h newpsx.h Main.h maths.h game.h audio.h psiactor.h 
menus.obj:	menus.c ultra64.h menus.h layout.h frontend.h textover.h frogger.h game.h hud.h actor2.h Shell.h frogmove.h cam.h platform.h enemies.h babyfrog.h event.h collect.h levplay.h multi.h game.h lang.h options.h story.h fadeout.h Textures.h temp_psx.h audio.h temp_psx.h memcard.h 
multi.obj:	multi.c define.h specfx.h types.h general.h enemies.h platform.h multi.h textover.h cam.h layout.h game.h frogger.h frontend.h frogmove.h collect.h map.h hud.h babyfrog.h Main.h menus.h textures.h audio.h 
newpsx.obj:	newpsx.c newpsx.h main.h 
overlays.obj:	overlays.c ultra64.h overlays.h maths.h newpsx.h Main.h textures.h 
path.obj:	path.c ultra64.h path.h maths.h Main.h 
platform.obj:	platform.c ultra64.h platform.h frogmove.h anim.h map.h collect.h game.h newpsx.h Main.h maths.h layout.h audio.h timer.h 
script.obj:	script.c codes.h script.h memload.h evenfunc.h actor2.h enemies.h platform.h babyfrog.h map.h frogger.h frogmove.h sprite.h anim.h collect.h game.h newpsx.h Main.h maths.h layout.h cam.h ptexture.h training.h lang.h frontend.h flatpack.h audio.h timer.h temp_psx.h 
sndview.obj:	sndview.c ultra64.h sndview.h 
specfx.obj:	specfx.c ultra64.h specfx.h sprite.h particle.h anim.h frogmove.h path.h enemies.h platform.h collect.h tongue.h multi.h babyfrog.h cam.h textures.h Eff_Draw.h Main.h layout.h 
sprite.obj:	sprite.c layout.h game.h sprite.h textures.h specfx.h psxsprite.h maths.h newpsx.h Shell.h Main.h 
textover.obj:	textover.c textover.h maths.h types2d.h main.h layout.h define.h game.h 
tongue.obj:	tongue.c ultra64.h tongue.h maths.h frogmove.h event.h evenfunc.h sprite.h anim.h game.h enemies.h platform.h babyfrog.h multi.h Cam.h map.h newpsx.h Main.h layout.h textures.h audio.h 
types.obj:	types.c types.h 
pTexture.obj:	pTexture.c pTexture.h 
particle.obj:	particle.c types.h Main.h particle.h layout.h 
audio.obj:	audio.c audio.h main.h memload.h frogger.h layout.h menus.h game.h cam.h 
charsel.obj:	charsel.c charsel.h textover.h frontend.h define.h game.h main.h frogger.h frogmove.h cam.h layout.h maths.h fixed.h overlays.h story.h lang.h 
CR_lang.obj:	CR_lang.c cr_lang.h sonylibs.h shell.h main.h fadeout.h textures.h layout.h audio.h 
story.obj:	story.c layout.h Main.h frogger.h define.h game.h frontend.h story.h 
font.obj:	font.c shell.h font.h textures.h 
file.obj:	file.c file.h main.h 
temp_psx.obj:	temp_psx.c map_draw.h world_eff.h main.h layout.h frogger.h savegame.h water.h actor2.h temp_psx.h cam.h textures.h map_draw.h timer.h memcard.h islvideo.h story.h audio.h 
snapshot.obj:	snapshot.c shell.h snapshot.h 
fadeout.obj:	fadeout.c fadeout.h layout.h sonylibs.h shell.h LoadingBar.h 
savegame.obj:	savegame.c main.h savegame.h layout.h frogger.h menus.h game.h 
training.obj:	training.c define.h overlays.h layout.h main.h training.h game.h sprite.h frogger.h hud.h babyfrog.h frogmove.h 
isltex.obj:	isltex.c isltex.h file.h font.h shell.h 
E3_Demo.obj:	E3_Demo.c frogger.h frontend.h backdrop.h textover.h main.h define.h game.h overlays.h sprite.h e3_demo.h layout.h multi.h levplay.h audio.h 
flatpack.obj:	flatpack.c Shell.h SonyLibs.h layout.h flatpack.h 
bbpsiani.obj:	bbpsiani.c bbpsiani.h 
World_Eff.obj:	World_Eff.c sonylibs.h shell.h types.h main.h map_draw.h actor2.h maths.h layout.h frontend.h sprite.h frogger.h game.h psxsprite.h temp_psx.h World_Eff.h 
options.obj:	options.c ultra64.h menus.h layout.h frontend.h textover.h frogger.h game.h hud.h actor2.h Shell.h frogmove.h cam.h platform.h enemies.h babyfrog.h event.h collect.h levplay.h multi.h game.h lang.h options.h story.h fadeout.h tongue.h backdrop.h Textures.h temp_psx.h audio.h temp_psx.h main.h memcard.h font.h layout.h 
memcard.obj:	memcard.c map_draw.h world_eff.h main.h layout.h frogger.h savegame.h water.h actor2.h temp_psx.h cam.h textures.h map_draw.h timer.h memcard.h lang.h audio.h game.h menus.h options.h 
psxsprite.obj:	psxsprite.c Shell.h overlays.h types2d.h psxsprite.h sprite.h layout.h maths.h textures.h 
timer.obj:	timer.c timer.h shell.h sonylibs.h main.h layout.h 
psxtongue.obj:	psxtongue.c tongue.h map_draw.h psxtongue.h cam.h 
psi.obj:	psi.c shell.h islpsi.h psitypes.h quatern.h timer.h flatpack.h 
quatern.obj:	quatern.c shell.h quatern.h islpsi.h psi.h 
LoadingBar.obj:	LoadingBar.c sonylibs.h Shell.h layout.h main.h lang.h textover.h frogger.h loadingbar.h textures.h frontend.h fadeout.h menus.h options.h game.h multi.h story.h define.h 
ObjViewer.obj:	ObjViewer.c sonylibs.h shell.h types.h islpsi.h psi.h psiactor.h main.h layout.h types2d.h actor2.h cam.h frogger.h objviewer.h world.h 
AM_lang.obj:	AM_lang.c am_lang.h sonylibs.h shell.h main.h 
gametext.obj:	gametext.c global.h texture.h poly.h gametext.h fileio.h pad.h texture.h maingame.h menu.h 
temp.obj:	temp.c shell.h menu.h lscape.h timer.h maingame.h hgram.h comcator.h exreplay.h draw.h particle.h lights.h psiexp.h bezier.h random.h radsound.h rad_file.h rad_eng.h rad_cam.h rad_play.h rad_pan.h rad_coll.h rad_obj.h rad_lite.h rad_weap.h rad_int.h rad_evnt.h radsight.h radsetup.h screenfx.h spotfx.h lecky.h sprite.h explore.h rad_game.h rad_imp.h overlays.h bff_load.h exlearn.h matrix.h demo.h psiexp.h 
islvideo.obj:	islvideo.c islvideo.h 
islfile.obj:	islfile.c islfile.h 
islmem.obj:	islmem.c islmem.h 
