/*
 *	NAME		:	symex_list.h
 *	COMMENT		:	header file for list structure used
 *					to store parsed s-expressions
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */

#ifndef __SYMEX_LIST_H__
#define __SYMEX_LIST_H__

typedef enum { SYMEX_NONE, SYMEX_LIST, SYMEX_BYTES } SYMEX_DATA_TYPE;

struct symex_list_node_t
{
	SYMEX_DATA_TYPE type;
	void *data;
	int length;
	struct symex_list_node_t *prev, *next;
};
typedef struct symex_list_node_t symex_list_node_t;

struct symex_list_t
{
	symex_list_node_t *fnode, *cnode;
};
typedef struct symex_list_t symex_list_t;

/* functions operating on symex_list_t */
symex_list_t*	symex_list_create 	(void);
void			symex_list_destroy 	(symex_list_t*);

/* functions operating on symex_list_node_t */
symex_list_node_t*	symex_list_node_create 	(
								SYMEX_DATA_TYPE type, 
								void *data,
								int length
								);
void				symex_list_node_destroy (symex_list_node_t*);

#endif /* __SYMEX_LIST_H__ */

