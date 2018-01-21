// Telemetry.Sample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h> 

#include "Poco/Logger.h"
#include "Poco/AsyncChannel.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/AutoPtr.h"
//#include "Telemetry.h"

#include <string>
#include "Telemetry.h"

//Please note that if you use Windows XP Targeting you will need to disable conformance mode which is on by default
//in VS2017.  This is a known issue and has been posted to MS Connect as a bug. Hopefully you wont need this note in the future.

//Building and Linking:

//This Sample is provided as a statically linked library.  You can choose to compile it yourself for different configurations and versions
//of visual studio.  Pre-compiled binaries are provided in the Dependencies folder for several versions of Visual Studio.

//Get the Local ComputerName.  Used for basic logging information.
std::wstring GetLocalComputerName()
{
	poco_ndc(GetLocalComputerName());

	TCHAR computer_name[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computer_name);
	GetComputerNameW(computer_name, &size);

	return computer_name;
}

int main()
{
	//There might be a better ways to get stack information, but this one is really simple and gives you human readable information.
	//A NDC maintains a stack of context information, consisting of an informational string(method name), and source code
	//file name and line number. NDCs are especially useful for tagging log messages with context information(stack traces).
	//Every thread has its own private NDC.
	poco_ndc(main);

	//Get a smart pointer to the Stackify Channel
	Poco::AutoPtr<DefenseAgainstTheDarkArts::StackifyChannel> p_stackify(new DefenseAgainstTheDarkArts::StackifyChannel);

	//Set Stackify Api Key (Required)
	p_stackify->setProperty("apiKey", "Your api key here");

	//Set Environment (Required
	p_stackify->setProperty("environment", "development");

	//Set Server/Computer Name (Required)
	std::wstring local_computer_name = GetLocalComputerName();
	const std::string server_name(local_computer_name.begin(), local_computer_name.end());
	p_stackify->setProperty("serverName", server_name);

	//Set Application Name Property (Required)
	p_stackify->setProperty("appName", "Telemetry.Test");

	//Set Application Location (Optional)
	p_stackify->setProperty("appLocation", "C:\\Source\\GitHub\\NerdPlanet.Services\\Telemetry.Test\\Win32\\Debug");

	//Set Logger information (Required) - Name it whatever you want
	p_stackify->setProperty("logger", "defense-against-the-dark-arts-stackify-logger-pococpp-1.0.0.0");
	
	//Set Platform Language (Optional)
	p_stackify->setProperty("platform", "cpp");

	//We want to run our logger on a background thread so it is non-blocking. 
	//The Poco::AsyncChannel manages this for us by adding the stackify channel to the async channel.
	Poco::AutoPtr<Poco::AsyncChannel> p_async(new Poco::AsyncChannel(p_stackify));

	//Set the root channel
	Poco::Logger::root().setChannel(p_async);

	//Name the Logger
	Poco::Logger& logger = Poco::Logger::get("Telemetry.Sample");
	
	//Set Source
	l
	
	//Log some messages
	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Test Message #" << i;
		logger.information(message_info.str().c_str(), "Some other value");
	}

	//Log an exception
	//logger.error("Some Bad Error", "")

	p_async->close();

	Poco::Logger::shutdown();

    return 0;
}

