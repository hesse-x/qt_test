set(MainWindowSrc
  MainWindow.cpp
)

set(MainWindowHdr
  MainWindow.h
)

add_library(MainWindow ${MainWindowSrc})
target_link_libraries(MainWindow PRIVATE Qt5::Gui Qt5::Widgets Engine)
