/*
 *	NAME		:	symex_parse.h
 *	COMMENT		:	header file defining functions for parsing
 *					s-expressions
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */

#ifndef __SYMEX_PARSE_H__
#define __SYMEX_PARSE_H__

#include "symex_list.h"

/* main functions */
symex_list_t*	symex_parse_from_file 	(FILE*);
symex_list_t*	symex_parse_file_string (char*);

int symex_get_char_from_file 	(void);
int symex_get_char_from_string 	(void);

/* parsing functions */
symex_list_node_t*	symex_parse_symex			(void);
symex_list_node_t*	symex_parse_list			(void);
symex_list_node_t*	symex_parse_string			(void);
symex_list_node_t*	symex_parse_simple_string 	(void);
symex_list_node_t*	symex_parse_bytes			(int length);

#endif /* __SYMEX_PARSE_H__ */
