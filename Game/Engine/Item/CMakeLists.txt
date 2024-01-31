set(ItemSrc
  Item.cpp
)

set(ItemHdr
  Item.h
)

add_library(Item ${ItemSrc})
target_link_libraries(Item PRIVATE Qt5::Core)
