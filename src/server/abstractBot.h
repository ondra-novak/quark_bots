#pragma once
#include <couchit/document.h>
#include "order.h"

namespace quark_bots {

typedef String TradeID;
typedef Value UserID;

using namespace json;
using namespace couchit;

/** Kazdy robot ma v DB ulozeny
 * - stav (saveState())
 * - zadane prikazy
 * - aktivni pozice (mohou byt virtualni)
 */
class AbstractBotAPI {
public:

	typedef std::function<void(const AbstractOrder &)> OrderObserver;

	virtual void onOrderStateChange(OrderObserver observer) = 0;
	//virtual void onTrade(EventObserver observer) = 0;
	///Vytvori order ale neexekuuje ho - pripravi se v pameti
	virtual OrderID createOrder(const AbstractOrder &order) = 0;
	///exekuuje pripraveny ordery
	virtual void executePreparedOrders() = 0;
	///zrusi order (pripraveny nebo aktivni)
	virtual void cancelOrder(OrderID orderId) = 0;
	virtual void enumActiveOrders(const OrderObserver &) = 0;


	///asynchronni cekani na zmenu stavu orderu
	/**
	 * @param predikate obsahuje podminku kterou musi order splnovat. Pozor na to
	 * ze prepnuti do finished je posledni stav a pote se jiz order nezmeni. Z toho duvodu
	 * se predikat nevola pokud je order finished a rovnou se zavola observer
	 * @param observer funkce zavolana jakmile dojde ke kyzenemu stavu. V tom pripade
	 * se jiz dalsi event nezavola dokud neni znova zavolana funkce waitForOrderChange.
	 */
	template<typename Predicate>
	void waitForOrderChange(Predicate predikate, ObserverObserver observer) {
		onOrderStateChange([=](const AbstractOrder &o){
			if (o.isFinished() || predikate(o)) observer(o);
			else waitForOrderChange(predikate,observer);
		});
	}
	///Zavola observer jakmile order dokoncen, tedy plne exekuovan, nebo vyrazen
	/**
	 * @param order id orderu ktery potrebujeme vedet
	 * @param observer observer
	 */
	void waitForOrderFinish(OrderID &order, const OrderObserver &observer) {
		onOrderStateChange([=](const AbstractOrder &o) {
			if (o.getId() == order && o.isFinished()) observer(o);
			else waitForOrderFinish(observer);
		});
	}

};

class AbstractBot {
public:

	virtual void init(Value config, Value state) = 0;


};


}
