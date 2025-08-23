cmake_minimum_required(VERSION 3.14)
project(Lifegame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Détection de la cible (linux ou windows)
if(NOT DEFINED TARGET)
    message(FATAL_ERROR "Veuillez définir le flag -DTARGET=linux ou -DTARGET=windows")
endif()


# --- Source files ---
file(GLOB_RECURSE SOURCES src/*.cpp)



# Linux : SDL2 installé avec les paquets
if(${TARGET} STREQUAL "linux")

    # --- Find SDL2 ---
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})

    # --- Find SDL2_ttf ---
    find_library(SDL2_TTF_LIB SDL2_ttf HINTS /usr/lib/x86_64-linux-gnu)
    if(NOT SDL2_TTF_LIB)
        message(FATAL_ERROR "SDL2_ttf not found! Run: sudo apt install libsdl2-ttf-dev")
    endif()


    add_executable(lifegame ${SOURCES})

    # --- Includes ---
    target_include_directories(lifegame PRIVATE
        includes
        ${SDL2_INCLUDE_DIRS}
        /usr/include/SDL2
    )

    # --- Link libraries ---
    target_link_libraries(lifegame PRIVATE
        ${SDL2_LIBRARIES}
        SDL2main
        ${SDL2_TTF_LIB}
    )


elseif(${TARGET} STREQUAL "windows")# Windows : lier les libs manuellement
    # --- Windows-specific settings ---
    set(WINDOWS_LIBS
        -lsetupapi
        -lwinmm
        -limm32
        -lversion
        -ldinput8
        -ldxguid
        -ldxerr8
        -luser32
        -lgdi32
        -lwinmm
        -limm32
        -lole32
        -loleaut32
        -lshell32
        -lsetupapi
        -lversion
        -luuid
        -static-libgcc
        -static-libstdc++
    )

    enable_language(RC)  # Active le compilateur de ressources
    set(RESOURCE_FILES ${CMAKE_SOURCE_DIR}/resources.rc)

    # Indiquer les chemins vers SDL2
    set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/SDL_lib/SDL2/include")
    set(SDL2_LIB_DIR "${CMAKE_SOURCE_DIR}/SDL_lib/SDL2/lib")

    # Inclure les dossiers d'includes
    include_directories(
        ${SDL2_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIR}
        includes
    )

    # Créer l'exécutable
    add_executable(lifegame WIN32 ${SOURCES} ${RESOURCE_FILES}) # le WIN32 permet d'avoir une UI plutôt que le console

    # Linker les libs SDL2 (attention à l'ordre)
    target_link_libraries(lifegame
        
        -lmingw32
        "${SDL2_LIB_DIR}/libSDL2.a"
        "${SDL2_LIB_DIR}/libSDL2main.a"
        "${SDL2_LIB_DIR}/libSDL2_ttf.a"
        "${WINDOWS_LIBS}"
        -lrpcrt4            # pour les gui Windows
        -mwindows           # pour les gui Windows
    )

    # Copier les DLLs dans le dossier de build pour l'exécutable
    file(COPY SDL_lib/SDL2/bin DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Copier dossier misc (si tu en as besoin)
file(COPY misc DESTINATION ${CMAKE_BINARY_DIR})