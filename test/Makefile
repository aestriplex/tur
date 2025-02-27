# test/Makefile
# -----------------------------------------------------------------------
# Copyright (C) 2025  Matteo Nicoli
# 
# This file is part of tur
# 
# tur is free software; you can redistribute it and/or modify
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

all: test_parse_repository
	./test_parse_repository

test_parse_repository: test.c test_parse_repository.c repo.o str.o
	$(CC) $(CFLAGS) -o $@ $^

repo.o: ../src/repo.c
	$(CC) $(CFLAGS) -o $@ -c $^

str.o: ../src/str.c
	$(CC) $(CFLAGS) -o $@ -c $^
