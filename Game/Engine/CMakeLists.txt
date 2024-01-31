add_subdirectory(Item)

set(EngineSrc
  Engine.cpp
  Registry.cpp
)

set(EngineHdr
  Engine.h
  Registry.h
)

add_library(Engine SHARED ${EngineSrc})
# target_compile_options(Engine PRIVATE -fvisibility=hidden)
target_link_libraries(Engine PRIVATE Item)
