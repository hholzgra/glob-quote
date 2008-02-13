--TEST--
Test array_push() function : usage variations - Pass different data types as $stack arg
--FILE--
<?php
/* Prototype  : int array_push(array $stack, mixed $var [, mixed $...])
 * Description: Pushes elements onto the end of the array 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $stack argument to array_push() to test behaviour
 */

echo "*** Testing array_push() : usage variations ***\n";

// Initialise function arguments not being substituted
$var = 'value';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $stack argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*16*/ "",
       '',
       array(),

       // string data
/*19*/ "string",
       'string',
       $heredoc,
       
       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of array_push()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( array_push($input, $var) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_push() : usage variations ***

-- Iteration 1 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 2 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 3 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 4 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 5 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 6 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 7 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 8 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 9 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 10 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 11 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 12 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 13 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 14 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 15 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 16 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 17 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 18 --
int(1)

-- Iteration 19 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 20 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 21 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 22 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 23 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 24 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 25 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)
Done
--UEXPECTF--
*** Testing array_push() : usage variations ***

-- Iteration 1 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 2 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 3 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 4 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 5 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 6 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 7 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 8 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 9 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 10 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 11 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 12 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 13 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 14 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 15 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 16 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 17 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 18 --
int(1)

-- Iteration 19 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 20 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 21 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 22 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 23 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 24 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)

-- Iteration 25 --

Warning: array_push(): First argument should be an array in %s on line %d
bool(false)
Done