set(GameSrc
  Game0.cpp
)

# set(GameHdr
# )

add_library(Games ${GameSrc})
target_compile_features(Games PRIVATE cxx_std_20)

target_link_options(Games INTERFACE
    -Wl,--whole-archive $<TARGET_FILE:Games> -Wl,--no-whole-archive
)
