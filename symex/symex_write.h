/*
 *	NAME		:	symex_write.h
 *	COMMENT		:	declares functions which write s-expressions
 *					stored in a list structure to disk
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */

#ifndef __SYMEX_WRITE_H__
#define __SYMEX_WRITE_H__

#include <stdio.h>

#include "symex_list.h"

void	symex_print_to_file (symex_list_node_t*, FILE*fp);
int		symex_write_to_file (symex_list_t*, const char* filename);

#endif /* __SYMEX_WRITE_H__ */
