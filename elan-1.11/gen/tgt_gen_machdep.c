/*
   File: tgt_gen_machdep.c
   Decides for which target to code

   CVS ID: "$Id: tgt_gen_machdep.c,v 1.8 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "tgt_gen_machdep.h"
#include "options.h"

string local_label_prefix;
string asm_underscore;
cpu_type tcpu;
int old_os;

static void init_machine_dependencies ()
{ local_label_prefix = new_string ("");
  asm_underscore = new_string ("");
  tcpu = TGT_UNDEF;
  old_os = 0;
}

static int string_has_prefix (char *s, char *pref)
{ char *s_ptr, *pref_ptr;
  for (s_ptr = s, pref_ptr = pref; (*s_ptr) && (*pref_ptr); s_ptr++, pref_ptr++)
    if (*s_ptr != *pref_ptr) return (0);
  if (!(*pref_ptr)) return (1);
  return (0);
};

static void determine_target_processor ()
{ if (string_has_prefix (target_cpu, "sun4") || string_has_prefix (target_cpu, "sparc"))
    tcpu = TGT_SPARC;
  else if (streq (target_cpu, "i386") || streq (target_cpu, "i486") ||
	   streq (target_cpu, "i586") || streq (target_cpu, "i686"))
    tcpu = TGT_INTELx86;
  else if (streq (target_cpu, "amd64") || streq (target_cpu, "x86_64"))
    tcpu = TGT_AMD64;
  else if (streq (target_cpu, "alpha"))
    { local_label_prefix = new_string ("$");
      tcpu = TGT_ALPHA;
    }
  else if (streq (target_cpu, "vax"))
    { local_label_prefix = new_string (".");
      tcpu = TGT_VAX;
    }
  if (tcpu == TGT_UNDEF)
    dcg_panic ("   can not code for a %s processor", target_cpu);
}

static void determine_os_quirks ()
{ if ((string_has_prefix (target_os, "SunOS") || string_has_prefix (target_os, "sunos")) &&
      (target_os[5] == '4'))
    { dcg_hint ("      generating code for a rather old os");
      old_os = 1;
      dcg_hint ("      generating leading underscores for public C routine names");
      detach_string (&asm_underscore);
      asm_underscore = new_string ("_");
    }
  else if ((tcpu == TGT_INTELx86) && streq (target_os, "mingw32"))
    { dcg_hint ("      generating code for mingw32");
      old_os = 1;
      detach_string (&asm_underscore);
      asm_underscore = new_string ("_"); 
    };
}

void determine_target_dependencies ()
{ dcg_warning (0, "   determining target properties...");
  dcg_hint ("      generating code for a %s-%s target", target_cpu, target_os);
  init_machine_dependencies ();
  determine_target_processor ();
  determine_os_quirks ();
}
