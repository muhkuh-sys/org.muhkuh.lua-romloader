m4 -E <application\templates\muhkuh_version.h.m4 >application\muhkuh_version.h.tmp
cmp -s application\muhkuh_version.h.tmp application\muhkuh_version.h || copy application\muhkuh_version.h.tmp application\muhkuh_version.h

m4 -E <application\templates\muhkuh.rc.m4 >application\muhkuh.rc.tmp
cmp -s application\muhkuh.rc.tmp application\muhkuh.rc || copy application\muhkuh.rc.tmp application\muhkuh.rc

