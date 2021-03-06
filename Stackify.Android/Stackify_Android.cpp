#include "Stackify_Android.h"
#include <sys/system_properties.h>

#include <string>
#include <iostream>
#include <chrono>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std::chrono;


namespace DefenseAgainstTheDarkArts
{
	Stackify::Stackify(std::string api_key, std::string application_name, std::string environment)
	{
		api_key_ = api_key;
		environment_ = environment;
		application_name_ = application_name;
		platform_ = "cpp";
		logger_ = "defense-against-the-dark-arts-stackify-logger-pococpp-1.0.0.0";
		server_name_ = GetLocalComputerName();
		location_ = GetWorkingDirectory();

		//Sets whether server variables will be logged as data on every message.  Otherwise Server variables are only logged on exceptions
		//Server variables are kvp of misc data that you can set
		log_server_variables = false;
	}

	Stackify::~Stackify()
	{
	}

	void Stackify::Trace(std::string text)
	{
		Message msg;
		msg.text = text;
		msg.priority = TRACE;

		web::json::value json = FormatMessage(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Debug(std::string text)
	{
		Message msg;
		msg.text = text;
		msg.priority = DEBUG;

		web::json::value json = FormatMessage(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Information(std::string text)
	{
		Message msg;
		msg.text = text;
		msg.priority = INFORMATION;

		web::json::value json = FormatMessage(msg);

		utility::stringstream_t stream;
		json.serialize(stream);

		PostMessage(api_key_, json);
	}

	void Stackify::Notice(std::string text)
	{
		Message msg;
		msg.text = text;
		msg.priority = NOTICE;

		web::json::value json = FormatMessage(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Warning(std::string text, std::string source_method, int source_line)
	{
		Message msg;
		msg.text = text;
		msg.priority = WARNING;
		msg.source_method = source_method;
		msg.source_line = source_line;

		web::json::value json = FormatMessage(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Error(std::string text, std::string error_message, std::string error_type, std::string source_method, int source_line)
	{
		Exception msg;
		msg.text = text;
		msg.priority = ERROR;
		msg.source_method = source_method;
		msg.source_line = source_line;
		msg.error_message = error_message;
		msg.error_type = error_type;

		web::json::value json = FormatException(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Critical(std::string text, std::string error_message, std::string error_type, std::string source_method, int source_line)
	{
		Exception msg;
		msg.text = text;
		msg.priority = CRITICAL;
		msg.source_method = source_method;
		msg.source_line = source_line;
		msg.error_message = error_message;
		msg.error_type = error_type;

		web::json::value json = FormatException(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Fatal(std::string text, std::string error_message, std::string error_type, std::string source_method, int source_line)
	{
		Exception msg;
		msg.text = text;
		msg.priority = FATAL;
		msg.source_method = source_method;
		msg.source_line = source_line;
		msg.error_message = error_message;
		msg.error_type = error_type;

		web::json::value json = FormatException(msg);
		PostMessage(api_key_, json);
	}

	void Stackify::Log(Message& message)
	{
		web::json::value json = FormatMessage(message);
		PostMessage(api_key_, json);
	}

	void Stackify::Log(Exception& exception)
	{
		web::json::value json = FormatException(exception);
		PostMessage(api_key_, json);
	}

	web::json::value Stackify::FormatMessage(Message msg)
	{
		json::value root;
		root["Env"] = json::value::string(environment_);
		root["ServerName"] = json::value::string(server_name_);
		root["AppName"] = json::value::string(application_name_);
		root["AppLoc"] = json::value::string(location_);
		root["Logger"] = json::value::string(logger_);
		root["Platform"] = json::value::string(platform_);

		// Create a JSON message
		json::value message;
		message["Msg"] = json::value::string(msg.text);
		message["Th"] = json::value::string(msg.thread_name);

		switch (msg.priority) {
		case TRACE:
			message["Level"] = json::value::string("Trace");
			break;
		case DEBUG:
			message["Level"] = json::value::string("Debug");
			break;
		case INFORMATION:
			message["Level"] = json::value::string("Information");
			break;
		case NOTICE:
			message["Level"] = json::value::string("Notice");
			break;
		case WARNING:
			message["Level"] = json::value::string("Warning");
			break;
		case ERROR:
			message["Level"] = json::value::string("Error");
			break;
		case CRITICAL:
			message["Level"] = json::value::string("Critical");
			break;
		case FATAL:
			message["Level"] = json::value::string("Fatal");
			break;
		}

		//Get Epoch Milliseconds
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		message["EpochMs"] = json::value::number(ms.count());

		message["SrcMethod"] = json::value::string(msg.source_method);
		message["SrcLine"] = json::value::number(msg.source_line);

		//Populate Data Values
		std::stringstream data;
		for (auto it = msg.data_map.begin(); it != msg.data_map.end(); ++it)
		{
			//Example: Long string containing "Key1":"Value1", "Key2" : "Value2"
			//Todo: Escape special characters
			data << it->first;
			data << ":";
			data << it->second;

			//Check  if it is the last element otherwise add a comma
			if (++it != msg.data_map.end())
			{
				data << ",";
			}
			else
			{
				//If it is the last element check to see if server variables should be appended to the message as data
				if (log_server_variables)
				{
					if (!server_variables_.size() > 0)
					{
						//Has values so add a comma
						data << ",";
					}
				}
			}
		}

		//Append server variables to data
		if (log_server_variables)
		{
			for (auto it = server_variables_.begin(); it != server_variables_.end(); ++it)
			{
				//Nothing in data so append as normal
				data << it->first;
				data << ":";
				data << it->second;

				//Check  if it is the last element
				if (++it != server_variables_.end())
				{
					data << ",";
				}
			}
		}

		if (!data.str().empty())
		{
			message["Data"] = json::value::string(data.str());
		}

		// Create the messages array
		json::value messages;
		messages[0] = message;

		// Assign the items array as the value for the Messages key
		root["Msgs"] = messages;

		return root;
	}

	web::json::value Stackify::FormatException(Exception ex)
	{
		json::value root;
		root["Env"] = json::value::string(environment_);
		root["ServerName"] = json::value::string(server_name_);
		root["AppName"] = json::value::string(application_name_);
		root["AppLoc"] = json::value::string(location_);
		root["Logger"] = json::value::string(U("defense-against-the-dark-arts-stackify-logger-pococpp-1.0.0.0"));
		root["Platform"] = json::value::string(U("cpp"));

		// Create a JSON message
		json::value message;
		message["Msg"] = json::value::string(ex.text);
		message["Th"] = json::value::string(ex.thread_name);

		switch (ex.priority) {
		case TRACE:
			message["Level"] = json::value::string("Trace");
			break;
		case DEBUG:
			message["Level"] = json::value::string("Debug");
			break;
		case INFORMATION:
			message["Level"] = json::value::string("Information");
			break;
		case NOTICE:
			message["Level"] = json::value::string("Notice");
			break;
		case WARNING:
			message["Level"] = json::value::string("Warning");
			break;
		case ERROR:
			message["Level"] = json::value::string("Error");
			break;
		case CRITICAL:
			message["Level"] = json::value::string("Critical");
			break;
		case FATAL:
			message["Level"] = json::value::string("Fatal");
			break;
		}

		//Get Epoch Milliseconds
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		message["EpochMs"] = json::value::number(ms.count());

		message["SrcMethod"] = json::value::string(ex.source_method);
		message["SrcLine"] = json::value::number(ex.source_line);

		//Environment Detail
		json::value environment_detail;
		environment_detail["DeviceName"] = json::value::string(server_name_);
		environment_detail["AppLocation"] = json::value::string(location_);
		environment_detail["ConfiguredAppName"] = json::value::string(application_name_);
		environment_detail["ConfiguredEnvironmentName"] = json::value::string(environment_);

		//Server Variables
		if (server_variables_.size() > 0)
		{
			json::value server_variables;
			for (auto it = server_variables_.begin(); it != server_variables_.end(); ++it)
			{
				server_variables[it->first] = json::value::string(it->second);
			}

			message["ServerVariables"] = server_variables;
		}

		//Error object
		json::value error;
		error["Message"] = json::value::string(ex.error_message);
		error["ErrorType"] = json::value::string(ex.error_type);
		error["ErrorTypeCode"] = json::value::string(ex.error_type_code);
		error["SourceMethod"] = json::value::string(ex.source_method);
		error["CustomerName"] = json::value::string(ex.customer_name);
		error["UserName"] = json::value::string(ex.user_name);

		//Error Data Object
		if (ex.error_data_map.size() > 0)
		{
			json::value error_data;
			for (auto it = ex.error_data_map.begin(); it != ex.error_data_map.end(); ++it)
			{
				error_data[it->first] = json::value::string(it->second);
			}

			error["Data"] = error_data;
		}

		//StackTrace
		//Todo: Review - Should caller pass in stack trace or just generate?  Leaning toward generate for simplicity
		//json::value stack_frames;
		//int frame_count = 0;

		/*boost::stacktrace::basic_stacktrace<> stack_trace = boost::stacktrace::stacktrace();
		for (auto it = stack_trace.begin(); it != stack_trace.end(); ++it)
		{
			//Todo: Remove the first 4 sections of stack trace.  Shows this method
			//Convert to source_file to wstring
			std::string source_file = it->source_file();
			
			//Convert source line to wstring
			std::stringstream source_line;
			source_line << it->source_line();

			//Convert method to wstring
			std::string method = it->name();
			
			json::value stack_frame;
			stack_frame["CodeFileName"] = json::value::string(source_file);
			stack_frame["LineNum"] = json::value::string(source_line.str());
			stack_frame["Method"] = json::value::string(method);

			stack_frames[frame_count] = stack_frame;
			frame_count++;
		}

		error["StackTrace"] = stack_frames;*/

		//Add Environment Detail to Exception object
		json::value stackify_exception;
		stackify_exception["EnvironmentDetail"] = environment_detail;
		stackify_exception["OccurredEpochMillis"] = json::value::number(ms.count());

		stackify_exception["Error"] = error;

		//Add Exception detail to message object
		message["Ex"] = stackify_exception;

		//Todo:: Add web request details

		// Create the messages array
		json::value messages;
		messages[0] = message;

		// Assign the items array as the value for the Messages key
		root["Msgs"] = messages;

		return root;
	}

	std::string Stackify::GetLocalComputerName()
	{
		char host_name_property[PROP_VALUE_MAX + 1];
		//__system_property_get("ro.product.model", model_string);
		__system_property_get("net.hostname", host_name_property);

		std::stringstream host_name;
		host_name << host_name_property;

		return host_name.str();
	}

	//I dont like this over GetWindowsVersionEx, but that is deprecated
	//Todo: Look for more detailed version information api (Registry?)
	std::string Stackify::GetOperatingSystemVersion()
	{
		char os_version_property[PROP_VALUE_MAX + 1];
		__system_property_get("ro.product.model", os_version_property);

		std::stringstream os_version;
		os_version << os_version_property;

		return os_version.str();
	}

	std::string Stackify::GetTimezone()
	{
		time_t t = time(nullptr);
		struct tm lt = { 0 };

		localtime_r(&t, &lt);

		return lt.tm_zone;
	}

	std::string Stackify::GetWorkingDirectory()
	{
		char* buf = new char[PATH_MAX];
		errno = 0;
		char* cwd = getcwd(buf, PATH_MAX);
		
		delete[] cwd;

		std::stringstream working_directory;
		working_directory << buf;

		return working_directory.str();
	}

	pplx::task<void> Stackify::PostMessage(std::string api_key, json::value message)
	{
		try {
			// Manually build up an HTTP request with header and request URI.
			http_request request(methods::POST);

			request.headers().add("Content-Type", "application/json");
			request.headers().add("Accept", "application/json");
			request.headers().add("X-Stackify-PV", "V1");
			request.headers().add("X-Stackify-Key", api_key.c_str());

			request.set_request_uri("log/save");

			request.set_body(message);

			http_client_config config;
			config.set_proxy(web::web_proxy(web::uri("http://localhost:8888")));

			http_client client("https://api.stackify.com");

			return client.request(request).then([](web::http::http_response response)
			{
				//This is really just fire and forget.  Uncomment for debugging
				/*if (response.status_code() == status_codes::OK)
				{
				auto body = response.extract_string();
				std::wcout << L"Result: " << body.get().c_str() << std::endl;
				}*/
			});
		}
		catch (const std::exception& e)
		{
			//wcout << L"Caught exception." << endl;
		}

		
	}

	void Stackify::SetEnvironment(std::string env)
	{
		environment_ = env;
	}

	std::string Stackify::GetEnvironment()
	{
		return environment_;
	}

	void Stackify::SetApplicationName(std::string app_name)
	{
		application_name_ = app_name;
	}

	std::string Stackify::GetApplicationName()
	{
		return application_name_;
	}

	void Stackify::SetServerVariable(std::string key, std::string value)
	{
		std::map<std::string, std::string>::iterator it = server_variables_.find(key);
		if (it != server_variables_.end()) {
			it->second = value;
		}
		else {
			server_variables_.insert(std::make_pair(key, value));
		}
	}

	std::string Stackify::GetServerVariable(std::string key)
	{
		const std::map<std::string, std::string>::iterator it = server_variables_.find(key);
		if (it != server_variables_.end())
			return it->second;

		return "Key Not Found";
	}
}
