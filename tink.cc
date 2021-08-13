/* tink extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/base64.h"
#include "php_tink.h"

// tink
#include <fstream>
#include "tink/cleartext_keyset_handle.h"
#include "tink/config/tink_config.h"
#include "tink/json_keyset_reader.h"

using crypto::tink::Aead;
using crypto::tink::CleartextKeysetHandle;
using crypto::tink::JsonKeysetReader;
using crypto::tink::KeysetHandle;
using crypto::tink::KeysetReader;
using crypto::tink::TinkConfig;

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
  ZEND_PARSE_PARAMETERS_START(0, 0)  \
  ZEND_PARSE_PARAMETERS_END()
#endif

#if HAVE_TINK_DEBUG
#define php_tink_debug(...) php_printf(__VA_ARGS__)
#else
#define php_tink_debug(...)
#endif

static bool
php_tink_init()
{
  php_tink_debug("Initializing Tink...\n");

  auto status = TinkConfig::Register();
  if (!status.ok()) {
    zend_error(E_WARNING, "Initialization of Tink failed: %s",
               status.error_message().c_str());
  }

  return status.ok();
}

static std::unique_ptr<KeysetReader> php_tink_get_json_keyset_reader(const char *filename)
{
  php_tink_debug("Creating a JsonKeysetReader...\n");

  std::unique_ptr<std::ifstream> keyset_stream(new std::ifstream());
  keyset_stream->open(filename, std::ifstream::in);

  auto keyset_reader_result = JsonKeysetReader::New(std::move(keyset_stream));
  if (!keyset_reader_result.ok()) {
    zend_error(E_WARNING, "Creation of the reader failed: %s",
               keyset_reader_result.status().error_message().c_str());
    return NULL;
  }

  return std::move(keyset_reader_result.ValueOrDie());
}

static std::unique_ptr<KeysetHandle> php_tink_read_keyset(const char *filename)
{
  auto keyset_reader = php_tink_get_json_keyset_reader(filename);
  if (!keyset_reader) {
    return NULL;
  }

  auto keyset_handle_result =
    CleartextKeysetHandle::Read(std::move(keyset_reader));
  if (!keyset_handle_result.ok()) {
    zend_error(E_WARNING, "Reading the keyset failed: %s",
               keyset_handle_result.status().error_message().c_str());
    return NULL;
  }

  return std::move(keyset_handle_result.ValueOrDie());
}

PHP_FUNCTION(tink_encrypt)
{
  char *keyset_filename, *data, *associated_data;
  size_t keyset_filename_len, data_len, associated_data_len;

  ZEND_PARSE_PARAMETERS_START(3, 3)
    Z_PARAM_STRING(keyset_filename, keyset_filename_len)
    Z_PARAM_STRING(data, data_len)
    Z_PARAM_STRING(associated_data, associated_data_len)
  ZEND_PARSE_PARAMETERS_END();

  // Init Tink.
  if (!php_tink_init()) {
    RETURN_FALSE;
  }

  // Read the keyset.
  auto keyset_handle = php_tink_read_keyset(keyset_filename);
  if (!keyset_handle) {
    RETURN_FALSE;
  }

  // Get the primitive.
  auto primitive_result = keyset_handle->GetPrimitive<Aead>();
  if (!primitive_result.ok()) {
    zend_error(E_WARNING, "getting AEAD-primitive from the factory failed: %s",
               primitive_result.status().error_message().c_str());
    RETURN_FALSE;
  }
  auto aead = std::move(primitive_result.ValueOrDie());

  // Encryption.
  auto encrypt_result = aead->Encrypt(data, associated_data);
  if (!encrypt_result.ok()) {
    zend_error(E_WARNING, "while encypting the input: %s",
               encrypt_result.status().error_message().c_str());
    RETURN_FALSE;
  }

  std::string output = encrypt_result.ValueOrDie();

  RETVAL_STRINGL(output.data(), output.size());
}

PHP_FUNCTION(tink_decrypt)
{
  char *keyset_filename, *data, *associated_data;
  size_t keyset_filename_len, data_len, associated_data_len;

  ZEND_PARSE_PARAMETERS_START(3, 3)
    Z_PARAM_STRING(keyset_filename, keyset_filename_len)
    Z_PARAM_STRING(data, data_len)
    Z_PARAM_STRING(associated_data, associated_data_len)
  ZEND_PARSE_PARAMETERS_END();

  // Init Tink.
  if (!php_tink_init()) {
    RETURN_FALSE;
  }

  // Read the keyset.
  auto keyset_handle = php_tink_read_keyset(keyset_filename);
  if (!keyset_handle) {
    RETURN_FALSE;
  }

  // Get the primitive.
  auto primitive_result = keyset_handle->GetPrimitive<Aead>();
  if (!primitive_result.ok()) {
    zend_error(E_WARNING, "getting AEAD-primitive from the factory failed: %s",
               primitive_result.status().error_message().c_str());
    RETURN_FALSE;
  }
  auto aead = std::move(primitive_result.ValueOrDie());

  // Decryption.
  auto decrypt_result = aead->Decrypt(data, associated_data);
  if (!decrypt_result.ok()) {
    zend_error(E_WARNING, "while decrypting the input: %s",
               decrypt_result.status().error_message().c_str());
    RETURN_FALSE;
  }

  std::string output = decrypt_result.ValueOrDie();

  RETVAL_STRINGL(output.data(), output.size());
}

PHP_RINIT_FUNCTION(tink)
{
#if defined(ZTS) && defined(COMPILE_DL_TINK)
  ZEND_TSRMLS_CACHE_UPDATE();
#endif

  return SUCCESS;
}

PHP_MINFO_FUNCTION(tink)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "tink support", "enabled");
  php_info_print_table_end();
}

ZEND_BEGIN_ARG_INFO(arginfo_tink_encrypt, 0)
  ZEND_ARG_INFO(0, keyset_filename)
  ZEND_ARG_INFO(0, data)
  ZEND_ARG_INFO(0, associated_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_tink_decrypt, 0)
  ZEND_ARG_INFO(0, keyset_filename)
  ZEND_ARG_INFO(0, data)
  ZEND_ARG_INFO(0, associated_data)
ZEND_END_ARG_INFO()

static const zend_function_entry tink_functions[] = {
  PHP_FE(tink_encrypt, arginfo_tink_encrypt)
  PHP_FE(tink_decrypt, arginfo_tink_decrypt)
  PHP_FE_END
};

zend_module_entry tink_module_entry = {
  STANDARD_MODULE_HEADER,
  "tink",            /* Extension name */
  tink_functions,    /* zend_function_entry */
  NULL,              /* PHP_MINIT - Module initialization */
  NULL,              /* PHP_MSHUTDOWN - Module shutdown */
  PHP_RINIT(tink),   /* PHP_RINIT - Request initialization */
  NULL,              /* PHP_RSHUTDOWN - Request shutdown */
  PHP_MINFO(tink),   /* PHP_MINFO - Module info */
  PHP_TINK_VERSION,  /* Version */
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TINK
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(tink)
#endif
