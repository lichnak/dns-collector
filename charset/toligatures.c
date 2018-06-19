/*
 *	The UniCode Library -- Table of Ligatures
 *
 *	(c) 2003 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include <ucw/lib.h>
#include <charset/unicat.h>
#include <charset/U-ligatures.h>

const u16 *
Uexpand_lig(uint x)
{
  return _U_lig_hash[x % LIG_HASH_SIZE];
}
