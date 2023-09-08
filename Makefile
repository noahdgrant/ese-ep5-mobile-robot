# -*- Makefile -*-

# Author: Noah Grant
# .Copyright (c) 2023 Author. All Rights Reserved.

# Variable declaration
CC=clang
CFLAGS=-g -Wall
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR=bin
BIN=/main

# Debug build
all:$(BIN)

# Release build
release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(BIN)

# Create binary files and object files
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Additional build commands
clean:
	$(RM) -r $(BINDIR)/* $(OBJ)/*
