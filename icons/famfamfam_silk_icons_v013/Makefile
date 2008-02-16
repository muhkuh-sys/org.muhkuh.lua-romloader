#--------------------------------------
#
# Simple makefile to convert all png images to xpm.
# Needs 'pngtopnm' and 'ppmtoxpm' from the netpbm suite and
# the stream editor 'sed'.
# It will most probably only run on linux.
#

ICONS_PNG = $(basename $(wildcard *.png))
ICONS_XPM = $(addsuffix .xpm,$(ICONS_PNG))

all: $(ICONS_XPM)

%.pnm: %.png
	pngtopnm $< >$@

%.apnm: %.png
	pngtopnm -alpha $< >$@

%.xpm: %.apnm %.pnm
	ppmtoxpm -name=icon_famfamfam_silk_$(notdir $(basename $@)) -alphamask=$^ | sed -e's/static char/static const char/' >$@

clean:
	rm -f $(ICONS_XPM)
	rm -f $(addsuffix .pnm,$(ICONS_PNG))
	rm -f $(addsuffix .apnm,$(ICONS_PNG))

