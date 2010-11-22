#!/bin/sh

INFILE="../../Zend/zend_language_parser.h"
OUTFILE="tokenizer_data.c"
AWK=awk

####################################################################

if test ! -f "./tokenizer.c"; then
    echo "Please run this script from within php-src/ext/tokenizer"
    exit 0
fi


echo '/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Johannes Schlueter <johannes@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* $Id: tokenizer_data_gen.sh,v 1.1.2.2.2.1 2008-02-12 00:45:14 stas Exp $ */

/*
   DO NOT EDIT THIS FILE!
   This file is generated using tokenizer_data_gen.sh
*/ 

#include "php.h"
#include "zend.h"
#include <zend_language_parser.h>

' > $OUTFILE


echo 'void tokenizer_register_constants(INIT_FUNC_ARGS) {' >> $OUTFILE
$AWK '/^#define T_/  { print "	REGISTER_LONG_CONSTANT(\"" $2 "\", " $2 ", CONST_CS | CONST_PERSISTENT);" }' < $INFILE >> $OUTFILE
echo '	REGISTER_LONG_CONSTANT("T_DOUBLE_COLON", T_PAAMAYIM_NEKUDOTAYIM, CONST_CS | CONST_PERSISTENT);' >> $OUTFILE
echo '}' >> $OUTFILE


echo '
char *get_token_type_name(int token_type)
{
	switch (token_type) {
' >> $OUTFILE

$AWK '
	/^#define T_PAAMAYIM_NEKUDOTAYIM/ {
		print "		case T_PAAMAYIM_NEKUDOTAYIM: return \"T_DOUBLE_COLON\";"
		next
	}
	/^#define T_/ {
		print "		case " $2 ": return \"" $2 "\";"
	}
' < $INFILE >> $OUTFILE

echo '
	}
	return "UNKNOWN";
}
' >> $OUTFILE

echo "Wrote $OUTFILE"
