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
| `-e <email>`, `--email <email>` | Specify a single email address |
| `-m <email_1,...,email_n>`, `--mail-list <email_1,...,email_n>` | Provide a comma-separated list of emails |
| `-o <FILE>`, `--out <FILE>` | Specify an output file format (e.g., `.tex`, `.html`, `.md`) |
| `-r <FILE>`, `--repos <FILE>` | Specify a file containing repository paths |

Either option `m` or `e` is required. If you don't specify any output file, it prints in `stdout`.

#### Repository list

By default, TUR looks for a file called `.rlist` in the current path. The file should have this format
```
/path/to/local/repo1[origin/repo1/url]
/path/to/local/repo2[origin/repo2/url]
``` 
If you’d like to rename that file (or put it in another directory), you should specify its path via the option `-r`

#### Example of usage

The following command produce a LaTeX file with the grouped commit list, sorted by date, in which `example@provider.com` is either the main author or the co-author.
```
% tur -f repository_list.txt -e example@provider.com -o commits.tex -sg
```
To specify more email adrresses, you just have to use the option `-m` with the *comma-separated* list of emails
```
% tur -f repository_list.txt -m example1@provider1.com,example2@provider2.com -o commits.tex -sg
```
