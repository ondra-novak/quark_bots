#pragma once

namespace quark_bots {


typedef json::String TradeID;
typedef json::String OrderID;
typedef json::Value UserID;


class Trade {
public:

	TradeID id;

	struct Side {
		UserID u;
		OrderID o;
		bool margin;
	};

	Side buyer, seller;
	double price;
	double size;

	void fromJson(json::Value json);

};

///informace o balanci
struct BalanceInfo {
	///celkova suma assetu
	double asset;
	///celkova suma currency
	double currency;

};

///Informace o tradu posilana robotovy
class TradeBalanceChange {
public:

	///ID tradu pro identifikaci
	TradeID id;
	///ID orderu ktery se tradu ucastni
	OrderID o;
	///tag orderu - slouzi k evidenci pozic
	json::String tag;
	///aktualni cena obchodu
	double price;
	///zmena balance na uctu
	BalanceInfo change;
	///akumulovany stav balance na uctu
	BalanceInfo total;

};

}
