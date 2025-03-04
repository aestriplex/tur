# src/Makefile
# -----------------------------------------------------------------------
# Copyright (C) 2025  Matteo Nicoli
# 
# This file is part of TUR.
# 
# TUR is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 	
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

CC = gcc
CFLAGS = -Wall -pedantic -O3 -std=c2x
CVARS = -D_GNU_SOURCE
DEBUG_FLAGS = -Wall -pedantic -g -std=c2x
INCLUDE_PATH = /usr/local/include
INSTALL_PATH = /usr/local/bin
LIB_PATH = /usr/local/lib
LIB = -lgit2
OUT = tur

# Change include and lib path for macOS with Apple Silicon
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_S), Darwin)
	ifeq ($(UNAME_M), arm64)
		INCLUDE_PATH = /opt/homebrew/include
		LIB_PATH = /opt/homebrew/lib
	endif
endif
ifeq ($(UNAME_S), Linux)
	INCLUDE_PATH = /usr/include
	ifeq ($(UNAME_M), arm64)
		LIB_PATH = /usr/lib/aarch64-linux-gnu
	else
		LIB_PATH = /usr/lib/x86_64-linux-gnu
	endif
endif

all: *.o
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -o $(OUT) $^ -L$(LIB_PATH) $(LIB)

*.o: *.c
	$(CC) $(CVARS) -I$(INCLUDE_PATH) $(CFLAGS) -c $^

.PHONY: debug
debug: *.c
	$(CC) $(CVARS) $(DEBUG_FLAGS) -I$(INCLUDE_PATH) -o $(OUT) $^ -L$(LIB_PATH) $(LIB)

.PHONY:clean
clean:
	rm -f *.o $(OUT)

.PHONY: install
install:
	cp $(OUT) $(INSTALL_PATH)/$(OUT)
