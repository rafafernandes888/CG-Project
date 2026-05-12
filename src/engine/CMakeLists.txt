cmake_minimum_required(VERSION 3.10)

project(engine)

file(GLOB SRC_FILES src/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/libs/rapidxml-1.13)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)
include_directories(${CMAKE_SOURCE_DIR}/src/shared/include)

target_link_libraries(${PROJECT_NAME} shared)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found!")
endif()

if(WIN32)
    message(STATUS "Configurando GLUT e GLEW a partir da pasta 'toolkits'")
    set(TOOLKITS_FOLDER "${CMAKE_SOURCE_DIR}/toolkits")

    # GLUT
    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found!")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)
    link_directories(${TOOLKITS_FOLDER}/glut)

    # GLEW
    if(NOT EXISTS "${TOOLKITS_FOLDER}/glew/GL/glew.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glew/glew32.lib")
        message(FATAL_ERROR "GLEW not found in toolkits/glew!")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glew)
    link_directories(${TOOLKITS_FOLDER}/glew)

    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
        ${TOOLKITS_FOLDER}/glew/glew32.lib
    )

    # copia as DLLs automaticamente após cada build
    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TOOLKITS_FOLDER}/glut/glut32.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glut32.dll"
            COMMENT "A copiar glut32.dll para a pasta do executável"
        )
    endif()

    if(EXISTS "${TOOLKITS_FOLDER}/glew/glew32.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TOOLKITS_FOLDER}/glew/glew32.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/glew32.dll"
            COMMENT "A copiar glew32.dll para a pasta do executável"
        )
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${GLEW_LIBRARIES}
    )

    if(NOT GLUT_FOUND)
        message(ERROR ": GLUT not found!")
    endif()
endif()