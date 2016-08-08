#ifndef SERVER_RESOURCE_HPP
#define	SERVER_RESOURCE_HPP
#define BOOST_SPIRIT_THREADSAFE
#include <boost/regex.hpp>
#include "renesolalog.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <condition_variable>
#include <assert.h>
#include "server_http.hpp"
#include "client_http.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
//Added for the default_resource example
#include<fstream>
using namespace std;
//Added for the json:
using namespace boost::property_tree;
using namespace boost::posix_time;
typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;


void defaultindex(HttpServer& server)
{
	try
	{
		server.default_resource["GET"] = [&server](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
			boost::filesystem::path web_root_path("web");
			if (!boost::filesystem::exists("web"))
				cerr << "Could not find web root." << endl;
			else {
				auto path = web_root_path;
				path += request->path;
				if (boost::filesystem::exists(path)) {
					if (boost::filesystem::canonical(web_root_path) <= boost::filesystem::canonical(path)) {
						if (boost::filesystem::is_directory(path))
							path += "/index.html";
						if (boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)) {
							ifstream ifs;
							ifs.open(path.string(), ifstream::in | ios::binary);

							if (ifs) {
								ifs.seekg(0, ios::end);
								size_t length = ifs.tellg();

								ifs.seekg(0, ios::beg);

								response << "HTTP/1.1 200 OK\r\nContent-Length: " << length << "\r\n\r\n";

								//read and send 128 KB at a time
								size_t buffer_size = 131072;
								vector<char> buffer;
								buffer.reserve(buffer_size);
								size_t read_length;
								try {
									while ((read_length = ifs.read(&buffer[0], buffer_size).gcount())>0) {
										response.write(&buffer[0], read_length);
										response.flush();
									}
								}
								catch (const exception &e) {
									cerr << "Connection interrupted, closing file" << endl;
								}

								ifs.close();
								return;
							}
						}
					}
				}
			}
			string content = "Could not open path " + request->path;
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << content.length() << "\r\n\r\n" << content;
		};


	}
	catch(exception& e) 
	{
          BOOST_LOG(test_lg::get())<<__LINE__<<": "<<e.what();
	}
}

string get_orders(string& order_num)
{
	try
	{	
		string first = "{ \"order_id\":";
		string last = ", \"customer_po\" : null, \"order_date\" : \"2015-04-16\", \"last_modified\" : \"2015-08-13\", \"ship_date\" : \"2015-04-16\", \"shipping_method\" : \"UPS Ground\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 0, \"shipping_total\" : 0, \"discount_total\" : 0, \"order_total\" : 0, \"notes\" : \"\", \"internal_notes\" : \"\", \"shipping_address\" : {			\"store_name\": null, \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", Orderbot API				November 23rd, 2015 12				\"website\" : null		}, \"billing_address\" : {				\"sales_channel\": \"DTC\", \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null			}, \"payment\" : [{					\"payment_method\": \"Paid From Web\", \"amount_paid\" : 0.1				}, { \"payment_method\": \"VOID\", \"amount_paid\" : -24.96 }, { \"payment_method\": \"Credit\", \"amount_paid\" : 24.96 }, { \"payment_method\": \"Customer Service\", \"amount_paid\" : -0.1 }], \"items\" : [{				\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 0, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 0, \"weight\" : 0.5			}], \"tracking_numbers\" : null, \"other_charges\" : null }";
		string temp=first+order_num+last;
		return temp;
	}
	catch(json_parser_error& e) 
	{
		basic_ptree<std::string, std::string> retJson;
		retJson.put<int>("errorCode",-8858);
		retJson.put<std::string>("message","json read or write error");
		retJson.put<std::string>("replyData",e.what());
		retJson.put<std::string>("replier","orderbot_simulate");

		ptime now = second_clock::local_time();  
		string now_str  =  to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());  
		//cout<<now_str<<endl;
		retJson.put<std::string>("replyTime",now_str);
        BOOST_LOG(test_lg::get())<<__LINE__<<": "<<e.what();

		std::stringstream ss;
		write_json(ss, retJson);
		return ss.str();
	}
	catch(exception& e) 
	{
		basic_ptree<std::string, std::string> retJson;
		retJson.put<int>("errorCode",-1);
		retJson.put<std::string>("message","DELETE_AREAS all from cache[KV_MF] unknown error");
		retJson.put<std::string>("replyData",e.what());
		retJson.put<std::string>("replier","orderbot_simulate");

		ptime now = second_clock::local_time();  
		string now_str  =  to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());  
		//cout<<now_str<<endl;
		retJson.put<std::string>("replyTime",now_str);
        BOOST_LOG(test_lg::get())<<__LINE__<<": "<<e.what();

		std::stringstream ss;
		write_json(ss, retJson);
		return ss.str();
	}
}

int apollo(HttpServer& server,string url)
{
	try
	{
		server.resource["^/admin/orders.json/([0-9]+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string number = request->path_match[1];
			string retString;
			/////////////////////////////////////////////
			string temp;
			request->content >> temp;
			cout << "::" << temp << endl;
			retString = get_orders(number);
			if (request->header.find("Authorization") == request->header.end())
			{
				throw std::runtime_error(std::string("need authorized"));
			}
			for (auto &i : request->header)
			{
				cout << i.first << ":" << i.second << endl;
				if (i.first == "Authorization")
				{
					cout << i.second << endl;
					string auth = "dGVzdGFwaUBvcmRlcmJvdC5jb206U2hpbnlFbGVwaGFudDIzMiM=";
					cout << i.second.compare(6, auth.length(), auth) << endl;
					if (i.second.compare(6, auth.length(), auth) != 0)
					{						
						throw std::runtime_error(std::string("Unauthorized"));
					}
						
				}
			}
			///////////////////////////////////////////
			//cout <<__LINE__<<":"<< retString << endl;
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 401 Unauthorized\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
 
server.resource["^/admin/orders.json/\\?(.+)"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string ret_string="test=test";
			string request_string = request->path_match[1];
			//cout << request_string << endl;
			
			std::vector<std::string> vector_string;
			boost::split(vector_string, request_string, boost::is_any_of("?&"));
			for (auto &i : vector_string)
			{
				cout << i << endl;
			}
			cout << "-------------------------------------------" << endl;
			ret_string = "[{\"order_id\": 2, \"customer_po\" : null, \"order_date\" : \"2015-04-16\", \"last_modified\" : \"2015-08-13\", \"ship_date\" : \"2015-04-16\", \"shipping_method\" : \"UPS Ground\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 0, \"shipping_total\" : 0, \"discount_total\" : 0, \"order_total\" : 0, \"notes\" : \"\", \"internal_notes\" : \"\", \"shipping_address\" : {\"store_name\": null, \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"billing_address\" : {\"sales_channel\": \"DTC\", \"full_name\" : \"John Smith\", \"street1\" : \"123 1st St.\", \"street2\" : \"\", \"city\" : \"San Francisco\", \"state\" : \"CA\", \"postal_code\" : \"11223\", \"country\" : \"US\", \"phone\" : \"5555555555\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"payment\" : [{\"payment_method\": \"Paid From Web\", \"amount_paid\" : 0.1}, { \"payment_method\": \"VOID\", \"amount_paid\" : -24.96 }, { \"payment_method\": \"Credit\", \"amount_paid\" : 24.96 }, { \"payment_method\": \"Customer Service\", \"amount_paid\" : -0.1 }], \"items\" : [{\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 0, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 0, \"weight\" : 0.5}], \"tracking_numbers\" : null, \"other_charges\" : null}, { \"order_id\": 4, \"customer_po\" : \"MONCLAIR 430\", \"order_date\" : \"2015-05-01\", \"last_modified\" : \"2015-05-20\", \"ship_date\" : \"2015-05-01\", \"shipping_method\" : \"Express\", \"order_status\" : \"unshipped\", \"customer_id\" : 1, \"order_tax_total\" : 50, \"shipping_total\" : 50, \"discount_total\" : 0, \"order_total\" : 2373, \"notes\" : \"\", \"internal_notes\" : \"Please quote white glove ASDF123\", \"shipping_address\" : {\"store_name\": null, \"full_name\" : \"test contact name \", \"street1\" : \"123 Will Let You Know\", \"street2\" : \"\", \"city\" : \"New Hyde Park\", \"state\" : \"NY\", \"postal_code\" : \"11040\", \"country\" : \"US\", \"phone\" : \"7788721267\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"billing_address\" : {\"sales_channel\": \"DTC\", \"full_name\" : \" \", \"street1\" : \"347 West 36th St\", \"street2\" : \"Suite 1300\", \"city\" : \"New York\", \"state\" : \"NY\", \"postal_code\" : \"10018\", \"country\" : \"US\", \"phone\" : \"\", \"fax\" : null, \"email\" : \"support@orderbot.com\", \"website\" : null}, \"payment\" : [], \"items\" : [{\"item_id\": 0, \"product_id\" : 96211, \"sku\" : \"ASDF123\", \"name\" : \"Test Product\", \"quantity\" : 1, \"unit_price\" : 2273, \"discount\" : 0, \"product_tax\" : 0, \"product_tax_total\" : 0, \"product_total\" : 2273, \"weight\" : 0.5}], \"tracking_numbers\" : null, \"other_charges\" : null }]";
				response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
server.resource["^/admin/orders.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string retString="";
			string temp;
			request->content>>temp;
			cout << "::"<<temp << endl;
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
////////////////////////////////////////////////////////////////////

server.resource["^/admin/products.json/\\?(.+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
	try {
		string ret_string = "test=test";
		string request_string = request->path_match[1];
		//cout <<request_string  << endl;
		std::vector<std::string> vector_string;
		boost::split(vector_string, request_string, boost::is_any_of("?&"));
		for (auto &i : vector_string)
		{
			 cout << i << endl;
		}
		cout << "-------------------------------------------" << endl;
		ret_string = "[{\"product_category_id\": 2, \"product_category\" : \"Rings\", \"product_group_id\" : 4, \"product_group\" : \"Bands\", \"product_id\" : 469315, \"product_name\" : \"TestAPI \", \"sku\" : \"testAPI\", \"upc\" : \"12345678\", \"cost\" : 2, \"descriptive_title\" : \"\", \"description\" : \"\", \"is_parent\" : false, \"parent_id\" : 0, \"units_of_measure\" : \"Each\", \"taxable\" : true, \"shipping_units_of_measure\" : \"Lbs\", \"weight\" : 1, \"active\" : true, \"base_price\" : 10, \"lead_times\" : null, \"variable_group_id\" : 0, \"variable_group_name\" : null, \"variable1_id\" : 0, \"variable1_name\" : null, \"variable_Value1_id\" : 0, \"variable_value1_name\" : null, \"variable2_id\" : 0, \"variable2_name\" : null, \"variable_value2_id\" : 0, \"variable_value2_name\" : null, \"custom_fields\" : [], \"tags\" : [], \"image_urls\" : [], \"visibility_on_web\" : null, \"optional_description_fields\" : {\"description1\": \"\", \"description2\" : \"\", \"description3\" : \"\", \"description4\" : \"\", \"description5\" : \"\", \"description6\" : \"\"}}, { \"product_category_id\": 2, \"product_category\" : \"Rings\", \"product_group_id\" : 3, \"product_group\" : \"With Stones\", \"product_id\" : 469316, \"product_name\" : \"Ring Stone\", \"sku\" : \"stone\", \"upc\" : \"\", \"cost\" : 3, \"descriptive_title\" : \"\", \"description\" : \"\", \"is_parent\" : false, \"parent_id\" : 0, \"units_of_measure\" : \"Each\", \"taxable\" : false, \"shipping_units_of_measure\" : \"Lbs\", \"weight\" : 1, \"active\" : true, \"base_price\" : 12, \"lead_times\" : null, \"variable_group_id\" : 0, \"variable_group_name\" : null, \"variable1_id\" : 0, \"variable1_name\" : null, \"variable_Value1_id\" : 0, \"variable_value1_name\" : null, \"variable2_id\" : 0, \"variable2_name\" : null, \"variable_value2_id\" : 0, \"variable_value2_name\" : null, \"custom_fields\" : [], \"tags\" : [], \"image_urls\" : [], \"visibility_on_webs\" : null, \"optional_description_fields\" : {\"description1\": \"test1\", \"description2\" : \"test2 \", \"description3\" : \"\", \"description4\" : \"\", \"description5\" : \"\", \"description6\" : \"\"} }]";
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
	}
	catch (json_parser_error& e)
	{
		ptree retJson;
		retJson.put("errorCode", 1111);
		retJson.put("message", "json parser error");
		retJson.put("replyData", e.what());
		retJson.put("replier", "orderbot_simulate");

		ptime now = second_clock::local_time();
		string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
		////cout<<now_str<<endl;
		retJson.put<std::string>("replyTime", now_str);
		std::stringstream ss;
		write_json(ss, retJson);
		////cout<<ss.str();
		string retString = ss.str();
		retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
		return -1;
	}
	catch (exception& e) {
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
		return -1;
	}
	catch (...) {
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
		return -1;
	}
};

server.resource["^/admin/products.json/([0-9]+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string number = request->path_match[1];
			string ret_string;
			ret_string = "{ \"product_category_id\": 2, \"product_category\": \"Rings\", \"product_group_id\": 4, \"product_group\": \"Bands\", \"product_id\": 469315, \"product_name\": \"TestAPI \", \"sku\": \"testAPI\", \"upc\": \"12345678\", \"cost\": 2, \"descriptive_title\": \"\", \"description\": \"\", \"is_parent\": false,\"parent_id\": 0, \"units_of_measure\": \"Each\", \"taxable\": true, \"shipping_units_of_measure\": \"Lbs\", \"weight\": 1, \"active\": true, \"base_price\": 10, \"lead_times\": null, \"variable_group_id\": 0, \"variable_group_name\": null, \"variable1_id\": 0, \"variable1_name\": null, \"variable_Value1_id\": 0, \"variable_value1_name\": null, \"variable2_id\": 0, \"variable2_name\": null, \"variable_value2_id\": 0, \"variable_value2_name\": null, \"custom_fields\": [], \"tags\": [], \"image_urls\": [], \"visibility_on_webs\": null, \"optional_description_fields\": { \"description1\": \"\", \"description2\": \"\", \"description3\": \"\", \"description4\": \"\", \"description5\": \"\", \"description6\": \"\" } }";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

////////////////////////////////////////////////////////////////////

server.resource["^/admin/Customers.json/\\?(.+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
	try {
		string ret_string = "test=test";
		string request_string = request->path_match[1];
		//cout <<request_string  << endl;
		std::vector<std::string> vector_string;
		boost::split(vector_string, request_string, boost::is_any_of("?&"));
		for (auto &i : vector_string)
		{
			 cout << i << endl;
		}
		cout << "-------------------------------------------" << endl;
		ret_string = "[{\"customer_id\": 2,\"account_id\": 2,\"customer_name\": \" \",\"address\": \"555\",\"address2\": \"\",\"city\": \"Vancouver\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"90210\",\"phone\": \"5555555555\",\"phone_ext\": \"\",\"sales_rep_id\": null,\"commission_rate\": 0,\"email\": \"\",\"customer_hash_id\": null,\"other_id\": \"7777777\",\"active\": true},{\"customer_id\": 3,\"account_id\": 3,\"customer_name\": \" \",\"address\": \"555\",\"address2\": \"\",\"city\": \"Vancouver\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"90210\",\"phone\": \"5555555555\",\"phone_ext\": \"\",\"sales_rep_id\": null,\"commission_rate\": 0,\"email\": \"\",\"customer_hash_id\": null,\"other_id\": null,\"active\": true},{\"customer_id\": 9,\"account_id\": 9,\"customer_name\": \"Peter Lin\",\"address\": \"122 sdgdfg\",\"address2\": \"12342\",\"city\": \"sfsdf\",\"state\": \"AL\",\"country\": \"US\",\"postal_code\": \"98001\",\"phone\": \"11111111\",\"phone_ext\": \"\",\"sales_rep_id\": 461,\"commission_rate\": 0,\"email\": \"Test@orderbot.com\",\"customer_hash_id\": null,\"other_id\": null,\"active\": true}]";
		response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
	}
	catch (json_parser_error& e)
	{
		ptree retJson;
		retJson.put("errorCode", 1111);
		retJson.put("message", "json parser error");
		retJson.put("replyData", e.what());
		retJson.put("replier", "orderbot_simulate");

		ptime now = second_clock::local_time();
		string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
		////cout<<now_str<<endl;
		retJson.put<std::string>("replyTime", now_str);
		std::stringstream ss;
		write_json(ss, retJson);
		////cout<<ss.str();
		string retString = ss.str();
		retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
		return -1;
	}
	catch (exception& e) {
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
		return -1;
	}
	catch (...) {
		response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
		return -1;
	}
};
server.resource["^/admin/customers.json/([0-9]+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string number = request->path_match[1];
			string ret_string;
			ret_string = "{  \"customer_id\": 2,  \"account_id\": 2,  \"customer_name\": \" \",  \"address\": \"555\",  \"address2\": \"\",  \"city\": \"Vancouver\",  \"state\": \"AL\",  \"country\": \"US\",  \"postal_code\": \"90210\",  \"phone\": \"5555555555\",  \"phone_ext\": \"\",  \"sales_rep_id\": null,  \"commission_rate\": 0,  \"email\": \"\",  \"customer_hash_id\": null,  \"other_id\": \"7777777\",  \"active\": true  }";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
////////////////////////////////////////////////////////////////////
server.resource["^/admin/salesreps.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[{\"sales_rep_id\": 461,\"sales_name\": \"Andrew\",\"commission\": 8,\"email_invoices\": null},{\"sales_rep_id\": 463,\"sales_name\": \"test s\",\"commission\": 0,\"email_invoices\": null},{\"sales_rep_id\": 509,\"sales_name\": \"Sales User\",\"commission\": 0,\"email_invoices\": null},{\"sales_rep_id\": 557,\"sales_name\": \"Peter\",\"commission\": 10,\"email_invoices\": null},{\"sales_rep_id\": 558,\"sales_name\": \"Sales Rep\",\"commission\": 5,\"email_invoices\": \"\"},{\"sales_rep_id\": 559,\"sales_name\": \"Anna\",\"commission\": 6,\"email_invoices\": null}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/salesreps.json/([0-9]+)$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			string number = request->path_match[1];
			string ret_string;
			ret_string = "{\"sales_rep_id\": 461,\"sales_name\":\"Andrew\",\"commission\": 8,\"email_invoices\":null}";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

////////////////////////////////////////////////////////////////////
server.resource["^/admin/account_groups.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"account_groups\" : [ { \"account_group_id\" : 18,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 33,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 34,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 91,\"account_group_name\" : \"Default\"},{ \"account_group_id\" : 92,\"account_group_name\" : \"Default\"}],\"sales_channel_id\" : 1,\"sales_channel_name\" : \"DTC\"}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
server.resource["^/admin/units_of_measurement_types.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"name\" : \"Each\",\"units_of_measure_id\" : 1},{ \"name\" : \"Pk\",\"units_of_measure_id\" : 2},{ \"name\" : \"Lbs\",\"units_of_measure_id\" : 4},{ \"name\" : \"v\",\"units_of_measure_id\" : 8}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
server.resource["^/admin/order_guides.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"name\" : \"Default\",\"order_guide_id\" : 364,\"sales_channel_id\" : 1,\"sales_channel_name\" : \"DTC\"},{ \"name\" : \"Default\",\"order_guide_id\" : 511,\"sales_channel_id\" : 2,\"sales_channel_name\" : \"Wholesale\"},{ \"name\" : \"Default\",\"order_guide_id\" : 512,\"sales_channel_id\" : 4,\"sales_channel_name\" : \"Distributors\"}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/product_structure.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"class_type_id\" : 2,\"class_type_name\" : \"Assembly\",\"product_classes\" : [ { \"categories\" : [ ],\"product_class_id\" : 26,\"product_class_name\" : \"Assembly\"},{ \"categories\" : [ ],\"product_class_id\" : 312,\"product_class_name\" : \"Assembly2\"},{ \"categories\" : [ ],\"product_class_id\" : 317,\"product_class_name\" : \"Assembly3\"}]},{ \"class_type_id\" : 1,\"class_type_name\" : \"Component\",\"product_classes\" : [ { \"categories\" : [ { \"category_id\" : 616,\"category_name\" : \"Your category1 \",\"groups\" : [ { \"group_id\" : 2631,\"group_name\" : \"Your group1 \"},{ \"group_id\" : 2632,\"group_name\" : \" Your group2\"}]} ],\"product_class_id\" : 25,\"product_class_name\" : \"Inventory 1\"},{ \"categories\" : [ ],\"product_class_id\" : 83,\"product_class_name\" : \"Inventory 2\"},{ \"categories\" : [ ],\"product_class_id\" : 311,\"product_class_name\" : \"Inventory4\"},{ \"categories\" : [ ],\"product_class_id\" : 315,\"product_class_name\" : \"Inventory5\"}]},{ \"class_type_id\" : 4,\"class_type_name\" : \"Sales\",\"product_classes\" : [ { \"categories\" : [ { \"category_id\" : 1,\"category_name\" : \"Your category1\",\"groups\" : [ { \"group_id\" : 1,\"group_name\" : \"Your group1\"},{ \"group_id\" : 2,\"group_name\" : \"Your group2\"}]},{ \"category_id\" : 2,\"category_name\" : \"Rings\",\"groups\" : [ { \"group_id\" : 3,\"group_name\" : \"With Stones\"},{ \"group_id\" : 4,\"group_name\" : \"Bands\"}]}],\"product_class_id\" : 33,\"product_class_name\" : \"Products\"} ]}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/product_variables.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"product_variables\" : [ { \"product_variable_values\" : [ { \"variable_value_id\" : 317,\"variable_value_name\" : \"XS\"},{ \"variable_value_id\" : 318,\"variable_value_name\" : \"S\"},{ \"variable_value_id\" : 319,\"variable_value_name\" : \"M\"},{ \"variable_value_id\" : 320,\"variable_value_name\" : \"L\"},{ \"variable_value_id\" : 321,\"variable_value_name\" : \"XL\"}],\"variable_id\" : 37,\"variable_name\" : \"Sizes\"},{ \"product_variable_values\" : [ { \"variable_value_id\" : 322,\"variable_value_name\" : \"Black\"},{ \"variable_value_id\" : 323,\"variable_value_name\" : \"Blue\"},{ \"variable_value_id\" : 324,\"variable_value_name\" : \"Green\"},{ \"variable_value_id\" : 325,\"variable_value_name\" : \"Red\"},{ \"variable_value_id\" : 326,\"variable_value_name\" : \"Orange\"},{ \"variable_value_id\" : 327,\"variable_value_name\" : \"Pink\"},{ \"variable_value_id\" : 328,\"variable_value_name\" : \"Purple\"},{ \"variable_value_id\" : 329,\"variable_value_name\" : \"White\"},{ \"variable_value_id\" : 330,\"variable_value_name\" : \"Yellow\"}],\"variable_id\" : 38,\"variable_name\" : \"Colours\"}],\"variable_group_id\" : 37,\"variable_group_name\" : \"General\"} ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/purchase_unit_categories.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"purchase_unit_category_id\" : 1,  \"purchase_unit_category_name\" : \"Default\"  },  { \"purchase_unit_category_id\" : 218,  \"purchase_unit_category_name\" : \"\"  }  ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/vendor_purchase_unit_of_measurements.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"vendor_purchase_unit_of_measurement_id\" : 61,  \"vendor_purchase_unit_of_measurement_name\" : \"Each\"  } ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/salesrep_groups.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"commission\" : 0.0,\"sales_rep_group_id\" : 1,\"sales_rep_group_name\" : \"Default\"} ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};
server.resource["^/admin/Vendors.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"vendor_id\" : 864,  \"vendor_name\" : \"TEST VENDOR\"  },  { \"vendor_id\" : 1840,  \"vendor_name\" : \"USD Test Vendor\"  }  ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/Countries.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"country\" : \"Canada\",\"country_id\" : 38,\"iso2\" : \"CA\",\"iso3\" : \"CAN\"},{ \"country\" : \"United States\",\"country_id\" : 226,\"iso2\" : \"US\",\"iso3\" : \"USA\"}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/States.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"abbreviation\" : \"AL\",  \"state\" : \"Alabama\",  \"state_id\" : 1  },  { \"abbreviation\" : \"AK\",  \"state\" : \"Alaska\",  \"state_id\" : 2  },  { \"abbreviation\" : \"AZ\",  \"state\" : \"Arizona\",  \"state_id\" : 3  },  { \"abbreviation\" : \"AR\",  \"state\" : \"Arkansas\",  \"state_id\" : 4  }]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/distribution_centers.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"address\" : \"Warehouse address\",\"address2\" : \"\",\"city\" : \"Warehouse city\",\"contact\" : \"warehosue contact\",\"country\" : \"US\",\"distribution_center_id\" : 1,\"distribution_center_name\" : \"Your warehouse\",\"email\" : \"warehouse@email.com\",\"fax\" : \"\",\"phone\" : \"999 - 888 - 7777\",\"postal_code\" : \"11111\",\"state\" : \"AL\",\"website\" : \"\"},{ \"address\" : \"New address \",\"address2\" : \"\",\"city\" : \"New City\",\"contact\" : \"\",\"country\" : \"CA\",\"distribution_center_id\" : 138,\"distribution_center_name\" : \"NEW DC\",\"email\" : \"\",\"fax\" : \"\",\"phone\" : \"\",\"postal_code\" : \"12345\",\"state\" : \"CA\",\"website\" : \"\"}]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

server.resource["^/admin/websites.json/$"]["GET"] = [&url](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) {
		try {
			//string number = request->path_match[1];
			string ret_string;
			ret_string = "[ { \"Website_Id\" : 35,  \"website_name\" : \"DTC Frontend\"  } ]";
			 
			//cout <<__LINE__<<":"<< retString << endl;
			ret_string.erase(remove(ret_string.begin(), ret_string.end(), '\n'), ret_string.end());
			response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret_string.length() << "\r\n\r\n" << ret_string;
			
			return 0;
		}
		catch (json_parser_error& e)
		{
			ptree retJson;
			retJson.put("errorCode", 1111);
			retJson.put("message", "json parser error");
			retJson.put("replyData", e.what());
			retJson.put("replier", "orderbot_simulate");

				ptime now = second_clock::local_time();
			string now_str = to_iso_extended_string(now.date()) + " " + to_simple_string(now.time_of_day());
			////cout<<now_str<<endl;
			retJson.put<std::string>("replyTime", now_str);
			std::stringstream ss;
			write_json(ss, retJson);
			////cout<<ss.str();
			string retString = ss.str();
			retString.erase(remove(retString.begin(), retString.end(), '\n'), retString.end());
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << retString.length() << "\r\n\r\n" << retString;
			return -1;
		}
		catch (exception& e) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
			return -1;
		}
		catch (...) {
			response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen("unknown error") << "\r\n\r\n" << "unknown error";
			return -1;
		}
	};

		return 0;
	}
	catch(exception& e) 
	{
          BOOST_LOG(test_lg::get())<<__LINE__<<": "<<e.what();
		  return -1;
	}
	catch(...) 
	{
          BOOST_LOG(test_lg::get())<<__LINE__<<": "<<"unknown error";
		  return -1;
	}
}

void serverRedisResource(HttpServer& server,string url)
{
	try
	{
		apollo(server,url);
		defaultindex(server);
	}
	catch(exception& e) 
	{
          BOOST_LOG(test_lg::get())<<__LINE__<<": "<<e.what();
	}
	catch(...) 
	{
         
	}
}


std::string&   replace_all(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
{     
    while(true)   {     
        std::string::size_type   pos(0);     
        if(   (pos=str.find(old_value))!=std::string::npos   )     
            str.replace(pos,old_value.length(),new_value);     
        else   break;     
    }     
    return   str;     
}     
std::string&   replace_all_distinct(std::string&   str,const   std::string&   old_value,const   std::string&   new_value)     
{     
    for(std::string::size_type   pos(0);   pos!=std::string::npos;   pos+=new_value.length())   {     
        if(   (pos=str.find(old_value,pos))!=std::string::npos   )     
            str.replace(pos,old_value.length(),new_value);     
        else   break;     
    }     
    return   str;     
} 
#endif	