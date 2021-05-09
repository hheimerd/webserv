//
// Created by Holli Heimerdinger on 4/20/21.
//

#include "Helper.hpp"

vector<string> split(string needle, string str)
{
	size_t len = str.length();
	vector<string> result;
	size_t start = 0;
	size_t end = 0;

	while (start < len)
	{
		end = str.find(needle, start);
		if (end == string::npos)
			end = len;
		result.push_back(str.substr(start, end - start));
		start = end + needle.length();
	}
	return result;
};

bool isContain(string str, int(*predicate)(int))
{
    return find_if(str.begin(), str.end(), predicate) != str.end();
}

int isEmptySpace(int c) 
{ 
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

pair<string, string> split_pair(string needle, string str) {
	size_t index;
	string header;
	string body;
	size_t len = str.length();

	index = str.find(needle, 0);
	if (index == string::npos) {
		return make_pair(str, "");
	}
	header = str.substr(0, index);
	body = str.substr(index + needle.length(), len - index);
	return make_pair(header, body);
}

string file_get_contents(const string& path)
{
    std::ifstream t(path);
    if (!t)
        throw exception();
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

string getTimestamp()
{
	char        buffer[256] = "";
	tm          *time;
	timeval     timeval = {};

	gettimeofday(&timeval, nullptr);
	time = localtime(&timeval.tv_sec);
	mktime(time);
	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", time);
	return string(buffer);
}

string getIP(unsigned long ip)
{
	string result = "";

//	cout << (ip & 0xFF000000);
	result += to_string(ip & 0xFF) + '.';
	result += to_string(ip >> 8 & 0xFF) + '.';
	result += to_string(ip >> 16 & 0xFF) + '.';
	result += to_string(ip >> 24 & 0xFF);
	return result;
}

int IsEmptySpace(int c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

std::string& ltrim(std::string& str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars)
{
    return ltrim(rtrim(str, chars), chars);
}

int create_dir(string path, int rights)
{
	unsigned    index;
	int         result;
	string      target;

	if (path[0] != '/')
		path = abs_path(path);
	if (is_dir(path))
		return (0);
	if (path == ".." || path == ".")
		return (0);
	index = path.find_last_of('/');
	if (index != -1) {
		target = path.substr(0, index);
		if (!is_dir(target))
			result = create_dir(target, rights);
	}
	if (result == -1)
		return -1;
	return mkdir(path.c_str(), rights);
}

int file_put_contents(string filename, const string &data, int rights)
{
	unsigned    index;
	int         fd;

	if (filename.empty())
		return -1;
	filename = abs_path(filename);
	if (!is_file(filename)) {
		index = filename.find_last_of('/');
		if (index != -1)
			if (create_dir(filename.substr(0, index)) == -1)
				return pError("mkdir");
	}
	fd = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, rights);
	if (fd == -1)
		return pError("open");
	if (write(fd, data.c_str(), data.size()) == -1)
	{
		pError("write");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


string  abs_path(string path) {
	string  result;
	int     index;
	string  tmp;
	char    buf[512] = {};
	vector<string> folders;

	if (path[0] == '/')
		return path;
	getcwd(buf, sizeof(buf));
	result = buf;
	folders = split("/", path);

	for (int i = 0; i < folders.size(); i++) {
		tmp = folders[i];
		if (tmp == "..")
			result.erase(result.find_last_of('/'), result.size());
		else if (tmp == "." || tmp.empty())
			continue ;
		else
			result += "/" + tmp;
	}
	return result;
}

int pError(string const&program)
{
	std::cerr << program << ": " << strerror(errno) << std::endl;
	return -1;
}

static string storage;


string readBefore(int fd, string const &needle, unsigned buf_size)
{
	string  result;
	string  tmp;
	long     n_pos = -1;
	char    *buf = new char[buf_size + 1]{0};

	while (true)
	{
		if (storage.empty()) {
			if (read(fd, buf, buf_size) < 0)
				break;
			tmp = buf;
		} else {
			tmp = storage;
		}
		memset(buf, 0, buf_size);
		n_pos = tmp.find(needle);
		if (n_pos != -1)
		{
			result += tmp.substr(0, n_pos);
			storage = tmp.substr(n_pos + needle.length());
			break;
		} else
			storage = "";
		result += tmp;
	}
	delete[] buf;
	return result;
}

string readLine(int fd, unsigned buf_size)
{
	return readBefore(fd, "\r\n", buf_size);
}

void clearStorage()
{
	storage = "";
}

string readCount(int fd, unsigned long count)
{
	char *buf = new char[count]{0};
	string result = storage;

	if (result.length() > count)
	{
		storage = result.substr(count + 2);
		result = result.substr(0, count);
	} else {
		read(fd, buf, count - result.length());
		result += buf;
		read(fd, buf, 2);
	}
	delete[] buf;
	return result;
}

string readFull(int fd)
{
	string  result;
	char    buf[2048] = {0};

	result = storage;
	while (read(fd, buf, 2047) > 0)
	{
		result += buf;
		memset(buf, 0, 2048);
	}
	return result;
}

bool is_file(string const&path)
{
	struct stat buf{};
	if (stat(path.c_str(), &buf) == -1)
		return false;
	return S_ISREG(buf.st_mode);
}

bool is_dir(string const&path)
{
	struct stat buf{};
	if (stat(path.c_str(), &buf) == -1)
		return false;
	return S_ISDIR(buf.st_mode);
}

bool exists(string const&path)
{
	struct stat buf{};
	if (stat(path.c_str(), &buf) == -1)
		return false;
	return true;
}

vector<string> *get_dir_content(string const &path)
{
	vector<string> *result;
	DIR *dir = opendir(path.c_str());
	struct dirent* dirent;

	if (!dir)
		return nullptr;

	result = new vector<string>;

	while ((dirent = readdir(dir)) != nullptr)
		result->push_back(dirent->d_name);

	closedir(dir);

	return result;
}
