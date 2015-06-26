file(GLOB_RECURSE INCS "*.h")
file(GLOB_RECURSE SRCS "*.cc")

add_executable(tests ${SRCS} ${INCS})

add_test(UnitTests tests)
