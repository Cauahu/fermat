#include <fermat/common/cmdline.h>
#include <iostream>

using std::cout;
using std::endl;
int main(int argc, char**argv) 
{
	fermat::CmdParser p;
	p.add<std::string>("host", 'h', "host name", true, "");
	p.add<int>("port", 'p', "port number", false, 80,fermat::range(1, 65535));
	p.add<std::string>("type", 't', "protocol type", false, "http", fermat::oneof<std::string>("http", "https", "ssh", "ftp"));
	p.add("gzip", 'g', "gzip when transfer");
	p.parse_check(argc, argv);

	cout << p.get<std::string>("type") << "://"
 	<< p.get<std::string>("host") << ":"
 	<< p.get<int>("port") << endl;
	if(p.exist("gzip")) {
		cout<<"compress: gzip enabled"<<endl;
	} else {
		cout<<"compress: gzip disabled"<<endl;	
	}
	return 0;
}