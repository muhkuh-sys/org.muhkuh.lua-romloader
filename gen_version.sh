m4 -E <application/templates/muhkuh_version.h.m4 >application/muhkuh_version.h.tmp
cmp -s application/muhkuh_version.h.tmp application/muhkuh_version.h || cp application/muhkuh_version.h.tmp application/muhkuh_version.h

m4 -E <application/templates/muhkuh.rc.m4 >application/muhkuh.rc.tmp
cmp -s application/muhkuh.rc.tmp application/muhkuh.rc || cp application/muhkuh.rc.tmp application/muhkuh.rc


m4 -E <regkuh/templates/regkuh_version.h.m4 >regkuh/regkuh_version.h.tmp
cmp -s regkuh/regkuh_version.h.tmp regkuh/regkuh_version.h || cp regkuh/regkuh_version.h.tmp regkuh/regkuh_version.h

m4 -E <regkuh/templates/regkuh.rc.m4 >regkuh/regkuh.rc.tmp
cmp -s regkuh/regkuh.rc.tmp regkuh/regkuh.rc || cp regkuh/regkuh.rc.tmp regkuh/regkuh.rc


m4 -E <serverkuh/templates/serverkuh_version.h.m4 >serverkuh/serverkuh_version.h.tmp
cmp -s serverkuh/serverkuh_version.h.tmp serverkuh/serverkuh_version.h || cp serverkuh/serverkuh_version.h.tmp serverkuh/serverkuh_version.h

m4 -E <serverkuh/templates/serverkuh.rc.m4 >serverkuh/serverkuh.rc.tmp
cmp -s serverkuh/serverkuh.rc.tmp serverkuh/serverkuh.rc || cp serverkuh/serverkuh.rc.tmp serverkuh/serverkuh.rc
