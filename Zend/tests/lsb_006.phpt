--TEST--
ZE2 Late Static Binding ensuring extending 'static' is not allowed
--FILE--
<?php

class Foo extends static {
}

?>
==DONE==
--EXPECTF--
Parse error: parse error, expecting `T_STRING' or `T_PAAMAYIM_NEKUDOTAYIM' or `T_NAMESPACE' in %s on line %d
