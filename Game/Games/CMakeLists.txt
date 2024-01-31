set(GameSrc
  Game0.cpp
)

# set(GameHdr
# )

add_library(Games SHARED ${GameSrc})
target_compile_features(Games PRIVATE cxx_std_20)
