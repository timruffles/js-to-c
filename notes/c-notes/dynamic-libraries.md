# Dynamic libs

Tell compiler you're outputting a dynamic library (dynamiclib is not in the man page :( )

    clang -dynamiclib dynlib.c -o ../out/dynlib.dylib

then compile your client program

    clang dynlib-client.c ../out/dynlib.dylib -o ../out/dynlib-client
    ../out/dynlib-client

if you make changes to the dynamic library, the compiled client program will see them as it's dynamically linked to it, rather than statically.
  


