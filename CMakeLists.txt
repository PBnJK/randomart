cmake_minimum_required(VERSION 3.28)

project(
	RandomArt
	VERSION 0.2.0
	LANGUAGES C
)

add_executable(
	randomart

	"src/main.c"

	"src/ast.c"
	"src/lang.c"
	"src/mempool.c"
	"src/node.c"
)

target_compile_features(randomart PRIVATE c_std_99)
target_compile_options(randomart PRIVATE -Wall -Wextra -pedantic)

target_include_directories(
	randomart
	PRIVATE
	${PROJECT_SOURCE_DIR}/inc
)

find_library(LIBM m)
target_link_libraries(randomart PRIVATE ${LIBM})

