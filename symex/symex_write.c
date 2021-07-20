/*
 *	NAME		:	symex_write.c
 *	COMMENT		:	implements functions used to write data
 *					stored in lists to file in s-expression form
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */

#ifdef __cplusplus
	extern "C" {
#endif

#include "symex_write.h"

void
symex_print_to_file (symex_list_node_t*node, FILE* fp)
{
	while (node != NULL)
	{
		if (node->type == SYMEX_BYTES)
		{
			fprintf (fp, " %d:", node->length);
			fwrite (node->data, 1, node->length, fp);
			fprintf (fp, " ");
		}
		else if (node->type == SYMEX_LIST)
		{
			fprintf (fp, "(");
			symex_print_to_file (node->data, fp);
			fprintf (fp, ")");
		}
		node = node->next;
	}
}

int
symex_write_to_file (symex_list_t *list, const char* filename)
{
	FILE* fp;
	
	fp = fopen (filename, "w");
	if (fp == NULL)
		return -1;
	
	symex_print_to_file (list->fnode, fp);

	return 0;
}

#ifdef __cplusplus
	} /* extern "C" */
#endif