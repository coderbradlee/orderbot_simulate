
#include "serverResource.hpp"


int main() {
	try
	{

		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini("config.ini", pt);
		unsigned short port = boost::lexical_cast<unsigned short>(pt.get<std::string>("webserver.port"));
		size_t threads = boost::lexical_cast<size_t>(pt.get<std::string>("webserver.threads"));
		string url = pt.get<std::string>("webserver.url");

		//HTTP-server at port 8080 using 4 threads
		HttpServer server(port, threads);
		//serverResource(server);
		serverRedisResource(server, url);
		thread server_thread([&server](){
			//Start server
			server.start();
		});


		/////////test http client start ////////////////////////////////////////////////////////////////////////////
		//Wait for server to start so that the client can connect
		this_thread::sleep_for(chrono::seconds(3));
		HttpClient client("localhost:8688");
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/orders.json/?created_at_min=2015-01-01&limit=200&page=1&order_status=unconfirmed,unshipped,to_be_shipped&Sales_channels=dtc,wholesale", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -G -d "created_at_min=2015-01-01&limit=200&page=1&order_status=unconfirmed,unshipped,to_be_shipped&Sales_channels=dtc,wholesale" http://172.18.100.85:8688/admin/orders.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/orders.json/123", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -u testapi@orderbot.com:ShinyElephant232# -X GET http://172.18.100.85:8688/admin/orders.json/123 			
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/products.json/?class_type=sales&category_name=Rings", ss);
			//cout << r2->content.rdbuf() << endl;

			//curl -G -d "class_type=sales&category_name=Rings" http://172.18.100.85:8688/admin/products.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/products.json/429229", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/products.json/429229
		}
		{
			//GET / admin / Customers.json / ? active = true & sales_channel_name = wholesale
			string json = "nothing";

			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/Customers.json/?active=true&sales_channel_name=wholesale", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -G -d "active=true&sales_channel_name=wholesale" http://172.18.100.85:8688/admin/Customers.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/customers.json/2", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/customers.json/2
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/salesreps.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/salesreps.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/salesreps.json/461", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/salesreps.json/461
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/account_groups.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/account_groups.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/units_of_measurement_types.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/units_of_measurement_types.json/
		}


		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/order_guides.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/order_guides.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/product_structure.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/product_structure.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/product_variables.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/product_variables.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/purchase_unit_categories.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/purchase_unit_categories.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/vendor_purchase_unit_of_measurements.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/vendor_purchase_unit_of_measurements.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/salesrep_groups.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/salesrep_groups.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/Vendors.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/Vendors.json/
		}

		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/Countries.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/Countries.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/States.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/States.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/distribution_centers.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -u testapi@orderbot.com:ShinyElephant232# -X GET http://172.18.100.85:8688/admin/distribution_centers.json/
		}
		{
			string json = "nothing";
			stringstream ss(json);
			auto r2 = client.request("GET", "/admin/websites.json/", ss);
			//cout << r2->content.rdbuf() << endl;
			//curl -X GET http://172.18.100.85:8688/admin/websites.json/
		}



		server.initsink->flush();
		server_thread.join();

		//this_thread::sleep_for(chrono::seconds(500));
	}
	catch (std::exception& e) {
		//cout << diagnostic_information(e) << endl;
		cout << e.what() << endl;
        }
	catch(...) 
	{
         
	}
    return 0;
}