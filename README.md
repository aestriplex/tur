# TUR - Multi-Repository Commit Tracker

TUR is an open-source command-line utility designed to help developers and project managers track commit histories across multiple repositories. By providing a flexible and feature-rich tool, TUR enables users to analyze and visualize commit contributions with ease.

## Features

* Track commits by one or multiple email addresses
* Support for multiple repositories via a simple repository list file
* Multiple output formats:
    - Standard output (stdout)
    - LaTeX
    - HTML
    - Jekyll/Markdown
* Sorting and grouping options

## Requirements

* Make
* GCC (or compatible C compiler)
* libgit2

## Build

[![C/C++ CI](https://github.com/aestriplex/tur/actions/workflows/c-cpp.yml/badge.svg?branch=main)](https://github.com/aestriplex/tur/actions/workflows/c-cpp.yml)

By simply running
```bash
make
```
You both build the tool and execute the tests. If you want to perform these operations separately, run
```bash
make tur
make test
```
To install just type
```bash
sudo make install
```
> **!** It's going to install tur in `/usr/local/bin`, so ensure to have it in your path.

## Usage

| Option | Description |
|--------|-------------|
| `-h`, `--help` | Show help message |
| `-v`, `--version` | Display version |
| `-g`, `--group` | Group commits by repository |
| `-s`, `--sort` | Sort commits by date |
| `-e EMAIL`, `--email EMAIL` | Specify a single email address |
| `-m FILE`, `--mail-list FILE` | Provide a comma-separated list of emails |
| `-o FILE`, `--out FILE` | Specify an output file format (e.g., `.tex`, `.html`, `.md`) |
| `-r FILE`, `--repos FILE` | Specify a file containing repository paths |

Either option `m` or `e` is required. If you don't specify any output file, it prints in `stdout`.

#### Repository list

By default, TUR looks for a file called `.rlist` in the current path. The file should have this format
```
/path/to/local/repo1[origin/repo1/url]
/path/to/local/repo2[origin/repo2/url]
``` 
If you want to rename that file (or put it in another directory) you should specify its path via the option `-f`

#### Example of usage

```
% tur -f repository_list.txt -e example@provider.com -o commits.tex -sg
```