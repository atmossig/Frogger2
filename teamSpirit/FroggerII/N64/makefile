c_files = $(wildcard *.c) $(wildcard *.C)
c_obj_files = $(patsubst %.c,%.obj,$(wildcard *.c)) $(patsubst %.c,%.obj,$(wildcard *.C))

###############################################################
##  Compiler

compiler = ccn64
assembler = asmn64

;compile_opt = -c -O0 -G0 -I c:/work/n64/include
compile_opt = -g -c -O0 -G0 -I c:/work/n64/include
assemble_opt = /l


###############################################################
##  Linker

linker = psylink

objects = $(patsubst \/%,\\%,$(patsubst %.c2,%.nob,$(wildcard objects/*.c2)))
	
textures = $(patsubst textures/%,textures\\%,$(patsubst %.ntb,%.fla,$(wildcard textures/*.ntb)))
	
###############################################################
##  Main Binary

block.bin: $(textures) $(c_obj_files) $(objects) $(textures) snmain.obj block.lnk levbanks\append.lnk
	copy /B block.lnk+levbanks\append.lnk block2.lnk
    $(linker) /s /p /c /m /g @block2.lnk,block.bin,block.sym,block.map
    setcsum block.bin 0x80100000

link:
	copy /B block.lnk+levbanks\append.lnk block2.lnk
    $(linker) /s /p /c /m /g @block2.lnk,block.bin,block.sym,block.map
    setcsum block.bin 0x80100000

snmain.obj: snmain.s
	$(assembler) $(assemble_opt) snmain,snmain

%.fla: %.ntb
        fpack textures\\$(patsubst textures%,%,$<)

#ccn64 -c -O0 $<.c
#psylink /c /p $<.obj,$<.nib

%.nob: %.c2
	copy $(patsubst objects/%,objects\\%,$<) $(patsubst objects/%.c2,objects\\%.c,$<)
	ccn64 -c -O0 -I c:/work/n64/include $(patsubst objects/%.c2,objects/%.c,$<)
	psylink /c /p $(patsubst objects/%.c2,objects/%.obj,$<),$(patsubst objects/%.c2,objects/%.bin,$<)
	fpack objects\\$(patsubst objects/%.c2,%,$<).bin
	copy $(patsubst objects/%.c2,objects\\%.fla,$<) $(patsubst objects/%.c2,objects\\%.nob,$<)
	fpack $(patsubst objects/%.c2,objects\\%.nib,$<)
	del $(patsubst objects/%.c2,objects\\%.c,$<)


# $(c_obj_files): $(c_files)
#	$(compiler) $(compile_opt) $(c_files)

%.obj: %.c
	$(compiler) $(compile_opt) $<

