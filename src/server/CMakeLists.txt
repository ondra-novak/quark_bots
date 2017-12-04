cmake_minimum_required(VERSION 2.8)
add_compile_options(-std=c++14)
file(GLOB qbots_SRC "*.cpp")
set(designs designdocs/index.design.cpp designdocs/trades.design.cpp designdocs/positions.design.cpp)
add_custom_command(OUTPUT ${designs} COMMAND $(MAKE) -C designdocs all ) 
add_custom_target(buildDesigns COMMAND $(MAKE) -C designdocs all  )
add_custom_target(cleanDesigns COMMAND $(MAKE) -C designdocs clean-designs)
add_executable (qbots ${qbots_SRC} ${designs})
add_dependencies(qbots buildDesigns)  
target_link_libraries (qbots LINK_PUBLIC simpleRpcServer simpleServer jsonrpc_client couchit imtjson pthread)
 
install(TARGETS qbots 
        DESTINATION "bin"
        ) 

install(FILES "../../conf/qbots.conf" DESTINATION "etc/qbots") 
 
