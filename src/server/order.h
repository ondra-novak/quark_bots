#pragma once
#include <imtjson/json.h>

namespace quark_bots {

using namespace json;

typedef String OrderID;

enum class OrderType {
	market,
	limit,
	stop,
	stoplimit,
	oco_limitstop
};

enum class OrderDir {
	buy,
	sell
};

enum class OrderState {
	validating,
	active,
	delayed,
	executed,
	canceled,
	rejected
};

class AbstractOrder {
public:

	template<typename Fn>
	static void fromJson(const Value &json, const Fn &fn);

	virtual void initFromJson(const Value &json) = 0;
	Value toJson() const {
		Object x;
		buildJson(x);
		return x;
	}

	virtual OrderType getType() const = 0;

	virtual ~AbstractOrder() {}

	virtual void buildJson(Object &obj) const = 0;

	static OrderType typeFromStr(StrViewA str);
	virtual const OrderID& getId() const ;
	virtual void generateId();
	virtual bool isFinished() const = 0;

};

class OrderBase: public AbstractOrder {
public:
		OrderBase() {}
		OrderBase(OrderType type,	OrderDir dir,double size,String tag);

		OrderDir getDir() const;

		const String& getId() const ;

		double getSize() const ;

		const String& getTag() const ;


		OrderType getType() const ;

		OrderState getState() const;

		bool isFinished() const;


		void generateId();


		virtual void initFromJson(const Value &json)override;
		virtual void buildJson(Object &obj) const override;


protected:

	OrderType type;
	OrderDir dir;
	OrderState state = OrderState::validating;
	double size;
	String tag;
	String id;
	bool finished = false;

};


class OrderMarker: public OrderBase {
public:

	OrderMarker() {}
	OrderMarker(OrderDir dir, double size, String tag);



};

class OrderLimit: public OrderBase {
public:
	OrderLimit() {}
	OrderLimit(OrderDir dir, double size, double limitPrice, String tag);

	virtual void initFromJson(const Value &json) override;
	virtual void buildJson(Object &obj) const override;

	double getLimitPrice() const;

protected:
	double limitPrice;
};



class OrderStop: public OrderBase {
public:
	OrderStop() {}
	OrderStop(OrderDir dir, double size, double limitPrice, String tag);

	virtual void initFromJson(const Value &json) override;
	virtual void buildJson(Object &obj) const override;

	double getStopPrice() const;

protected:
	double stopPrice;
};


class OrderStopLimitBase: public OrderBase {
public:
	OrderStopLimitBase() {}
	OrderStopLimitBase(OrderType type, OrderDir dir, double size, double limitPrice, double stoppPrice, String tag);

	virtual void initFromJson(const Value &json) override;
	virtual void buildJson(Object &obj) const override;

	double getLimitPrice() const;
	double getStopPrice() const;

protected:
	double limitPrice;
	double stopPrice;
};

class OrderStopLimit: public OrderStopLimitBase {
public:
	OrderStopLimit() {}
	OrderStopLimit(OrderDir dir, double size, double limitPrice, double stoppPrice, String tag);
};

class OrderOcoLimitStop: public OrderStopLimitBase {
public:
	OrderOcoLimitStop() {}
	OrderOcoLimitStop(OrderDir dir, double size, double limitPrice, double stoppPrice, String tag);

};


template<typename Fn>
inline void quark_bots::AbstractOrder::fromJson(const Value& json,	const Fn& fn) {

	OrderType t = typeFromStr(json["type"].getString());
	switch (t) {
	case OrderType::market: {
		OrderMarker o; o.initFromJson(json);fn(o);
		}break;
	case OrderType::limit: {
		OrderLimit o; o.initFromJson(json);fn(o);
		}break;
	case OrderType::stop: {
		OrderStop o; o.initFromJson(json);fn(o);
		}break;
	case OrderType::stoplimit: {
		OrderStopLimit o; o.initFromJson(json);fn(o);
		}break;
	case OrderType::oco_limitstop: {
		OrderOcoLimitStop o; o.initFromJson(json);fn(o);
		}break;
	}


}



}
