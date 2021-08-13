/* tink extension for PHP */

#ifndef PHP_TINK_H
# define PHP_TINK_H

extern zend_module_entry tink_module_entry;
# define phpext_tink_ptr &tink_module_entry

# define PHP_TINK_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_TINK)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif /* PHP_TINK_H */
