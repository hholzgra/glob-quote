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
  | Author: Sterling Hughes <sterling@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_simplexml.h"


zend_class_entry *sxe_class_entry;

static php_sxe_object *php_sxe_object_new(TSRMLS_D);
static zend_object_value php_sxe_register_object(php_sxe_object * TSRMLS_DC);

/* {{{ php_sxe_fetch_object()
 */
static inline php_sxe_object *
php_sxe_fetch_object(zval *object TSRMLS_DC)
{
	return (php_sxe_object *) zend_object_store_get_object(object TSRMLS_CC);
}
/* }}} */

/* {{{ _node_as_zval()
 */
static void
_node_as_zval(php_sxe_object *sxe, xmlNodePtr node, zval *value)
{
	char *contents;

	contents = xmlNodeListGetString(sxe->document, node->xmlChildrenNode, 1);
	if (contents) {
		ZVAL_STRING(value, contents, 1);
		xmlFree(contents);
	} else {
		php_sxe_object *subnode;

		subnode = php_sxe_object_new(TSRMLS_C);
		subnode->document = sxe->document;
		subnode->node = node;

		value->type = IS_OBJECT;
		value->value.obj = php_sxe_register_object(subnode TSRMLS_CC);
	}
}
/* }}} */

/* {{{ sxe_property_read()
 */
static zval *
sxe_property_read(zval *object, zval *member TSRMLS_DC)
{
	zval           *return_value;
	zval           *value;
	php_sxe_object *sxe;
	char           *name;
	xmlNodePtr      node;
	int             counter = 0;

	MAKE_STD_ZVAL(return_value);
	ZVAL_NULL(return_value);
	
	name = Z_STRVAL_P(member);

	sxe = php_sxe_fetch_object(object TSRMLS_CC);

	if (sxe->node == NULL) {
		sxe->node = node = xmlDocGetRootElement(sxe->document)->xmlChildrenNode;
	} else {
		node = sxe->node->xmlChildrenNode;
	}

	while (node) {
		if (!xmlStrcmp(node->name, name)) {
			if (counter == 1) {
				array_init(return_value);
				add_next_index_zval(return_value, value);
			}

			MAKE_STD_ZVAL(value);
			_node_as_zval(sxe, node, value);
			
			if (++counter > 1) {
				add_next_index_zval(return_value, value);
			}
		}

		node = node->next;
	}

	if (counter == 1) {
		return_value = value;
	}

			
	return return_value;
}
/* }}} */

/* {{{ sxe_property_write()
 */
static void
sxe_property_write(zval *object, zval *member, zval *value TSRMLS_DC)
{
}
/* }}} */

/* {{{ sxe_property_exists()
 */
static int
sxe_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	
}
/* }}} */

/* {{{ sxe_property_delete()
 */
static void
sxe_property_delete(zval *object, zval *member TSRMLS_DC)
{
}
/* }}} */

/* {{{ sxe_objects_compare()
 */
static int
sxe_objects_compare(zval *object1, zval *object2 TSRMLS_DC)
{
}
/* }}} */

/* {{{ sxe_constructor_get()
 */
static union _zend_function *
sxe_constructor_get(zval *object TSRMLS_DC)
{
	return NULL;
}
/* }}} */

/* {{{ sxe_method_get()
 */
static union _zend_function *
sxe_method_get(zval *object, char *name, int len TSRMLS_DC)
{
	zend_internal_function *f;

	f = emalloc(sizeof(zend_internal_function));
	f->type = ZEND_OVERLOADED_FUNCTION;
	f->arg_types = NULL;
	f->scope = sxe_class_entry;
	f->fn_flags = 0;
	f->function_name = estrndup(name, len);

	return (union _zend_function *) f;
}
/* }}} */

/* {{{ sxe_call_method()
 */
static int
sxe_call_method(char *method, INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_NULL();
	return 1;
}
/* }}} */

/* {{{ sxe_class_entry_get()
 */
static zend_class_entry *
sxe_class_entry_get(zval *object TSRMLS_DC)
{
	return sxe_class_entry;
}
/* }}} */


static zend_object_handlers sxe_object_handlers[] = {
	ZEND_OBJECTS_STORE_HANDLERS,
	sxe_property_read,
	sxe_property_write,
	NULL,
	NULL,
	NULL,
	NULL,
	sxe_property_exists,
	sxe_property_delete,
	NULL,
	sxe_method_get,
	sxe_call_method,
	sxe_constructor_get,
	sxe_class_entry_get,
	NULL,
	sxe_objects_compare
};

/* {{{ sxe_object_clone()
 */
static void
sxe_object_clone(void *object, void **clone TSRMLS_DC)
{
	
}
/* }}} */

/* {{{ sxe_object_dtor()
 */
static void
sxe_object_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	zend_objects_destroy_object(object, handle TSRMLS_CC);
}
/* }}} */

/* {{{ php_sxe_object_new() 
 */
static php_sxe_object *
php_sxe_object_new(TSRMLS_D)
{
	php_sxe_object *intern;

	intern = emalloc(sizeof(php_sxe_object));
	intern->zo.ce = sxe_class_entry;
	intern->zo.in_get = 0;
	intern->zo.in_set = 0;

	ALLOC_HASHTABLE(intern->zo.properties);
	zend_hash_init(intern->zo.properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	return intern;
}
/* }}} */

/* {{{ php_sxe_register_object
 */
static zend_object_value
php_sxe_register_object(php_sxe_object *intern TSRMLS_DC)
{
	zend_object_value rv;

	rv.handle = zend_objects_store_put(intern, sxe_object_dtor, sxe_object_clone);
	rv.handlers = (zend_object_handlers *) &sxe_object_handlers;

	return rv;
}
/* }}} */

/* {{{ sxe_object_new()
 */
static zend_object_value
sxe_object_new(zend_class_entry *ce TSRMLS_DC)
{
	php_sxe_object    *intern;
	zend_object_value  rv;

	intern = php_sxe_object_new(TSRMLS_C);
	return php_sxe_register_object(intern TSRMLS_CC);
}
/* }}} */

/* {{{ proto simplemxml_element simplexml_load_file(string filename)
   Load a filename and return a simplexml_element object to allow for processing */
PHP_FUNCTION(simplexml_load_file)
{
	php_sxe_object *sxe;
	char           *filename;
	int             filename_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	sxe = php_sxe_object_new(TSRMLS_C);
	sxe->document = xmlParseFile(filename);
	if (sxe->document == NULL) {
		RETURN_FALSE;
	}

	return_value->type = IS_OBJECT;
	return_value->value.obj = php_sxe_register_object(sxe TSRMLS_CC);
}
/* }}} */


function_entry simplexml_functions[] = {
	PHP_FE(simplexml_load_file, NULL)
	{NULL, NULL, NULL}
};


zend_module_entry simplexml_module_entry = {
	STANDARD_MODULE_HEADER,
	"simplexml",
	simplexml_functions,
	PHP_MINIT(simplexml),
	PHP_MSHUTDOWN(simplexml),
	PHP_RINIT(simplexml),	
	PHP_RSHUTDOWN(simplexml),
	PHP_MINFO(simplexml),
	"0.1",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SIMPLEXML
ZEND_GET_MODULE(simplexml)
#endif

/* {{{ PHP_MINIT_FUNCTION(simplexml)
 */
PHP_MINIT_FUNCTION(simplexml)
{
	zend_class_entry sxe;
	zend_internal_function sxe_constructor;

	INIT_CLASS_ENTRY(sxe, "simplexml_element", NULL);
	sxe.create_object = sxe_object_new;
	sxe_class_entry = zend_register_internal_class(&sxe TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION(simplexml)
 */
PHP_MSHUTDOWN_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION(simplexml)
 */
PHP_RINIT_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION(simplexml)
 */
PHP_RSHUTDOWN_FUNCTION(simplexml)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION(simplexml)
 */
PHP_MINFO_FUNCTION(simplexml)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Simplexml support", "enabled");
	php_info_print_table_end();

}
/* }}} */

/**
 * Local Variables:
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: t
 * End:
 * vim600: fdm=marker
 * vim: noet sw=4 ts=4
 */
