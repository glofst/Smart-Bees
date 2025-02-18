/**
 * @author: Polyakov Daniil
 * @mail: arjentix@gmail.com
 * @github: Arjentix
 * @date: 14.09.2019
*/

#include "HTTPHandler.h"
#include "test_runner.h"

#include <sstream>

using namespace std;

ostream& operator<<(ostream& os, HTTPHandler::Method method) {
	os << static_cast<int>(method);
	return os;
}

void test_get()
{
	istringstream input(
		"GET /wiki/page HTTP/1.1\r\n"
		"Host: ru.wikipedia.org\r\n"
		"User-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\r\n"
		"Accept: text/html\r\n"
		"Connection: close\r\n"
		"\r\n"
	);
	map<string, string> expected_headers = {
		{"Host", "ru.wikipedia.org"},
		{"User-Agent", "Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5"},
		{"Accept", "text/html"},
		{"Connection", "close"}
	};

	auto req = HTTPHandler::parse_request(input.str());
	ASSERT_EQUAL(req.method, HTTPHandler::Method::GET);
	ASSERT_EQUAL(req.headers, expected_headers);
	ASSERT_EQUAL(req.uri, "/wiki/page");
	ASSERT(req.body.empty());
}

void test_post()
{
	istringstream input("POST http://www.site.ru/news.html HTTP/1.0\r\n"
		"Host: www.site.ru\r\n"
		"Referer: http://www.site.ru/index.html\r\n"
		"Cookie: income=1\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 35\r\n"
		"\r\n"
		"login=Petya%20Vasechkin&password=qq"
	);

	map<string, string> expected_headers = {
		{"Host", "www.site.ru"},
		{"Referer", "http://www.site.ru/index.html"},
		{"Cookie", "income=1"},
		{"Content-Type", "application/x-www-form-urlencoded"},
		{"Content-Length", "35"},
	};

	string expected_body = "login=Petya%20Vasechkin&password=qq";

	auto req = HTTPHandler::parse_request(input.str());
	ASSERT_EQUAL(req.method, HTTPHandler::Method::POST);
	ASSERT_EQUAL(req.headers, expected_headers);
	ASSERT_EQUAL(req.uri, "http://www.site.ru/news.html");
	ASSERT_EQUAL(req.body, expected_body);
}

void test_answer()
{
	HTTPHandler::Answer answer = {
		200,
		"OK",
		{
			{"Content-Type", "text/html"},
			{"Connection", "close"},
		},
		"<html>Hello</html>"
	};
	ostringstream result;
	string expected(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html>Hello</html>"
	);

	HTTPHandler::write_answer(answer, result);
	ASSERT_EQUAL(result.str(), expected);
}

int main()
{
	TestRunner tr;
	RUN_TEST(tr, test_get);
	RUN_TEST(tr, test_post);
	RUN_TEST(tr, test_answer);

	return 0;
}
