add_subdirectory(tools)
add_subdirectory(characters)
add_subdirectory(realmd)
add_subdirectory(world)


########### install files ###############

FILE(GLOB _SQL *.sql)

install(FILES ${_SQL} DESTINATION share/trinity/sql)
