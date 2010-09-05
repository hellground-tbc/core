add_subdirectory(updates)
add_subdirectory(tools)
add_subdirectory(HG_updates)


########### install files ###############

FILE(GLOB _SQL *.sql)

install(FILES ${_SQL} DESTINATION share/trinity/sql)
