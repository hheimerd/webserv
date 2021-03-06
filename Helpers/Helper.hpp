//
// Created by Holli Heimerdinger on 4/20/21.
//

#ifndef HELPER_HPP
#define HELPER_HPP

#include "../includes.hpp"
#include "dirent.h"
#include <fstream> // TODO: #include <fstream>
#include <libc.h>

using namespace std;

vector<string> split(string needle, string str);
pair<string, string> split_pair(string needle, string str);

string  file_get_contents(const string& path);
int     create_dir(string path, int rights = 0777);
int     file_put_contents(string filename, const string &data, int rights = 0777);
int     file_put_contents(string filename, int fd, int rights = 0777);


string to_http_date(time_t const &timeval);
string get_http_timestamp();
string getIP(unsigned long ip);

bool is_file(string const&path);
bool is_dir(string const&path);
bool exists(string const&path);
string last_modified(string path);

string& ltrim(string& str, const string& chars = "\t\n\v\f\r ");
string& rtrim(string& str, const string& chars = "\t\n\v\f\r ");
string& trim(string& str, const string& chars = "\t\n\v\f\r ");

int cmp_word(string needle, string haystack, int limit);
int		match(const char *s1, const char *s2);
int		n_match(const char *s1, const char *s2);

int isEmptySpace(int c);
bool isContain(string str, int(*predicate)(int));

int pError(string const&program);

string      toUpper(string str);

string      toCgiHeader(string str);
string      toChunked(string body);

string      itoa(int value, int base = 10);

vector<string> *get_dir_content(string const &path);

string  abs_path(string path);

template <typename T, typename  F>
bool contains(T begin, T end, F value)
{
    while (begin != end)
    {
        begin++;
        if (*begin == value)
            return true;
    }
    return false;
}

#endif //WEBSERV_HELPER_HPP
