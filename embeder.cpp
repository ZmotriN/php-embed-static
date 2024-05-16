/*
Copyright (c) 2024 Maxime Larrivée-Roy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef ZEND_WIN32
#define ZEND_WIN32
#endif
#ifndef PHP_WIN32
#define PHP_WIN32
#endif
#ifndef ZTS
#define ZTS 1
#endif
#ifndef ZEND_DEBUG
#define ZEND_DEBUG 0
#endif


#include "php.h"
#include "php_embed.h"
#include "ext/standard/php_standard.h"
#include "zend_smart_str.h"


static DWORD embed_get_module_path(char* path);
static void embed_register_constant_string(char* name, char* val);
static void embed_register_constant_bool(char* name, BOOL val);


static void embeded_ini_defaults(HashTable *configuration_hash)
{
	zval ini_value;

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("Embed SAPI error:"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("error_prepend_string"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("On"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("display_errors"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("On"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("display_startup_errors"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("Off"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("log_errors"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("32767"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("error_reporting"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("1"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("winbinder.low_level_functions"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("1"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("com.allow_dcom"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("1"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("com.autoregister_typelib"), &ini_value);

	ZVAL_NEW_STR(&ini_value, zend_string_init(ZEND_STRL("UTF-8"), 1));
	zend_hash_str_update(configuration_hash, ZEND_STRL("iconv.internal_encoding"), &ini_value);
}


long main(int argc, char** argv) {
	int i;
	char path[MAXPATHLEN];
	char dir[MAXPATHLEN];
	HANDLE chnd;
	zval ret_value;
	long exit_status;
	char *eval_string = "include 'res:///PHP/RUN';";

	php_embed_module.ini_defaults = embeded_ini_defaults;

	PHP_EMBED_START_BLOCK(argc, argv);

	embed_get_module_path(path);
	embed_register_constant_string("SELF", path);
	
	strcpy(dir, path);
	for (i = strlen(dir) - 1; i >= 0 && dir[i] != 92; i--) dir[i] = 0;
	embed_register_constant_string("DIR", dir);

	chnd = GetStdHandle(STD_OUTPUT_HANDLE);
	embed_register_constant_bool("CONSOLE", (chnd != NULL && chnd != INVALID_HANDLE_VALUE));

	embed_register_constant_string("S", "\\");
	embed_register_constant_string("R", "\r");
	embed_register_constant_string("N", "\n");
	embed_register_constant_string("RN", "\r\n");

	zend_first_try{
		PG(during_request_startup) = 0;

		if (zend_eval_string(eval_string, &ret_value, "main") != SUCCESS) {
			php_printf("Failed to eval.\n");
		} 

		exit_status = Z_LVAL(ret_value);
	} zend_catch{
		exit_status = EG(exit_status);
	}
	zend_end_try();

	PHP_EMBED_END_BLOCK();

	return exit_status;
}


static DWORD embed_get_module_path(char* path)
{
	DWORD pathsize;
	pathsize = GetModuleFileNameA(NULL, path, MAXPATHLEN);
	path[pathsize] = 0;
	return pathsize;
}


static void embed_register_constant_string(char* name, char* val)
{
	zend_constant et;
	zval _zval;

	et.name = zend_string_init_interned(name, strlen(name), 0);
	ZVAL_STRING(&_zval, val);
	ZVAL_COPY(&et.value, &_zval);
	Z_CONSTANT_FLAGS(et.value) = 0;
	zend_register_constant(&et);
}


static void embed_register_constant_bool(char* name, BOOL val)
{
	zend_constant et;
	zval _zval;

	et.name = zend_string_init_interned(name, strlen(name), 0);
	ZVAL_BOOL(&_zval, val);
	ZVAL_COPY(&et.value, &_zval);
	Z_CONSTANT_FLAGS(et.value) = 0;
	zend_register_constant(&et);
}
