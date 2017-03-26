// BiliData.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <time.h>


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

int main(int argc, char* argv[])
{
	auto fileStream = std::make_shared<ostream>();
	// Open stream to output file.

	pplx::task<void> requestTask = fstream::open_ostream(U("a.json")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(U("http://api.bilibili.com/"));

		// Build request URI and start the request.
		uri_builder builder(U("/index"));
		return client.request(methods::GET, builder.to_string());
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());

		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	while (1) {
		time_t rawtime;
		struct tm *info;
		time(&rawtime);
		info = gmtime(&rawtime);
		char mon = info->tm_mon;
		char day = info->tm_mday;
		char hrs = info->tm_hour;
		char min = info->tm_min;
		std::stringstream fn;
		fn << info->tm_mon << "-" << info->tm_mday << "-" << info->tm_hour+8 << "-" << info->tm_min;
		std::string fnme;
		fn >> fnme;

		try
		{
			requestTask.wait();
		}
		catch (const std::exception &e)
		{
			printf("Error exception:%s\n", e.what());
		}
		std::stringstream fn1;
		fn1 << fnme << ".json";
		std::string fnme1;
		char filename[sizeof(fnme1)];
		fn1 >> filename;
		if (!std::rename("a.json", filename))
		{
			std::cout << fnme;
		}
		_sleep(900 * 1000);
	}
	return 0;
}
