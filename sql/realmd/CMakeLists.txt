
########### install files ###############

FILE(GLOB _SQL_UPDATES *.sql)

install(FILES ${_SQL_UPDATES} DESTINATION share/trinity/sql/realmd)
