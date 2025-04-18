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
| `-d`, `--diffs` | Shows the commit stats (line added, removed and file changed) |
| `-g`, `--group` | Group commits by repository |
| `-m, --message` | Shows the first line of the commit message |
| `-v`, `--version` | Display version |
| `--date-only` | Each commit will be printed without time information |
| `--no-ansi` | Avoid ANSI escape characters in terminal (e.g. colors) |
| `-e <e_1,...,e_n>`, `--emails <e_1,...,e_n>` | Provide a comma-separated list of emails |
| `-o <FILE>`, `--out <FILE>` | Specify an output file format (e.g., `.tex`, `.html`, `.md`) |
| `-r <FILE>`, `--repos <FILE>` | Specify a file containing repository paths |
| `-s`, `--sort` | Sort commits by date. You have to specify an order: ASC (Ascending order) or DESC (Descending order) |

Option `e` is required. If you don't specify any output file, it prints in `stdout`.

#### Repository list

By default, TUR looks for a file called `.rlist` in the current path. The file should have this format
```
/path/to/local/repo1[origin/repo1/url]
/path/to/local/repo2[origin/repo2/url]
``` 
If you’d like to rename that file (or put it in another directory), you should specify its path via the option `-r`

#### Example of usage

The following command produce a LaTeX file with the grouped commit list, sorted by date (descending), in which `example@provider.com` is either the main author or the co-author.
```
% tur -f repository_list.txt -e example@provider.com -o commits.tex -g -s DESC
```
To specify more email adrresses, you can still use the option `-e` with the *comma-separated* list of emails:
```
% tur -f repository_list.txt -e example1@provider1.com,example2@provider2.com -o commits.tex -g -s DESC
```
Then you can add additional options like `-m` (to print the commit message) and `-d` to print the commit diff
```
$ tur -f repository_list.txt -e example1@provider1.com,example2@provider2.com -o commits.tex -gdm -s DESC
```
> **NOTE** The stdout output generates only textual bytes, so it's "easily greppable". If you want to avoid ANSI escape codes, you just have to use the option --no-ansi

So, a pipeline could look like this
```
$ tur -f repository_list.txt -e example1@provider1.com,example2@provider2.com -gdm -s DESC | grep repo1
```
