m4 -E <application\templates\muhkuh_version.h.m4 >application\muhkuh_version.h.tmp
cmp -s application\muhkuh_version.h.tmp application\muhkuh_version.h || copy application\muhkuh_version.h.tmp application\muhkuh_version.h

m4 -E <installer\inno\templates\muhkuh.iss.m4 >installer\inno\muhkuh.iss.tmp
cmp -s installer\inno\muhkuh.iss.tmp installer\inno\muhkuh.iss || copy installer\inno\muhkuh.iss.tmp installer\inno\muhkuh.iss

