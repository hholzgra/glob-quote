/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sterling Hughes <sterling@php.net>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_EXPAT_COMPAT_H
#define PHP_EXPAT_COMPAT_H

#if !defined(HAVE_LIBEXPAT) && defined(HAVE_LIBXML)
#define LIBXML_EXPAT_COMPAT 1

#include <libxml.h>
#include <libxml/hash.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/tree.h>

typedef xmlChar XML_Char;

typedef void (*XML_StartElementHandler)(void *, const XML_Char *, const XML_Char **);
typedef void (*XML_EndElementHandler)(void *, const XML_Char *);
typedef void (*XML_CharacterDataHandler)(void *, const XML_Char *, int);
typedef void (*XML_ProcessingInstructionHandler)(void *, const XML_Char *, const XML_Char *);
typedef void (*XML_DefaultHandler)(void *, const XML_Char *, int);
typedef void (*XML_UnparsedEntityDeclHandler)(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
typedef void (*XML_NotationDeclHandler)(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
typedef int  (*XML_ExternalEntityRefHandler)(void *, const XML_Char *, const XML_Char *, const XML_Char *, const XML_Char *);
typedef void (*XML_StartNamespaceDeclHandler)(void *, const XML_Char *, const XML_Char *);
typedef void (*XML_EndNamespaceDeclHandler)(void *, const XML_Char *);

typedef struct _XML_Memory_Handling_Suite {
  void *(*malloc_fcn)(size_t size);
  void *(*realloc_fcn)(void *ptr, size_t size);
  void (*free_fcn)(void *ptr);
} XML_Memory_Handling_Suite;

typedef struct _XML_Parser {
	int namespace;

	xmlHashTablePtr _ns_map;
	xmlHashTablePtr _reverse_ns_map;
	
	void *user;
	xmlParserCtxtPtr parser;

	XML_StartElementHandler          h_start_element;
	XML_EndElementHandler            h_end_element;
	XML_CharacterDataHandler         h_cdata;
	XML_ProcessingInstructionHandler h_pi;
	XML_DefaultHandler               h_default;
	XML_UnparsedEntityDeclHandler    h_unparsed_entity_decl;
	XML_NotationDeclHandler          h_notation_decl;
	XML_ExternalEntityRefHandler     h_external_entity_ref;
	XML_StartNamespaceDeclHandler    h_start_ns;
	XML_EndNamespaceDeclHandler      h_end_ns;
	XML_Memory_Handling_Suite        mem_hdlrs;
} *XML_Parser;

XML_Parser XML_ParserCreate(const XML_Char *);
XML_Parser XML_ParserCreateNS(const XML_Char *, const XML_Char);
XML_Parser XML_ParserCreate_MM(const XML_Char *, const XML_Memory_Handling_Suite *, const XML_Char *);
void XML_SetUserData(XML_Parser, void *);
void *XML_GetUserData(XML_Parser);
void XML_SetElementHandler(XML_Parser, XML_StartElementHandler, XML_EndElementHandler);
void XML_SetCharacterDataHandler(XML_Parser, XML_CharacterDataHandler);
void XML_SetProcessingInstructionHandler(XML_Parser, XML_ProcessingInstructionHandler);
void XML_SetDefaultHandler(XML_Parser, XML_DefaultHandler);
void XML_SetUnparsedEntityDeclHandler(XML_Parser, XML_UnparsedEntityDeclHandler);
void XML_SetNotationDeclHandler(XML_Parser, XML_NotationDeclHandler);
void XML_SetExternalEntityRefHandler(XML_Parser, XML_ExternalEntityRefHandler);
void XML_SetStartNamespaceDeclHandler(XML_Parser, XML_StartNamespaceDeclHandler);
void XML_SetEndNamespaceDeclHandler(XML_Parser, XML_EndNamespaceDeclHandler);
int  XML_Parse(XML_Parser, const XML_Char *, int data_len, int is_final);
int  XML_GetErrorCode(XML_Parser);
const XML_Char *XML_ErrorString(int);
int  XML_GetCurrentLineNumber(XML_Parser);
int  XML_GetCurrentColumnNumber(XML_Parser);
int  XML_GetCurrentByteIndex(XML_Parser);
const XML_Char *XML_ExpatVersion(void);
void XML_ParserFree(XML_Parser);

#define XML_ERROR_NONE 0
#define XML_ERROR_NO_MEMORY 0
#define XML_ERROR_SYNTAX 0
#define XML_ERROR_NO_ELEMENTS 0
#define XML_ERROR_INVALID_TOKEN 0
#define XML_ERROR_UNCLOSED_TOKEN 0
#define XML_ERROR_PARTIAL_CHAR 0
#define XML_ERROR_TAG_MISMATCH 0
#define XML_ERROR_DUPLICATE_ATTRIBUTE 0
#define XML_ERROR_JUNK_AFTER_DOC_ELEMENT 0
#define XML_ERROR_PARAM_ENTITY_REF 0
#define XML_ERROR_UNDEFINED_ENTITY 0
#define XML_ERROR_RECURSIVE_ENTITY_REF 0
#define XML_ERROR_ASYNC_ENTITY 0
#define XML_ERROR_BAD_CHAR_REF 0
#define XML_ERROR_BINARY_ENTITY_REF 0
#define XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF 0
#define XML_ERROR_MISPLACED_XML_PI 0
#define XML_ERROR_UNKNOWN_ENCODING 0
#define XML_ERROR_INCORRECT_ENCODING 0
#define XML_ERROR_UNCLOSED_CDATA_SECTION 0
#define XML_ERROR_EXTERNAL_ENTITY_HANDLING 0

#else 
#include <expat.h>
#endif /* HAVE_LIBEXPAT */

#endif /* PHP_EXPAT_COMPAT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
