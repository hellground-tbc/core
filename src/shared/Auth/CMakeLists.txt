
########### next target ###############

SET(hellgroundauth_STAT_SRCS
   AuthCrypt.cpp
   AuthCrypt.h
   BigNumber.cpp
   BigNumber.h
   Hmac.cpp
   Hmac.h
   SARC4.cpp
   SARC4.h
   Sha1.cpp
   Sha1.h
   md5.c
   md5.h
   WardenKeyGeneration.h
)

add_library(hellgroundauth STATIC ${hellgroundauth_STAT_SRCS})
