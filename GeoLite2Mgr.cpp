//
//	<one line to give the program's name and a brief idea of what it does.>
//	Copyright (C) 2019. NetDragon. All rights reserved.
//
//	Created: 2019/3/6 13:15:38
//	Version: 1.0
//  Author: WenJin Yu
//

#include "Precomp.h"
#include "GeoLite2Mgr.h"
#include <cassert>

namespace lp {

GeoLite2Mgr::GeoLite2Mgr(const char* filename, GeoLite2Logger logger)
	: filename_(filename)
	, logger_(logger)
	, status_(MMDB_FILE_OPEN_ERROR)
{
	assert(filename_);
	assert(logger_);

	status_ = MMDB_open("GeoLite2-Country.mmdb", MMDB_MODE_MMAP, &mmdb_);
	if (status_ != MMDB_SUCCESS) {
		ostringstream osslog;
		osslog << "Fail open: " << filename_ << " " << MMDB_strerror(status_);

		if (status_ == MMDB_IO_ERROR) {
			osslog << endl << "IO error: " << strerror(errno);
		}
		logger_(osslog);
	}
}

GeoLite2Mgr::~GeoLite2Mgr()
{
	if (IsOpen()) {
		MMDB_close(&mmdb_);
	}
}

string GeoLite2Mgr::LocateCountry(string ip)
{
	if (!IsOpen()) {
		ostringstream osslog;
		osslog << "DB not open: " << ip;
		logger_(osslog);
		return "";
	}

	int gai_error, mmdb_error;
	MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb_, ip.c_str(), &gai_error, &mmdb_error);

	if (0 != gai_error) {
		ostringstream osslog;
		osslog << "Error from getaddrinfo for " << ip << " " << gai_strerror(gai_error);
		logger_(osslog);
		return "";
	}

	if (MMDB_SUCCESS != mmdb_error) {
		ostringstream osslog;
		osslog << "Got an error from libmaxminddb: " << ip << " " << mmdb_error << " " << MMDB_strerror(mmdb_error);
		logger_(osslog);
		return "";
	}

	if (!result.found_entry) {
		ostringstream osslog;
		osslog << "No entry for this ip: " << ip;
		logger_(osslog);
		return "";
	}

	MMDB_entry_data_s data;
	int status = MMDB_get_value(&result.entry, &data, "country", "iso_code", NULL);
	if (status != MMDB_SUCCESS) {
		ostringstream osslog;
		osslog << "Fail get value: " << ip << " " << status << " " << MMDB_strerror(status);
		logger_(osslog);
		return "";
	}

	if (!data.has_data) {
		ostringstream osslog;
		osslog << "No data was found: " << ip;
		logger_(osslog);
		return "";
	}

	switch (data.type)
	{
	case MMDB_DATA_TYPE_UTF8_STRING:
	{
		return string(data.utf8_string, data.data_size);
	}
	default:
	{
		ostringstream osslog;
		osslog << "Invalid country value: " << ip;
		logger_(osslog);
		return "";
	}
	}
}

}
