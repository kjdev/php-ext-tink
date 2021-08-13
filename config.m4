dnl config.m4 for extension tink

PHP_ARG_WITH([tink],
  [for tink support],
  [AS_HELP_STRING([--with-tink],
    [Include tink support])])

PHP_ARG_ENABLE([tink-debug],
  [whether to enable tink debug support],
  [AS_HELP_STRING([--enable-tink-debug],
    [Enable tink debug support])],
  [no],
  [no])

if test "$PHP_TINK" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, ,TINK_SHARED_LIBADD)

  dnl --with-tink -> check with-path
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/tink/config.h"
  if test -r $PHP_TINK/$SEARCH_FOR; then # path given as parameter
    TINK_DIR=$PHP_TINK
  else # search default path list
    AC_MSG_CHECKING([for tink files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        TINK_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$TINK_DIR"; then
    AC_MSG_ERROR([Please reinstall the tink distribution])
  fi

  dnl --with-tink -> add include path
  PHP_ADD_INCLUDE($TINK_DIR/include)

  dnl --with-tink -> check for lib
  LIBNAME=tink
  dnl LIBSYMBOL=TINK # you most likely want to change this
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TINK_DIR/$PHP_LIBDIR, TINK_SHARED_LIBADD)
  dnl ],[
  dnl   AC_MSG_ERROR([tink library not found])
  dnl ],[
  dnl   -L$TINK_DIR/$PHP_LIBDIR
  dnl ])
  AC_HAVE_LIBRARY($LIBNAME,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TINK_DIR/$PHP_LIBDIR, TINK_SHARED_LIBADD)
  ],[
    AC_MSG_ERROR([$LIBNAME library not found])
  ],[
    -L$TINK_DIR/$PHP_LIBDIR
  ])

  PHP_SUBST(TINK_SHARED_LIBADD)

  AC_DEFINE(HAVE_TINK, 1, [ Have tink support ])

  PHP_NEW_EXTENSION(tink, tink.cc, $ext_shared,, -std=c++11)

  if test "$PHP_TINK_DEBUG" != "no"; then
    AC_DEFINE(HAVE_TINK_DEBUG, 1, [ Have tink debug support ])
  fi
fi
