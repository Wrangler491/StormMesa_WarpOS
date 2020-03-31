/*
 *  common functions to mange double-linked-lists
 *  Copyright (C) 1997-8  Niels Froehling <Niels.Froehling@Informatik.Uni-Oldenburg.DE>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LISTS_H
#define LISTS_H

struct nnode {
  struct nnode *succ;						/* Pointer to next (successor) */
  struct nnode *pred;						/* Pointer to previous (predecessor) */
  int data;
  void *obj;								/* Pointer to object in node*/
};

struct nlist {
  struct nnode *head;
  struct nnode *tail;
  struct nnode *tailpred;
  int nodes;
};

extern void nNewList( struct nlist *list);
void nAddHead( struct nlist *list,  struct nnode *node);
extern void nAddTail( struct nlist *list,  struct nnode *node);
struct nnode *nRemHead( struct nlist *list);
struct nnode *nRemTail( struct nlist *list);
void nRemove( struct nlist *list,  struct nnode *node);
void nInsert( struct nlist *list,  struct nnode *node,
	      struct nnode *pred);
void npreInsert( struct nlist *list,  struct nnode *node,
		 struct nnode *succ);
struct nnode *nGetHead( struct nlist *list);
struct nnode *nGetTail( struct nlist *list);
struct nnode *nSuccNode( struct nnode *node);
void nMoveList( struct nlist *slist,  struct nlist *dlist);
void nAppendList( struct nlist *preList,
		  struct nlist *postList);

/* from higher values to lower */
struct nnode *nFindNode( struct nnode *startnode,
			 int numnodes,  int data);
void nEnqueue( struct nlist *list,  struct nnode *node);
void nMoveNodeSorted( struct nlist *slist,  struct nlist *dlist,
		      struct nnode *node);
void nSortListWeighted( struct nlist *list,  int radix);
void nSortListLinear( struct nlist *list,  int radix);
void nSortListLinearMax( struct nlist *list,  int radix,
			 int radix_divide);

/* from lower values to higher */
struct nnode *nFindNodeReverse( struct nnode *startnode,
			        int numnodes,  int data);
void nEnqueueReverse( struct nlist *list,  struct nnode *node);
void nMoveNodeSortedReverse( struct nlist *slist,  struct nlist *dlist,
			     struct nnode *node);
void nSortListWeightedReverse( struct nlist *list,  int radix);
void nSortListLinearReverse( struct nlist *list,  int radix);
void nSortListLinearMaxReverse( struct nlist *list,  int radix,
			        int radix_divide);

void nAddTailext(struct nlist *list,  struct nnode *node, void *ptr);
void *nGetNextext(struct nnode *node);

#define	nGetNext(node)		nSuccNode(node)
#define nGetHeadext(node)	nGetHead(node)->obj
#define nGetTailext(node)	nGetTail(node)->obj

#endif								/* LISTS_H */
