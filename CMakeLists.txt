cmake_minimum_required(VERSION 3.5)
project(game)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-fPIC)

# 设置生成 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 配置clang-tidy
# set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
add_custom_target(
    clang-tidy
    COMMAND run-clang-tidy -fix -format -quiet -j 4
    COMMENT "Running clang-tidy"
)

# Find the Google Test package
find_package(GTest REQUIRED)

# 配置iwyu
find_program(IWYU_PATH NAMES include-what-you-use include-what-you-use)
add_custom_target(
    iwyu
    COMMAND python3 ${CMAKE_SOURCE_DIR}/build_tools/iwyu_tool.py -p . -- -Xiwyu --transitive_includes_only -Xiwyu --cxx17ns -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/build_tools/iwyu.imp
    COMMENT "Running iwyu"
)

add_custom_target(
    clang-format
    COMMAND bash ${CMAKE_SOURCE_DIR}/build_tools/clang_format.sh ${CMAKE_SOURCE_DIR}
    COMMENT "Running clang-format"
)


# 查找Qt包
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

# 添加工程的根目录到头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS})

add_subdirectory(Game)

add_executable(Exec Game/Entry/main.cpp)
target_link_libraries(Exec PRIVATE MainWindow)
target_link_libraries(Exec PRIVATE Games)

# 添加可执行文件
# add_executable(MyQtProject main.cpp)

# 链接Qt库
# target_link_libraries(MyQtProject PRIVATE Qt5::Core Qt5::Widgets)
