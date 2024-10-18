set(SOURCES
    CMSkt.cpp
    rSkt.cpp
    vSkt.cpp
    multiset_test.cpp
    mySkt.cpp
)

add_library(multiset ${SOURCES})
target_link_libraries(multiset singleset)