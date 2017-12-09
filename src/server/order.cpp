#include "order.h"
#include <imtjson/namedEnum.h>
#include <couchit/defaultUIDGen.h>

namespace quark_bots {


json::NamedEnum<OrderType> orderTypes ({
		{OrderType::market, "market"},
		{OrderType::limit, "limit"},
		{OrderType::stop, "stop"},
		{OrderType::stoplimit, "stoplimit"},
		{OrderType::oco_limitstop, "oco_limitstop"}
});

json::NamedEnum<OrderDir> orderDirs ({
		{OrderDir::buy, "buy"},
		{OrderDir::sell, "sell"},
});

json::NamedEnum<OrderState> orderStates ({
		{OrderState::validating, "validating"},
		{OrderState::active, "active"},
		{OrderState::delayed, "delayed"},
		{OrderState::executed, "executed"},
		{OrderState::canceled, "canceled"},
		{OrderState::rejected, "rejected"},
});


OrderBase::OrderBase(OrderType type, OrderDir dir, double size, String tag)
:type(type),dir(dir),size(size),tag(tag)

{
}

void OrderBase::initFromJson(const Value& json) {
	finished = json["finished"].getBool();
	state = orderStates[json["status"].getString()];
	type = orderTypes[json["type"].getString()];
	dir = orderDirs[json["dir"].getString()];
	size = json["size"].getNumber();
	id = json["id"].getString();
	tag = json["data"]["tag"].getString();
}

void OrderBase::generateId() {
	if (!id.empty()) {
		couchit::DefaultUIDGen &gen = couchit::DefaultUIDGen::getInstance();
		id = gen("o.");
	}
}

OrderDir OrderBase::getDir() const {
}

const String& OrderBase::getId() const {
}

double OrderBase::getSize() const {
}

const String& OrderBase::getTag() const {
}

OrderType OrderBase::getType() const {
}

OrderState OrderBase::getState() const {
}

bool OrderBase::isFinished() const {
}

void OrderBase::buildJson(Object& obj) const {
	obj.set("id",id)
		   ("dir",orderDirs[dir])
		   ("type",orderTypes[type])
		   ("size",size);
	obj.object("data").set("tag",tag);
}

OrderMarker::OrderMarker(OrderDir dir, double size, String tag)
	:OrderBase(OrderType::market, dir, size, tag)
{

}

OrderLimit::OrderLimit(OrderDir dir, double size, double limitPrice, String tag)
	:OrderBase(OrderType::limit, dir, size, tag)
	,limitPrice(limitPrice)
{
}

void OrderLimit::initFromJson(const Value& json) {
	OrderBase::initFromJson(json);
	limitPrice = json["limitPrice"].getNumber();;
}

void OrderLimit::buildJson(Object& obj) const {
	OrderBase::buildJson(obj);
	obj.set("limitPrice",limitPrice);
}

OrderStop::OrderStop(OrderDir dir, double size, double limitPrice, String tag)
:OrderBase(OrderType::limit, dir, size, tag)
,stopPrice(stopPrice)

{
}

void OrderStop::initFromJson(const Value& json) {
	OrderBase::initFromJson(json);
	stopPrice = json["stopPrice"].getNumber();;
}

void OrderStop::buildJson(Object& obj) const {
	OrderBase::buildJson(obj);
	obj.set("stopPrice",stopPrice);
}

double OrderLimit::getLimitPrice() const {
	return limitPrice;
}

double OrderStop::getStopPrice() const {
	return stopPrice;
}



OrderStopLimitBase::OrderStopLimitBase(OrderType type, OrderDir dir,
		double size, double limitPrice, double stoppPrice, String tag)
:OrderBase(type,dir,size,tag)
,limitPrice(limitPrice)
,stopPrice(stopPrice)
{
}

void OrderStopLimitBase::initFromJson(const Value& json) {
	OrderBase::initFromJson(json);
	limitPrice = json["limitPrice"].getNumber();;
	stopPrice = json["stopPrice"].getNumber();;
}

void OrderStopLimitBase::buildJson(Object& obj) const {
	OrderBase::buildJson(obj);
	obj.set("limitPrice",limitPrice);
	obj.set("stopPrice",stopPrice);
}

double OrderStopLimitBase::getLimitPrice() const {
	return limitPrice;
}


double OrderStopLimitBase::getStopPrice() const {
	return stopPrice;
}

OrderStopLimit::OrderStopLimit(OrderDir dir, double size,
		double limitPrice, double stoppPrice, String tag)
	:OrderStopLimitBase(OrderType::stoplimit,dir,size,limitPrice,stoppPrice,tag)
{
}

OrderOcoLimitStop::OrderOcoLimitStop(OrderDir dir, double size,
		double limitPrice, double stoppPrice, String tag)
	:OrderStopLimitBase(OrderType::oco_limitstop,dir,size,limitPrice,stoppPrice,tag)
{
}

OrderType quark_bots::AbstractOrder::typeFromStr(StrViewA str) {
	return orderTypes[str];
}

}

