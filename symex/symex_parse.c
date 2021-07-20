/*
 *	NAME		:	symex_parse.c
 *	COMMENT		:	Implements functions used to parse
 *					s-expressions
 *	COPYRIGHT	:	(c)2006 Hariprasad Govardhanam
 *					Released under BSD License.
 *					See COPYRIGHT file that accompanies this
 *					code for details
 *	CHANGES		:
 */
#ifdef __cplusplus
	extern "C" {
#endif

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* user defined include files */
#include "symex_parse.h"

#ifndef NO_DEBUG
#	define DBG_MSG(msg)
#else
#	define DBG_MSG(msg) fprintf (stderr, "%s\n", msg)
#endif

#define MAX_BUF_SIZE 255

FILE *fp = NULL;
char *str = NULL;
int prev_char = -1, cur_char = -1, cur_pos;

/* char reading functions */
int (*symex_get_char) (void);
void symex_unget_char (void);

/* main functions */
symex_list_t*
symex_parse_from_file (FILE* f)
{
	symex_list_node_t *node;
	symex_list_t *list;

	if (f == NULL)
		return NULL;
	
	fp = f;

	symex_get_char = symex_get_char_from_file;

	
	node = symex_parse_symex ();
	if (node != NULL)
	{
		list = symex_list_create ();
		if (list == NULL)
			return NULL;
		
		list->fnode = node;
		return list;
	}
	
	return NULL;
}

symex_list_t*
symex_parse_from_string (char* s)
{
	symex_list_node_t *node;
	symex_list_t *list;
	
	if (s == NULL)
		return NULL;

	str = s;

	symex_get_char = symex_get_char_from_string;

	cur_pos = 0;
	
	node = symex_parse_symex ();
	if (node != NULL)
	{
		list = symex_list_create ();
		if (list == NULL)
			return NULL;
		list->fnode = node;
		return list;
	}
	return NULL;
}

/* char reading functions */
void
symex_unget_char (void)
{
	prev_char = cur_char;
}

int
symex_get_char_from_file (void)
{
	if (cur_char != 0)
	{
		if (prev_char == -1)
			cur_char = fgetc (fp);
		else
			prev_char = -1;
	
		if (cur_char == EOF)
			cur_char = 0;
	}
	return cur_char;
}

int
symex_get_char_from_string (void)
{
	if (cur_char != 0)
	{
		if (prev_char == -1)
		{
			cur_pos++;
			cur_char = str[cur_pos];
		}
		else
			prev_char = -1;
	}
	return cur_char;
}

void
symex_remove_blanks ()
{
	int ch;
	while ((ch = symex_get_char ()) != 0)
	{
		if (isspace(ch) || ch == '\n' || ch == '\t')
			continue;
		else
			break;
	}
	symex_unget_char ();
}

/* parsing functions */
symex_list_node_t*
symex_parse_symex (void)
{
	symex_list_node_t* node;

	symex_remove_blanks ();
	if ((node = symex_parse_string ()) == NULL)
		node = symex_parse_list ();
	return node;
}

symex_list_node_t*
symex_parse_list (void)
{
	int ch;
	symex_list_node_t *lnode, *node, *tnode;
	if ((ch = symex_get_char ()) == '(')
	{
		lnode = symex_list_node_create (SYMEX_LIST, NULL, -1);
		
		while ((node = symex_parse_symex ()) != NULL)
		{
			if (lnode->data == NULL)
				lnode->data = node;
			else
			{
				tnode = lnode->data;
				while (tnode->next != NULL) tnode = tnode->next;

				tnode->next = node;
			}
		}
		
		if ((ch = symex_get_char ()) == ')')
		{
			return lnode;
		}
		
		return NULL;
	} else 
		symex_unget_char ();
	return NULL;
}

symex_list_node_t*
symex_parse_string (void)
{
	return symex_parse_simple_string ();
}

symex_list_node_t*
symex_parse_simple_string (void)
{
	return symex_parse_bytes (-1);
}

symex_list_node_t*
symex_parse_bytes (int length)
{
	char *buf;
	int ch, index = 0;

	buf = malloc (MAX_BUF_SIZE);
	if (buf == NULL)
		return NULL;

	while ((length == -1 || index < length) && (ch = symex_get_char ()) != 0)
	{
		if (isalpha (ch) || isdigit (ch) || ch == '.')
		{
			buf[index] = ch;
			index++;
		}
		else
		{
			symex_unget_char ();
			break;
		}
	}
	
	buf[index] = 0;

	if (index > 0)
	{
		return symex_list_node_create (SYMEX_BYTES, buf, index);
	}
	
	free (buf);
	return NULL;
}

#ifdef __cplusplus
	} /* extern "C" */
#endif 