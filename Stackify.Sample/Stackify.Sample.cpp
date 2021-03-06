// Stackify.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Stackify.h"
#include <boost/stacktrace.hpp>


int main()
{
	//This is the a stackify sample that uses the C++ REST SDK and the Stackify.lib
	DefenseAgainstTheDarkArts::Stackify stackify_logger(L"Your api key here.", L"Stackify.Sample", L"Development");
	
	//Log some simple messages.
	for (int i = 0; i < 10; ++i)
	{
		std::wstringstream message_info;
		message_info << L"Information #" << i;
		stackify_logger.Information(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::wstringstream message_info;
		message_info << "Warning #" << i;
		stackify_logger.Warning(message_info.str().c_str(), L"main", 123);
	}

	for (int i = 0; i < 10; ++i)
	{
		std::wstringstream message_info;
		message_info << L"Debug #" << i;
		stackify_logger.Debug(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::wstringstream message_info;
		message_info << L"Notice #" << i;
		stackify_logger.Notice(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::wstringstream message_info;
		message_info << L"Trace #" << i;
		stackify_logger.Trace(message_info.str().c_str());
	}

	//Log Some Exceptions
	stackify_logger.Error(L"Some Error", L"Something is null and it shouldnt be", L"NULL Reference Exception", L"main", 222);
	stackify_logger.Critical(L"Some Error", L"Something is null and it shouldnt be", L"NULL Reference Exception", L"main", 222);
	stackify_logger.Fatal(L"Some Error", L"Something is null and it shouldnt be", L"NULL Reference Exception", L"main", 222);

	//You can also log by sending in an exception or the message object
	DefenseAgainstTheDarkArts::Exception ex;
	ex.text = L"Sample Error Message";
	ex.error_message = L"Something null";
	ex.error_type = L"Null Reference Exception";
	ex.source_method = L"main";
	ex.source_line = 11;
	ex.error_type_code = L"1234";
	ex.transaction_id = L"{1345}";
	ex.thread_name = L"main";
	ex.priority = ERROR;

	stackify_logger.Log(ex);

	Sleep(10000);
    return 0;
}

