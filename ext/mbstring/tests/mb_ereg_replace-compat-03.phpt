--TEST--
mb_ereg_replace() compat test 3
--SKIPIF--
extension_loaded('mbstring') or die('skip');
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/003.phpt) */
  $a="\\'test";
  echo mb_ereg_replace("\\\\'","'",$a);
?>
--EXPECT--
'test
