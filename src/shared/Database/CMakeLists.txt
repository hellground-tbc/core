SET(hellgrounddatabase_STAT_SRCS
   Database.cpp
   Database.h
   DatabaseEnv.h
   DatabaseImpl.h
   Field.cpp
   Field.h
   QueryResult.cpp
   QueryResult.h
   QueryResult.cpp
   QueryResultMysql.cpp
   QueryResultMysql.h
   DatabaseMysql.h
   DatabaseMysql.cpp
   SQLStorage.cpp
   SQLStorage.h
   SQLStorageImpl.h
   SqlPreparedStatement.cpp
   SqlPreparedStatement.h
   SqlDelayThread.cpp
   SqlDelayThread.h
   SqlOperations.cpp
   SqlOperations.h
   dbcfile.cpp
   dbcfile.h
)

add_library(hellgrounddatabase STATIC ${hellgrounddatabase_STAT_SRCS})
