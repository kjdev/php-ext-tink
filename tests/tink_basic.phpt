--TEST--
tink_encrypt/tink_decript(): Basic test
--SKIPIF--
<?php
if (!extension_loaded('tink')) {
  echo 'skip';
}
?>
--FILE--
<?php
$keyset_file = __DIR__ . '/keyset.json';

$src = 'testing tink primitives';
$associated_data = 'associated-data';

// encrypt
$encrypt = tink_encrypt($keyset_file, $src, $associated_data);

// decrypt
$dest = tink_decrypt($keyset_file, $encrypt, $associated_data);

echo $dest, PHP_EOL;
?>
--EXPECT--
testing tink primitives
