# test/Makefile
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
CVARS = -D_GNU_SOURCE
CFLAGS = -Wall -pedantic -O3 -std=c2x
TEST_BINS = test_parse_repository test_parse_email_list test_str test_utils test_opts_args test_lookup_table test_array

all: $(TEST_BINS)
	./test_parse_repository
	./test_parse_email_list
	./test_str
	./test_utils
	./test_opts_args
	./test_lookup_table
	./test_array

test_parse_repository: test.c test_parse_repository.c repo.o str.o utils.o log.o array.o commit.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^ -lgit2

test_parse_email_list: test.c test_parse_email_list.c opts_args.o str.o utils.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^

test_str: test.c test_str.c str.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^

test_utils: test.c test_utils.c str.o utils.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^

test_opts_args: test.c test_opts_args.c opts_args.o utils.o str.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^

test_lookup_table: test.c test_lookup_table.c lookup_table.o str.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^

test_array: test.c test_array.c commit.o str.o log.o array.o
	$(CC) $(CVARS) $(CFLAGS) -o $@ $^ -lgit2

repo.o: ../src/repo.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

str.o: ../src/str.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

opts_args.o: ../src/opts_args.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

utils.o: ../src/utils.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

log.o: ../src/log.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

lookup_table.o: ../src/lookup_table.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

array.o: ../src/array.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

commit.o: ../src/commit.c
	$(CC) $(CVARS) $(CFLAGS) -o $@ -c $^

.PHONY: clean
clean:
	rm -f *o $(TEST_BINS)
