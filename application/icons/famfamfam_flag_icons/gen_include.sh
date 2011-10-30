#! /bin/sh

rm muhkuh_flags.h muhkuh_flags.cpp

touch muhkuh_flags.h
touch muhkuh_flags.cpp

for F in `ls *.xpm` ;
	do {
		echo extern const char *icon_famfamfam_silk_`basename ${F} .xpm`[`wc -l ${F} | sed "s/\([0-9]*\) *[_.0-9a-zA-Z]*/\\1/"`-4]\; >> muhkuh_flags.h
		echo \#include \"icons/famfamfam_flag_icons/${F}\" >> muhkuh_flags.cpp
	}
done
