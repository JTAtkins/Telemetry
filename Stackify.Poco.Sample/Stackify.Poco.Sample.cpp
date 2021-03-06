// Telemetry.Sample.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "StackifyChannel.h"
#include <Windows.h> 

#include "Poco/Logger.h"
#include "Poco/AsyncChannel.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/AutoPtr.h"

#include <string>


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

DWORD GetOperatingSystemMajorVersion()
{
	poco_ndc(GetOperatingSystemMajorVersion);

	OSVERSIONINFOEX OSVI;
	// ReSharper disable once CppEntityAssignedButNoRead
	BOOL os_version_info_ex;

	ZeroMemory(&OSVI, sizeof(OSVERSIONINFOEX));
	OSVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!((os_version_info_ex = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&OSVI)))))
	{
		return -1; //Failed to query OS Information
	}

	return OSVI.dwMajorVersion;
}

DWORD GetOperatingSystemMinorVersion()
{
	poco_ndc(GetOperatingSystemMinorVersion);

	OSVERSIONINFOEX OSVI;
	// ReSharper disable once CppEntityAssignedButNoRead
	BOOL os_version_info_ex;

	ZeroMemory(&OSVI, sizeof(OSVERSIONINFOEX));
	OSVI.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!((os_version_info_ex = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&OSVI)))))
	{
		return -1; //Failed to query OS Information
	}

	return OSVI.dwMinorVersion;
}

std::string GetWindowsTimeZoneName()
{
	poco_ndc(GetWindowsTimeZoneName());

	const char *subkey = "SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation";
	constexpr size_t keysize{ 128 };
	HKEY key;
	char key_name[keysize]{};
	unsigned long tz_keysize = keysize;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey, 0,
		KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
	{
		if (RegQueryValueExA(key, "TimeZoneKeyName", nullptr, nullptr,
			reinterpret_cast<LPBYTE>(key_name), &tz_keysize) != ERROR_SUCCESS)
		{
			memset(key_name, 0, tz_keysize);
		}
		RegCloseKey(key);
	}

	return std::string(key_name);
}

int main()
{
	//There might be a better ways to get stack information, but this one is really just instrumentation to trace program execution.
	//A NDC maintains a stack of context information, consisting of an informational string(method name), and source code
	//file name and line number. NDCs are especially useful for tagging log messages with context information(stack traces).
	//Every thread has its own private NDC.
	poco_ndc(main);

	//Get a smart pointer to the Stackify Channel
	Poco::AutoPtr<DefenseAgainstTheDarkArts::StackifyChannel> p_stackify(new DefenseAgainstTheDarkArts::StackifyChannel);

	//Set Stackify Api Key (Required)
	p_stackify->setProperty("apiKey", "Your API Key Here");

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

	//Set Operating System
	p_stackify->setProperty("operatingSystem", "Windows");

	//Set OS Major Version
	const DWORD major_version = GetOperatingSystemMajorVersion();
	std::stringstream os_major_version;
	os_major_version << major_version;
	p_stackify->setProperty("osMajorVersion", os_major_version.str());

	//Set OS Minor Version
	const DWORD minor_version = GetOperatingSystemMinorVersion();
	std::stringstream os_minor_version;
	os_minor_version << minor_version;
	p_stackify->setProperty("osMinorVersion", os_minor_version.str());

	const std::string tz = GetWindowsTimeZoneName();
	p_stackify->setProperty("timezone", tz);

	//We want to run our logger on a background thread so it is non-blocking. 
	//The Poco::AsyncChannel manages this for us by adding the stackify channel to the async channel.
	Poco::AutoPtr<Poco::AsyncChannel> p_async(new Poco::AsyncChannel(p_stackify));

	//Set the root channel
	Poco::Logger::root().setChannel(p_async);

	//Name the Logger
	Poco::Logger& logger = Poco::Logger::get("Telemetry.Sample");
	
	
	//Log some simple messages.
	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Information #" << i;
		logger.information(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Warning #" << i;
		logger.warning(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Debug #" << i;
		logger.debug(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Notice #" << i;
		logger.notice(message_info.str().c_str());
	}

	for (int i = 0; i < 10; ++i)
	{
		std::stringstream message_info;
		message_info << "Trace #" << i;
		logger.trace(message_info.str().c_str());
	}

	//If you want to log more detail you can create the message object yourself and add additional key/value pairs like the exception below.
	//Log an exception
	Poco::Message message_exception;
	message_exception.setPriority(Poco::Message::PRIO_FATAL);
	message_exception.setSourceLine(88);
	message_exception.setSourceFile("TelemetrySample.cpp");

	message_exception.setText("Exception Message.  Something really bad happened."); //Main text of the message that is logged

	//Log Additional information by key value pair (You will need to modify telemety.cpp to read and write this key/value pair to stackify
	message_exception.set("error_message", "NullPointerException: Something important was null that can't be null (Example error message with exception details)");
	message_exception.set("error_type", "NullPointerException");
	message_exception.set("source_method", "main()");

	//Dump Stack Trace (if tracking with NDC)
	const int stack_length = Poco::NDC::current().depth();
	std::stringstream stack_depth;
	stack_depth << stack_length;

	std::stringstream stack_trace;
	Poco::NDC::current().dump(stack_trace);
	message_exception.set("stack_trace", stack_trace.str());

	logger.log(message_exception);
	
	p_async->close();

	Poco::Logger::shutdown();

    return 0;
}

