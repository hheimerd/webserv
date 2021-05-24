//
// Created by Holli Heimerdinger on 5/15/21.
//

#include "Reader.hpp"

using namespace std;

int Reader::readBefore(string &result, int fd, string const &needle, unsigned buf_size)
{
	string      tmp;
	long        n_pos = -1;
	char        *buf = new char[buf_size + 1]{0};
	int             error;
	int         counter = 0;


	if (storage.empty()) {
		if (reedUsed)
			return 1;
		if ((error = read(fd, buf, buf_size)) < 0) {
			pError("read");
			delete[] buf;
			return -1;
		}
		reedUsed = true;
		storage = buf;
		delete[] buf;
	}
	n_pos = storage.find(needle);
	if (n_pos == -1)
		return 1;

	result = storage.substr(0, n_pos);
    storage = storage.substr(n_pos + needle.length());
	return 0;
}

int Reader::readLine(string &result, int fd, unsigned buf_size)
{
	return readBefore(result,fd, "\r\n", buf_size);
}

void Reader::clearStorage()
{
	storage = "";
}

int Reader::readCount(string &result, unsigned long count, int fd)
{
	char *buf = new char[count]{0};
	result = storage;
	int read_count;

	if (count > MAX_STACK_SIZE)
		use_file = true;

	if (result.length() >= count)
	{
		storage = result.substr(count + 2);
		result = result.substr(0, count);
	} else {
        if (reedUsed)
            return 1;
		read_count = read(fd, buf, count - result.length());
		if (read_count < 1)
			return -1;
		reedUsed = true;
		if (use_file)
		{
			if (!file_fd)
			{
				file_fd = open(TMP_PATH "/tmp_big_in.txt", O_CREAT|O_RDWR|O_TRUNC, 0644);
				if (file_fd == -1){
					pError("open");
					delete[] buf;
					return -1;
				}
			}
			if (write(file_fd, buf, read_count) != read_count) {
				pError("write");
				delete[] buf;
				return -1;
			}
		} else {
			result += buf;
		}
		delete[] buf;
		storage = "";
	}
	return 0;
}

Reader::Reader(): reedUsed(false)
{
	use_file = false;
	file_fd = 0;
    storage = "";
};

Reader::Reader(Reader const &rhs)
{
	operator=(rhs);
};

Reader::~Reader()
{
	if (file_fd)
		close(file_fd);
};

Reader &Reader::operator=(Reader const &rhs)
{
	if (this != &rhs) {
		this->storage = rhs.storage;
	}
	return *this;
}

int Reader::getStorage(string &result) {
    result = storage;
    return 0;
};