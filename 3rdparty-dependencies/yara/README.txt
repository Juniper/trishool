For yara-3.7.1, while building on windows we had to made quite a lot of
changes to the VS project solution:

1. Change it to build x86_32 solution from x86_64.
2. Delete the file cuckoo.c so that we didn't have the json(jansson)
   dependency.
3. Remove any references to jansson.lib and libcrypto.lib.
4. From the pre-processor directives remove the declarative for
   HAVE_LIBCRYPTO, since we removed the reference to libcrypto.lib in (3).
5. From the pre-processor directives remove the declarative for and
   CUCKOO_MODULE, since we removed the reference to jansson.lib in (3).
6. Now since you have removed the support for libcrypto and removed
   HAVE_LIBCRYPTO, which basically means we are asking yara-3.7.1 to not
   use openssl, we need to ask the code to use windows crypto libraries.
   To do so, define a new preprocessor directive, HAVE_WINCRYPT_H.
6. From inside crypto.h, change

   HCRYPTPROV yr_cryptprov;

   to

   extern HCRYPTPROV yr_cryptprov;

   and instead define the variable globally in libyara.c
7. From the header file defintion, remove the references to jansson and
   openssl header files.
