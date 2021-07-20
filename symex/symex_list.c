/*
 *	NAME		:	symex_list.c
 *	COMMENT		:	Implements functions declared in symex_list.h
 *					that operate on symex_list_t and symex_list_node_t
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */

#ifdef __cplusplus
	extern "C" {
#endif

/* Standard unclude files */
#include <stdio.h>
#include <stdlib.h>

/* User-Defined include files */
#include "symex_list.h"

/* functions related symex_list_t begin */

symex_list_t*
symex_list_create (void)
{
	symex_list_t* list;

	list = malloc (sizeof (symex_list_t));
	if (list == NULL)
		return NULL;

	list->fnode = NULL;
	list->cnode = NULL;

	return list;
}

void
symex_list_destroy (symex_list_t *list)
{
	if (list != NULL)
	{
		if (list->fnode != NULL)
			free (list->fnode);
		free (list);
	}
}

/* functions related to symex_list_t end */

/* functions related to symex_list_node_t begin */

symex_list_node_t*
symex_list_node_create (SYMEX_DATA_TYPE type, void *data, int length)
{
	symex_list_node_t* node;

	node = malloc (sizeof (symex_list_node_t));
	if (node == NULL)
		return NULL;

	node->type = type;
	node->data = data;
	node->length = length;
	node->next = NULL;
	node->prev = NULL;
	
	return node;
}

void
symex_list_node_destroy (symex_list_node_t* node)
{
	if (node != NULL)
	{
		if (node->data != NULL)
		{
			if (node->type == SYMEX_LIST)
				symex_list_node_destroy (node->data);
			else
				free (node->data);
		}
		if (node->next != NULL)
			symex_list_node_destroy (node->next);
		free (node);
	}
}

/* functions related to symex_list_node_t end */

#ifdef __cplusplus
	} /* extern "C" */
#endif