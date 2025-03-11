# Makefile
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

all: tur test

tur:
	@echo "*****************************\n  Building tur\n*****************************"
	@$(MAKE) -C src/

.PHONY: test
test:
	@echo "*****************************\n  TESTS\n*****************************"
	@$(MAKE) -C test/

.PHONY: debug
debug:
	@$(MAKE) -C src debug

.PHONY: clean
clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C test clean

.PHONY: install
install:
	@$(MAKE) -C src install
