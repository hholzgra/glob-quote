--TEST--
Test function get_cfg_var() by calling deprecated option
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--INI--
magic_quotes_gpc=1
--SKIPIF--
<?php if (version_compare(PHP_VERSION, "5.3", "<")) die("skip requires 5.3 or greater"); ?>
--FILE--
<?php
echo "*** Test by calling method or function with deprecated option ***\n";
var_dump(get_cfg_var( 'magic_quotes_gpc' ) );

?>
--EXPECTF--
Warning: Directive 'magic_quotes_gpc' is deprecated in PHP 5.3 and greater in %s on line 0
*** Test by calling method or function with deprecated option ***
string(1) "1"

