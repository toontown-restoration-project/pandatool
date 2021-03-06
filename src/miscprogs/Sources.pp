#define LOCAL_LIBS \
  p3progbase

#define OTHER_LIBS \
    p3pipeline:c p3event:c p3pstatclient:c panda:m \
    p3pandabase:c p3pnmimage:c p3mathutil:c p3linmath:c p3putil:c p3express:c \
    pandaexpress:m \
    p3interrogatedb p3prc  \
    p3dtoolutil:c p3dtoolbase:c p3dtool:m \
    $[if $[WANT_NATIVE_NET],p3nativenet:c] \
    $[if $[and $[HAVE_NET],$[WANT_NATIVE_NET]],p3net:c p3downloader:c]

#define UNIX_SYS_LIBS m

#begin bin_target
  #define TARGET bin2c

  #define SOURCES \
    binToC.cxx binToC.h

#end bin_target
