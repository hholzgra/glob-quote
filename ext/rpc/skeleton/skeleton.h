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
   | Author: Harald Radi <h.radi@nme.at>                                  |
   +----------------------------------------------------------------------+
 */

#ifndef SKELETON_H
#define SKELETON_H

#include "../handler.h"
#include "../php_rpc.h"

RPC_DECLARE_HANDLER(skeleton);

ZEND_MINIT_FUNCTION(skeleton);
ZEND_MSHUTDOWN_FUNCTION(skeleton);
ZEND_MINFO_FUNCTION(skeleton);

/* TODO: define your functions here */
ZEND_FUNCTION(skeleton_function);
/**/

#endif /* SKELETON_H */