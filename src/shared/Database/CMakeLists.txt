SET(trinitydatabase_STAT_SRCS
   DBCStores.cpp
   DBCStores.h
   DBCStructure.h
   DBCfmt.cpp
   Database.cpp
   Database.h
   DatabaseEnv.h
   DatabaseImpl.h
   DatabaseMysql.cpp
   DatabaseMysql.h
   Field.cpp
   Field.h
   QueryResult.h
   SQLStorage.cpp
   SQLStorage.h
   SQLStorageImpl.h
   SqlDelayThread.cpp
   SqlDelayThread.h
   SqlOperations.cpp
   SqlOperations.h
   dbcfile.cpp
   dbcfile.h
)

add_library(trinitydatabase STATIC ${trinitydatabase_STAT_SRCS})
