#include <stdbool.h>
#ifndef bool
	"error: bool is not defined"
#endif
#ifndef false
	"error: false is not defined"
#endif
#if false
	"error: false is not 0"
#endif
#ifndef true
	"error: true is not defined"
#endif
#if true != 1
	"error: true is not 1"
#endif
#ifndef __bool_true_false_are_defined
	"error: __bool_true_false_are_defined is not defined"
#endif

struct s { _Bool s: 1; _Bool t; } s;

char a[true == 1 ? 1 : -1];
char b[false == 0 ? 1 : -1];
char c[__bool_true_false_are_defined == 1 ? 1 : -1];
char d[(bool) 0.5 == true ? 1 : -1];
/* See body of main program for 'e'.  */
char f[(_Bool) 0.0 == false ? 1 : -1];
char g[true];
char h[sizeof (_Bool)];
char i[sizeof s.t];
enum { j = false, k = true, l = false * true, m = true * 256 };
/* The following fails for
 * HP aC++/ANSI C B3910B A.05.55 [Dec 04 2003].
 */
_Bool n[m];
char o[sizeof n == m * sizeof n[0] ? 1 : -1];
char p[-1 - (_Bool) 0 < 0 && -1 - (bool) 0 < 0 ? 1 : -1];
/* Catch a bug in an HP-UX C compiler.  See
 * http://gcc.gnu.org/ml/gcc-patches/2003-12/msg02303.html
 * http://lists.gnu.org/archive/html/bug-coreutils/2005-11/msg00161.html
 */
_Bool q = true;
_Bool *pq = &q;

int main ()
{
	bool e = &s;
	*pq |= q;
	*pq |= ! q;
	/* Refer to every declared value, to avoid compiler optimizations.  */
	return (!a + !b + !c + !d + !e + !f + !g + !h + !i + !!j + !k + !!l
	        + !m + !n + !o + !p + !q + !pq);
	
	;
	return 0;
}
