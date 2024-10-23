set(SOURCES
    CMSkt.cpp
    rSkt.cpp
    vSkt.cpp
    multiset_test.cpp
    AMSkt.cpp
)

add_library(multiset ${SOURCES})
target_link_libraries(multiset singleset)