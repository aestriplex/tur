# TODO

- [ ] Cache the `repository_array_t` results in a file saved in `.tur/cached` in the same directory of `.rlist`
- [ ] Add the option -c that allows the user to clear the cache, i.e. removes the file `.tur/cached`
- [ ] At the moment the url parameter works only for GitHub repositories, and the platform-specific url is contained in `GITHUB_URL`, a constant defined in `utils.c`. Move the platform-specific url from there to the `.rlist` file, so other services (such as GitLab) can be used
