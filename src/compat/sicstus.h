/*  $Id$

    Part of SWI-Prolog

    Author:        Jan Wielemaker
    E-mail:        J.Wielemaker@cs.vu.nl
    WWW:           http://www.swi-prolog.org
    Copyright (C): 1985-2010, VU University Amsterdam

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This include file emulates <sicstus.h> for SWI-Prolog.

This  version  was   written   to   get    the   Alpino   parser   suite
(http://www.let.rug.nl/vannoord/alp/Alpino/) to run on SWI-Prolog. It is
by no means complete and intended  as   a  `living document'. So, please
contribute   your   changes.    See     also    library(qpforeign)   and
library(dialect/sicstus).

Most should be(come) fully compatible. Some  issues are hard to emulate.
Please checks the notes for:

	* SP_raise_exception()
	* SP_fail()
	* SP_to_os()
	* SP_from_os()
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef SICSTUS_H_INCLUDED
#define SICSTUS_H_INCLUDED
#include <SWI-Prolog.h>

typedef term_t SP_term_ref;
typedef predicate_t SP_pred_ref;

#define SP_ERROR  -1
#define SP_FAILURE 0
#define SP_SUCCESS 1


		 /*******************************
		 *  READING AND WRITING TERMS	*
		 *******************************/

#define SP_new_term_ref() PL_new_term_ref()
#define SP_get_list(l,h,t) PL_get_list(l,h,t)
#define SP_cons_list(l,h,t) PL_cons_list(l,h,t)
#define SP_put_integer(t,i) PL_put_integer(t,i)
#define SP_put_float(t,f) PL_put_float(t,f)
#define SP_put_string(t,f) PL_put_atom_chars(t,f)

		 /*******************************
		 * RETURN CODES AND EXCEPTIONS	*
		 *******************************/

/* Note: In SICSTus, these functions put something into the
   environment that is picked up by the wrapper.  These versions
   return immediately!  The SICStus route is quite complicated
   because of the multi-threading nature of SWI-Prolog.  With
   some care, it should be possible to to place these calls such
   that both SICStus and SWI are happy.
*/

#define SP_raise_exception(t) do { PL_raise_exception(t); return 0; } while(0)
#define SP_fail()	      return FALSE


		 /*******************************
		 *	 C CALLING PROLOG	*
		 *******************************/

#define SP_predicate(name,arity,module) PL_predicate(name,arity,module)

inline int
SP_query(SP_pred_ref predicate, ...)
{ atom_t name;
  int arity;
  module_t module;
  fid_t fid;
  term_t t0;
  va_list args;

  if ( !(fid = PL_open_foreign_frame()) )
    return SP_ERROR;

  PL_predicate_info(predicate, &name, &arity, &module)

  if ( !(t0 = PL_new_term_refs(arity)) )
  { PL_close_foreign_frame(fid);
    return SP_ERROR;
  }

  va_start(args, predicate);
  for(i=0; i<arity; i++)
  { term_t a = va_arg(args, term_t);
    PL_put_term(t0+i, a);
  }
  va_end(args);

  if ( !(qid=PL_open_query(NULL, PL_Q_CATCH_EXCEPTION, predicate, t0)) )
    return SP_ERROR;
  if ( !PL_next_solution(qid) )
  { term_t ex = PL_exception(qid);

    PL_cut_query(qid);
    PL_close_foreign_frame(fid);
    if ( ex )
      return SP_ERROR;
    return SP_FAILURE;
  }
  PL_cut_query(qid);
  PL_close_foreign_frame(fid);

  return SP_SUCCESS;
}


#define SP_malloc(n) PL_malloc(n)
#define SP_realloc(p,n) PL_realloc(p,n)
#define SP_free(p) PL_free(p)


/* These functions perform string-encoding conversion between Prolog and
   the OS-native representation.  This is done using the conversion-flag
   in SWI-Prolog's PL_get_chars() and PL_unify_chars() routines. I'm not
   sure how we should deal with this.
*/

#define SP_to_os(s,c) (s)
#define SP_from_os(s,c) (s)

#endif /*SICSTUS_H_INCLUDED*/
