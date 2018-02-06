// Stackify.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Stackify.h"
#include <boost/stacktrace.hpp>


int main()
{
	//This is the a stackify sample that uses the C++ REST SDK and the Stackify.lib
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

	DefenseAgainstTheDarkArts::Stackify stackify(L"Your api key here", L"Stackify.Sample", L"Development");
	auto json = stackify.FormatException(ex);

	stackify.PostMessage(L"3Lq3Gq2Xx9Hu4Jk9Mq7Ir9Mv2Di1Cs8Bk5Eh3Dm", json).wait();

	
    return 0;
}

