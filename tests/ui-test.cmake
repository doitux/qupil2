# ctest ui_test_phone / ui_test_wide: qupil --ui-test (the important flows, see
# src/app/UiTest.h), then the restart half on the same data (the unsaved note is back).
#   cmake -DQUPIL=<qupil> -DSIZE=390x844 -DOUT=<folder> -P ui-test.cmake
file(REMOVE_RECURSE "${OUT}")
file(MAKE_DIRECTORY "${OUT}/data")
foreach(phase first restart)
    set(args --ui-test "${OUT}/${phase}" --lang de --ui-test-size ${SIZE} --data-dir "${OUT}/data")
    if(phase STREQUAL "restart")
        list(APPEND args --ui-test-phase restart)
    endif()
    execute_process(COMMAND "${QUPIL}" ${args} RESULT_VARIABLE result)
    if(EXISTS "${OUT}/${phase}/results.md")
        file(READ "${OUT}/${phase}/results.md" text)
        message("${text}")
    endif()
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "UI test (${phase}, ${SIZE}) failed: ${result} – screenshots and screens in ${OUT}/${phase}")
    endif()
endforeach()
