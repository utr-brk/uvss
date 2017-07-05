/*
 * string_convert.c
 *
 *  Created on: 23 May 2014
 *      Author: Burak
 */


#include "string_convert.h"
//#include <convert.h>
#include <malloc.h>
#include <stdio.h>
#include <memory.h>
//odroid
/*
const char *string_covert( const char *text, int textlen)
{

	static char * 	ret = NULL;


	if (NULL != ret)
		free(ret);
	ret = NULL;


	if (!text)
		return NULL;
	ret = (char*)calloc(textlen * 4 + 1, 1);//0.0.6 malloc'du calloc oldu


	if (0 != charset_convert("GB18030", "UTF-8", text, textlen, ret, textlen * 4 + 1))
	{
		memset(ret, 0, textlen * 4 + 1);
	}

	return ret;
}
*/

int  string_len(const char * text)
{
	int retval = 0;
	if (NULL != text){
		while (*text){
			text++;
			retval++;
		}
	}
	return retval;
}

