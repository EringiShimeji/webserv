set(CMAKE_CXX_STANDARD 98)
# 実行ファイルを /bin に出力
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ここに lib/ 以下のファイルを追加する
# 実行ファイルとテストで再利用するために、静的ライブラリを作成
add_library(webserv_lib STATIC
        lib/add.cpp
        lib/add.hpp
)
# lib/ を include path に追加
# PUBLIC にすると、依存するターゲットにも反映される
target_include_directories(webserv_lib PUBLIC lib)

# webserv という実行ファイルを作成
add_executable(webserv cmd/main.cpp)
target_link_libraries(webserv PRIVATE webserv_lib)