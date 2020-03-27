GAME_NAME := AdvanceWars
GAME_DIR := .
CLIENT_DIR := . Scenes Layers Sprites Menus Dialogs Tables
SERVER_DIR := .
includeFilename := $(if $(Client),../libGamesClient/Makefile,../libGamesEngines/Makefile)
include $(includeFilename)