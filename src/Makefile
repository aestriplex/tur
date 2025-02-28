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
DEBUG_FLAGS = -Wall -pedantic -g -std=c2x
INCLUDE_PATH = /usr/local/include
LIB_PATH = /usr/local/lib
LIB = -lgit2
OUT = tur

all: *.o
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -L$(LIB_PATH) $(LIB) -o $(OUT) $^

*.o: *.c
	$(CC) $(CFLAGS) -c $^

.PHONY: debug
debug: *.c
	$(CC) $(DEBUG_FLAGS) $(LIB) -I$(INCLUDE_PATH) -L$(LIB_PATH) -o $(OUT) $^

.PHONY:clean
clean:
	rm -f *.o $(OUT)
