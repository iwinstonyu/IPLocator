//
//	<one line to give the program's name and a brief idea of what it does.>
//	Copyright (C) 2019. NetDragon. All rights reserved.
//
//	Created: 2019/3/6 13:15:25
//	Version: 1.0
//  Author: WenJin Yu
//

#pragma once
#include <GeoLite2/maxminddb.h>
#include <functional>
#include <sstream>
using namespace std;

namespace lp {

typedef function<void(ostringstream& osslog)> GeoLite2Logger;

class GeoLite2Mgr
{
public:
	GeoLite2Mgr(const char* filename, GeoLite2Logger logger);
	~GeoLite2Mgr();

	bool IsOpen() { return status_ == MMDB_SUCCESS; }
	string LocateCountry(string ip);

private:
	const char* filename_;
	GeoLite2Logger logger_;
	int status_;
	MMDB_s mmdb_;
};

}