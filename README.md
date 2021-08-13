# Tink for PHP

**It's still experimental.**

This extension allows Tink.

Documentation for Tink can be found at [https://github.com/google/tink](https://github.com/google/tink).

## Build from sources

``` sh
$ git clone --depth=1 https://github.com/kjdev/php-ext-tink.git
$ phpize
$ ./configure --with-tink=[Tink C++ include/lib path]
$ make
$ make install
```

> Will need Tink's C ++ header files and libraries

## Configration

php.ini:

```
extension=tink.so
```

## Function

* tink\_encrypt — Tink encryption
* tink\_decrypt — Tink decryption

> implementation of https://github.com/google/tink/tree/master/examples/cc/helloworld
> command-line utility.

### tink\_encrypt — Tink encryption

#### Description

string **tink\_encrypt** ( string _$keyset_filename_, string _$data_, string _$associated_data_ )

Tink encryption.

#### Pameters

* _keyset_filename_

  name of the file with the keyset to be used for encryption.

* _data_

  a string to encryption.

* _associated_data_

  a string to be used as assciated data.

#### Return Values

Returns the encryption data or FALSE if an error occurred.


### tink\_decrypt — Tink decryption

#### Description

string **tink\_decrypt** ( string _$keyset_filename_, string _$data_, string _$associated_data_ )

Tink decryption.

#### Pameters
* _keyset_filename_

  name of the file with the keyset to be used for encryption.

* _data_

  a string to decryption.

* _associated_data_

  a string to be used as assciated data.

#### Return Values

Returns the decryption data or FALSE if an error occurred.


## Examples

```php
$keyset_file = 'keyset.json'; // See: tests/keyset.json
$data = 'message';
$associated_data = 'associated-data';

// encryption
$encrypt = tink_encrypt($keyset_file, $data, $associated_data);

// decryption
$var = tink_decrypt($keyset_file, $encrypt, $associated_data);
var_dump($var);
```
