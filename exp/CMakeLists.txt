file(GLOB_RECURSE ALL_EXP_C *_exp.c)

foreach(ONE_EXP_C ${ALL_EXP_C})
    get_filename_component(ONE_EXP_EXEC ${ONE_EXP_C} NAME_WE)
    add_executable (${ONE_EXP_EXEC} ${ONE_EXP_C})
    target_link_libraries(${ONE_EXP_EXEC}  PUBLIC Src Ops Utils ${CMAKE_DL_LIBS} python3.6m m)
endforeach()
