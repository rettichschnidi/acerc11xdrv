CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#include all test files
FILE(GLOB test_sources test_*.cpp)

#add the cute-headers as well as the ones of our own application
INCLUDE_DIRECTORIES("../" "cute/")

CONFIGURE_FILE (
        "${PROJECT_SOURCE_DIR}/src/tests/testing_helper.h.in"
        "${PROJECT_BINARY_DIR}/configured_files/testing_helper.h"
)

#execture all the cute tests
FOREACH(test ${test_sources} )
	STRING(REGEX REPLACE ".*test_(.*)" "\\1" basename "${test}")
	STRING(REGEX REPLACE "(.*)(.cpp)" "\\1" noext "${basename}")
	ADD_EXECUTABLE(testexe_${noext} ${test})
	TARGET_LINK_LIBRARIES(testexe_${noext} 
		corelib
		${CORE_LIBS}
	)
	ADD_TEST(test_${noext} ${EXECUTABLE_OUTPUT_PATH}/testexe_${noext})
	SET_TESTS_PROPERTIES(test_${noext} PROPERTIES FAIL_REGULAR_EXPRESSION "failure") #this test fails if cute detects an error
ENDFOREACH()

#Ever program should have a help parameter
FOREACH(EXECUTABLE ${EXECUTABLES})
    ADD_TEST("${EXECUTABLE}:help_available" ${PROJECT_BINARY_DIR}/bin/${EXECUTABLE} "--help")
    SET_TESTS_PROPERTIES("${EXECUTABLE}:help_available" PROPERTIES PASS_REGULAR_EXPRESSION "Shows this information" TIMEOUT 5)
    ADD_TEST("${EXECUTABLE}:help_shows_up_if_no_parameter" ${PROJECT_BINARY_DIR}/bin/${EXECUTABLE})
    SET_TESTS_PROPERTIES("${EXECUTABLE}:help_shows_up_if_no_parameter" PROPERTIES PASS_REGULAR_EXPRESSION "Shows this information" TIMEOUT 5)
ENDFOREACH()

IF(ACERC11XDRV_CREATEPICTUREWIDGET)
    FILE(COPY ${PROJECT_SOURCE_DIR}/example-data/ DESTINATION ${PROJECT_BINARY_DIR}/example-data/)
    FILE(GLOB PICTURE_WIDGET_TEST_PICTURES ${PROJECT_BINARY_DIR}/example-data/pictures/*jpg)
    #overkill to test it for all of them...
    FOREACH(PIC ${PICTURE_WIDGET_TEST_PICTURES})
        ADD_TEST("no-overwrite-per-default:${PIC}" ${PROJECT_BINARY_DIR}/bin/createpicturewidget ${PIC})
        SET_TESTS_PROPERTIES("no-overwrite-per-default:${PIC}" PROPERTIES PASS_REGULAR_EXPRESSION "Error:\ File\ ${PIC}.widget\ already\ exists")
        ADD_TEST("overwrite-if-existing:${PIC}" ${PROJECT_BINARY_DIR}/bin/createpicturewidget ${PIC} "--overwriteIfExisting")
    ENDFOREACH()
ENDIF()

IF(ACERC11XDRV_CREATEBRIGHTNESSWIDGET)
    #test all parameters
    ADD_TEST("accept_level_maximum:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level maximum --overwriteIfExisting)
    ADD_TEST("accept_level_high:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level high --overwriteIfExisting)
    ADD_TEST("accept_level_medium:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level medium --overwriteIfExisting)
    ADD_TEST("accept_level_low:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level low --overwriteIfExisting)

    ADD_TEST("reject_level_maximummaximum:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level maximummaximum )
    ADD_TEST("reject_level_DOES_NOT_EXIST:" ${PROJECT_BINARY_DIR}/bin/createbrightnesswidget --level DOES_NOT_EXIST)
    SET_TESTS_PROPERTIES("reject_level_maximummaximum:" "reject_level_DOES_NOT_EXIST:" PROPERTIES PASS_REGULAR_EXPRESSION "Error\ in\ arguments\ parsing:\ in\ option\ 'level':\ invalid\ option\ value")
ENDIF()