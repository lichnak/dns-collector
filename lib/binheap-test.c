/*
 *	Sherlock Library -- Binomial Heaps: Testing
 *
 *	(c) 2003 Martin Mares <mj@ucw.cz>
 *
 *	This software may be freely distributed and used according to the terms
 *	of the GNU Lesser General Public License.
 */

#include "lib/lib.h"

#include <stdio.h>
#include <string.h>

#define BH_PREFIX(x) bht_##x
#define BH_WANT_INSERT
#define BH_WANT_FINDMIN
#define BH_WANT_DELETEMIN
#include "lib/binheap-node.h"

struct item {
  struct bht_node n;
  uns key;
};

static inline uns bht_key(struct bht_node *n)
{
  return ((struct item *)n)->key;
}

static inline uns bht_less(struct bht_node *a, struct bht_node *b)
{
  return bht_key(a) < bht_key(b);
}

static void
bht_do_dump(struct bht_node *a, struct bht_node *expected_last, uns offset)
{
  if (!a)
    return;
  printf("%*s", offset, "");
  printf("[%d](%d)%s\n", a->order, bht_key(a), a == expected_last ? " L" : "");
  for (struct bht_node *b=a->first_son; b; b=b->next_sibling)
    bht_do_dump(b, a->last_son, offset+1);
}

static void
bht_dump(struct bht_heap *h)
{
  printf("root\n");
  for (struct bht_node *b=h->root.first_son; b; b=b->next_sibling)
    bht_do_dump(b, b->last_son, 1);
}

#include "lib/binheap.h"

int main(void)
{
  uns i;
  struct bht_heap h;
#define N 1048576
#define K(i) ((259309*i+1009)%N)

  bht_init(&h);

  for (i=0; i<N; i++)
    {
      struct item *a = xmalloc_zero(sizeof(*a));
      a->key = K(i);
      // printf("Insert %d\n", a->key);
      bht_insert(&h, &a->n);
      // bht_dump(&h);
    }
  bht_dump(&h);
  ASSERT(bht_key(bht_findmin(&h)) == 0);
  for (i=0; i<N; i++)
    {
      struct item *a = (struct item *) bht_deletemin(&h);
      // printf("\nDeleted %d:\n", a->key);
      ASSERT(a->key == i);
      // bht_dump(&h);
    }
  bht_dump(&h);

  return 0;
}
