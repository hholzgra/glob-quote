/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <ss@2ns.de>                                 |
   +----------------------------------------------------------------------+
 */

/*
 * TODO:
 * - improve session id creation to avoid collisions
 *   (make use of mersenne twister, other data such as IP, browser etc.)
 * - improve files handler for better scaling
 * - add complete support for objects (partially implemented)
 * - complete ZTS support (currently only useable as non-ZTS)
 * - userland callback functions for ps_module
 */
#if !(WIN32|WINNT)
#include <sys/time.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"


#include "php_session.h"
#include "../standard/md5.h"


#ifdef ZTS
int ps_globals_id;
#else
static php_ps_globals ps_globals;
#endif

#include "modules.c"

function_entry session_functions[] = {
	PHP_FE(session_name, NULL)
	PHP_FE(session_module_name, NULL)
	PHP_FE(session_save_path, NULL)
	PHP_FE(session_id, NULL)
	PHP_FE(session_decode, NULL)
	PHP_FE(session_register, NULL)
	PHP_FE(session_unregister, NULL)
	PHP_FE(session_encode, NULL)
	PHP_FE(session_start, NULL)
	{0}
};

PHP_INI_BEGIN()
	PHP_INI_ENTRY("session_save_path", "/tmp", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session_name", "PHPSESSID", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session_module_name", "files", PHP_INI_ALL, NULL)
	PHP_INI_ENTRY("session_auto_start", "0", PHP_INI_ALL, NULL)
PHP_INI_END()

static int php_minit_session(INIT_FUNC_ARGS);
static int php_rinit_session(INIT_FUNC_ARGS);
static int php_mshutdown_session(SHUTDOWN_FUNC_ARGS);
static int php_rshutdown_session(SHUTDOWN_FUNC_ARGS);

zend_module_entry session_module_entry = {
	"session",
	session_functions,
	php_minit_session, php_mshutdown_session,
	php_rinit_session, php_rshutdown_session,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#define STR_CAT(P,S,I) {\
	pval *__p = (P);\
		ulong __i = __p->value.str.len;\
		__p->value.str.len += (I);\
		if (__p->value.str.val) {\
			__p->value.str.val = (char *)erealloc(__p->value.str.val, __p->value.str.len + 1);\
		} else {\
			__p->value.str.val = emalloc(__p->value.str.len + 1);\
				*__p->value.str.val = 0;\
		}\
	strcat(__p->value.str.val + __i, (S));\
}

#define MAX_STR 512
#define STD_FMT "%s|"
#define NOTFOUND_FMT "!%s|"

static char *_php_session_encode(int *newlen PSLS_DC)
{
	pval *buf;
	pval **struc;
	char *key;
	char *ret = NULL;
	char strbuf[MAX_STR + 1];
	ELS_FETCH();

	buf = ecalloc(sizeof(*buf), 1);
	buf->type = IS_STRING;
	buf->refcount++;

	for(zend_hash_internal_pointer_reset(&PS(vars));
			zend_hash_get_current_key(&PS(vars), &key, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&PS(vars))) {
		if(zend_hash_find(&EG(symbol_table), key, strlen(key) + 1, (void **) &struc) 
				== SUCCESS) {
			snprintf(strbuf, MAX_STR, STD_FMT, key);
			STR_CAT(buf, strbuf, strlen(strbuf));
			php3api_var_serialize(buf, struc);
		} else {
			snprintf(strbuf, MAX_STR, NOTFOUND_FMT, key);
			STR_CAT(buf, strbuf, strlen(strbuf));
		}
		efree(key);
	}

	if(newlen) *newlen = buf->value.str.len;
	ret = buf->value.str.val;
	efree(buf);

	return ret;
}

#define PS_ADD_VARL(name,namelen) \
	zend_hash_update(&PS(vars), name, namelen + 1, 0, 0, NULL)

#define PS_ADD_VAR(name) PS_ADD_VARL(name, strlen(name))

#define PS_DEL_VARL(name,namelen) \
	zend_hash_del(&PS(vars), name, namelen);

#define PS_DEL_VAR(name) PS_DEL_VARL(name, strlen(name))

	
static void _php_session_decode(char *val, int vallen PSLS_DC)
{
	char *p, *q;
	char *name;
	char *endptr = val + vallen;
	pval *current;
	int namelen;
	int has_value;
	ELS_FETCH();

	for(p = q = val; (p < endptr) && (q = strchr(p, '|')); p = q) {
		if(p[0] == '!') {
			p++;
			has_value = 0;
		} else
			has_value = 1;
		
		name = estrndup(p, q - p);
		namelen = q - p;
		q++;
		
		if(has_value) {
			current = (pval *) ecalloc(sizeof(pval), 1);

			if(php3api_var_unserialize(&current, &q, val + vallen)) {
				zend_hash_update(&EG(symbol_table), name, strlen(name) + 1,
						&current, sizeof(current), NULL);
				PS_ADD_VAR(name);
			} else {
				efree(current);
			}
		}
		PS_ADD_VAR(name);
		efree(name);
	}
}

static char *_php_create_id(int *newlen)
{
	PHP3_MD5_CTX context;
	unsigned char digest[16];
	char buf[256];
	struct timeval tv;
	int i;

	gettimeofday(&tv, NULL);
	PHP3_MD5Init(&context);
	
	sprintf(buf, "%ld%ld", tv.tv_sec, tv.tv_usec);
	PHP3_MD5Update(&context, buf, strlen(buf));

	PHP3_MD5Final(digest, &context);

	for(i = 0; i < 16; i++)
		sprintf(buf + (i << 1), "%02x", digest[i]);
	buf[i << 1] = '\0';
	
	if(newlen) *newlen = i << 1;
	return estrdup(buf);
}

static void _php_session_initialize(PSLS_DC)
{
	char *val;
	int vallen;
	
	if(PS(mod)->open(&PS(mod_data), PS(save_path), PS(session_name)) == FAILURE) {
		php3_error(E_ERROR, "failed to initialize session module");
		return;
	}
	if(PS(mod)->read(&PS(mod_data), PS(id), &val, &vallen) == SUCCESS) {
		_php_session_decode(val, vallen PSLS_CC);
		efree(val);
	}
}

static void _php_session_save_current_state(PSLS_D)
{
	char *val;
	int vallen;
	
	val = _php_session_encode(&vallen PSLS_CC);
	if(val) {
		PS(mod)->write(&PS(mod_data), PS(id), val, vallen);
		efree(val);
	} else {
		PS(mod)->write(&PS(mod_data), PS(id), "", 0);
	}
	PS(mod)->close(&PS(mod_data));
	PS(nr_open_sessions)--;
}

#define COOKIE_FMT "Set-cookie: %s=%s"

static void _php_session_send_cookie(PSLS_D)
{
	int len;
	char *cookie;

	len = strlen(PS(session_name)) + strlen(PS(id)) + sizeof(COOKIE_FMT);
	cookie = emalloc(len + 1);
	snprintf(cookie, len, COOKIE_FMT, PS(session_name), PS(id));

	sapi_add_header(cookie, len);
}

static ps_module *_php_find_ps_module(char *name PSLS_DC)
{
	ps_module *ret = NULL;
	ps_module **mod;

	for(mod = ps_modules; ((*mod && (*mod)->name) || !*mod); mod++) {
		if(*mod && !strcasecmp(name, (*mod)->name)) {
			ret = *mod;
			break;
		}
	}
	
	return ret;
}

static void _php_session_start(PSLS_D)
{
	pval **ppid;
	int send_cookie = 1;
	char *session_data;
	int datalen;
	ELS_FETCH();

	if(!PS(id) &&
		zend_hash_find(&EG(symbol_table), PS(session_name), 
				strlen(PS(session_name)) + 1, (void **) &ppid) == SUCCESS) {
		convert_to_string((*ppid));
		PS(id) = estrndup((*ppid)->value.str.val, (*ppid)->value.str.len);
		send_cookie = 0;
	}
	
	if(!PS(id)) {
		PS(id) = _php_create_id(NULL);
	}

	if(send_cookie) {
		_php_session_send_cookie(PSLS_C);
	}
	PS(nr_open_sessions)++;

	_php_session_initialize(PSLS_C);
}

/* {{{ proto string session_name([string newname])
   return the current session name. if newname is given, the session name is replaced with newname */
PHP_FUNCTION(session_name)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(session_name));

	if(ac < 0 || ac > 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string(p_name);
		efree(PS(session_name));
		PS(session_name) = estrndup(p_name->value.str.val, p_name->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   return the current module name used for accessing session data. if newname is given, the module name is replaced with newname */
PHP_FUNCTION(session_module_name)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(mod)->name);

	if(ac < 0 || ac > 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		ps_module *tempmod;

		convert_to_string(p_name);
		tempmod = _php_find_ps_module(p_name->value.str.val);
		if(tempmod) {
			if(PS(mod_data))
				PS(mod)->close(&PS(mod_data));
			PS(mod_data) = tempmod;
		} else {
			efree(old);
			php3_error(E_ERROR, "Cannot find named PHP session module (%s)",
					p_name->value.str.val);
			RETURN_FALSE;
		}
	}

	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   return the current save path passed to module_name. if newname is given, the save path is replaced with newname */
PHP_FUNCTION(session_save_path)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	char *old;
	PSLS_FETCH();

	old = estrdup(PS(save_path));

	if(ac < 0 || ac > 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string(p_name);
		efree(PS(save_path));
		PS(save_path) = estrndup(p_name->value.str.val, p_name->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto string session_id([string newid])
   return the current session id. if newid is given, the session id is replaced with newid */
PHP_FUNCTION(session_id)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	char *old = empty_string;
	PSLS_FETCH();

	if(PS(id))
		old = estrdup(PS(id));

	if(ac < 0 || ac > 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1) {
		convert_to_string(p_name);
		if(PS(id)) efree(PS(id));
		PS(id) = estrndup(p_name->value.str.val, p_name->value.str.len);
	}
	
	RETVAL_STRING(old, 0);
}
/* }}} */

/* {{{ proto session_register(string varname)
   adds varname to the list of variables which are freezed at the session end */
PHP_FUNCTION(session_register)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	PSLS_FETCH();

	if(ac != 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(p_name);
	
	if(!PS(nr_open_sessions)) _php_session_start(PSLS_C);
	PS_ADD_VAR(p_name->value.str.val);
}
/* }}} */

/* {{{ proto session_unregister(string varname)
   removes varname from the list of variables which are freezed at the session end */
PHP_FUNCTION(session_unregister)
{
	pval *p_name;
	int ac = ARG_COUNT(ht);
	PSLS_FETCH();

	if(ac != 1 || getParameters(ht, ac, &p_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(p_name);
	
	PS_DEL_VAR(p_name->value.str.val);
}
/* }}} */

/* {{{ proto string session_encode()
   serializes the current setup and returns the serialized representation */
PHP_FUNCTION(session_encode)
{
	int len;
	char *enc;

	enc = _php_session_encode(&len PSLS_CC);
	RETVAL_STRINGL(enc, len, 0);
}
/* }}} */

/* {{{ proto session_decode(string data)
   deserializes data and reinitializes the variables */
PHP_FUNCTION(session_decode)
{
	pval *str;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	_php_session_decode(str->value.str.val, str->value.str.len PSLS_CC);
}
/* }}} */

/* {{{ proto session_start()
   Begin session - reinitializes freezed variables, registers browsers etc */
PHP_FUNCTION(session_start)
{
	PSLS_FETCH();

	_php_session_start(PSLS_C);
}
/* }}} */

void php_rinit_globals(PSLS_D)
{
	PS(mod) = _php_find_ps_module(INI_STR("session_module_name"));
		
	zend_hash_init(&PS(vars), 0, NULL, NULL, 0);
	PS(save_path) = estrdup(INI_STR("session_save_path"));
	PS(session_name) = estrdup(INI_STR("session_name"));
	PS(id) = NULL;
	PS(nr_open_sessions) = 0;
	PS(mod_data) = NULL;
}

void php_rshutdown_globals(PSLS_D)
{
	if(PS(mod_data))
		PS(mod)->close(&PS(mod_data));
	efree(PS(save_path));
	efree(PS(session_name));
	if(PS(id)) efree(PS(id));
	zend_hash_destroy(&PS(vars));
}

int php_rinit_session(INIT_FUNC_ARGS)
{
	PSLS_FETCH();

	php_rinit_globals(PSLS_C);

	if(INI_INT("session_auto_start")) {
		_php_session_start();
	}
	if(PS(mod) == NULL)
		return FAILURE;

	return SUCCESS;
}

int php_rshutdown_session(SHUTDOWN_FUNC_ARGS)
{
	PSLS_FETCH();

	if(PS(nr_open_sessions) > 0) {
		_php_session_save_current_state();
	}
	php_rshutdown_globals(PSLS_C);
	return SUCCESS;
}

int php_minit_session(INIT_FUNC_ARGS)
{
#ifdef ZTS
	ps_globals_id = ts_allocate_id(sizeof(php_ps_globals), NULL, NULL);
#endif
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}

int php_mshutdown_session(SHUTDOWN_FUNC_ARGS)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
