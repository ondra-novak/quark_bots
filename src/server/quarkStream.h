#pragma once

#include <shared/future.h>
#include <imtjson/value.h>

namespace quark_bots {

using ondra_shared::SharedFuture;
using json::Value;

class QuarkStream {
public:
	QuarkStream();


	struct StreamItem {
		Value data;
		SharedFuture<StreamItem> next;
	};

	SharedFuture<StreamItem> data;
};




} /* namespace quark_bots */


