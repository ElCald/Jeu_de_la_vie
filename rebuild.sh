#!/bin/bash

export DISPLAY=:0
export XDG_RUNTIME_DIR="/tmp/runtime-$USER"

BUILD_LINUX="build_debug"
BUILD_WIN="build_release"
VERSION="1.1"

if [ "$1" == "windows" ]; then
    echo "Compilation Windows"
    # Supprimer le répertoire build_release s'il existe
    if [ -d $BUILD_WIN ]; then
        echo "Suppression du dossier $BUILD_WIN..."
        rm -rf $BUILD_WIN
    fi

    # Créer le dossier build_release
    mkdir $BUILD_WIN
    cd $BUILD_WIN

    # Lancer CMake
    echo "Génération du projet avec CMake..."
    cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-toolchain.cmake .. -DTARGET=windows




elif [ "$1" == "linux" ]; then
    echo "Compilation Linux"
    # Supprimer le répertoire build_debug s'il existe
    if [ -d $BUILD_LINUX ]; then
        echo "Suppression du dossier $BUILD_LINUX..."
        rm -rf $BUILD_LINUX
    fi

    # Créer le dossier build_debug
    mkdir $BUILD_LINUX
    cd $BUILD_LINUX

    # Lancer CMake
    echo "Génération du projet avec CMake..."
    cmake .. -DTARGET=linux



else
    echo "Usage : $0 [windows|linux]"
    exit 1
fi


# Vérifier si cmake a réussi
if [ $? -eq 0 ]; then
    echo "Configuration CMake réussie."
else
    echo "Erreur lors de la configuration CMake."
    exit 1
fi

# Compiler le projet
echo "Compilation avec make..."
make -j$(nproc)
if [ $? -eq 0 ]; then
    echo "Build terminé avec succès."
else
    echo "Erreur lors de la compilation."
    exit 1
fi



if [ "$1" == "windows" ]; then
    echo "Création de l'archive"
    
    mkdir win_archiv
    cp lifegame.exe win_archiv
    cp -r bin win_archiv
    cp -r misc win_archiv

    cd win_archiv

    zip -r LifeGame_x86_64_$VERSION.zip *
    echo "Archive crée avec succès"

fi