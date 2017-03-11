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

const time_t t = time(NULL);

int main(int argc, char* argv[])
{
	auto fileStream = std::make_shared<ostream>();
	struct tm* current_time = localtime(&t);
	std::string tim = "\"tm_day\", \"tm_hour\", \"tm_min\"";
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
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}

	return 0;
}
