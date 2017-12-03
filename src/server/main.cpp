/*
 * main.cpp
 *
 *  Created on: Dec 3, 2017
 *      Author: ondra
 */


#include <simpleServer/abstractService.h>
#include <rpc/rpcServer.h>
#include <shared/stringview.h>
#include <shared/ini_config.h>
#include <shared/stdLogFile.h>

namespace quark_bots {

using namespace simpleServer;
using ondra_shared::StrViewA;
using ondra_shared::IniConfig;
using ondra_shared::IniItem;
using ondra_shared::StdLogFile;
using ondra_shared::LogLevelToStrTable;
using ondra_shared::LogLevel;


static RpcHttpServer initServer(const IniConfig::KeyValueMap &cfg) {

	auto v_port = cfg.mandatory["bind"];
	auto threads = cfg["threads"];
	auto dispatchers = cfg["dispatchers"];

	NetAddr addr = NetAddr::create(v_port.getString(),0);
	return RpcHttpServer(addr, threads.getUInt(),dispatchers.getUInt());
}

static std::unique_ptr<StdLogFile> initLogFile(const IniConfig::KeyValueMap &cfg) {

	auto log = cfg["log"];
	auto logLevel = cfg["logLevel"];

	std::string logPath = log.getPath();
	LogLevelToStrTable table;
	std::unique_ptr<StdLogFile> lf(new StdLogFile(logPath,table.fromString(logLevel.getString(),LogLevel::note)));

	if (!*lf) {
		throw std::runtime_error("Failed to open log file " + logPath);
	}
	return lf;

}

int serviceMain(ServiceControl service, StrViewA name, ArgList args) {

	IniConfig cfg;
	if (args.empty()) {
		std::cerr << "Invalid arguments" << std::endl
				  << std::endl
				  << "Usage: qbots <controlFile> [start|run|restart] <configFile>" << std::endl;
				  return 1;
	}

	if (!cfg.load(args[0],[](const IniItem &directive){
		std::string err;
		if (directive.key == "include") err = "Unable to open include file: ";
		else err = "Unsupported directive: ";
		err.append(directive.key.data, directive.key.length);
		err.append(" ");
		err.append(directive.value.data, directive.value.length);
		throw std::runtime_error(err);

	})) {
		std::cerr << "Failed to load config file: " << args[0] << std::endl;
	}


	auto serverCfg = cfg["server"];

	auto logFile = initLogFile(serverCfg);
	auto rpcServer = initServer(serverCfg);
	logFile->setDefault();

	rpcServer.add_listMethods("methods");
	rpcServer.add_ping("ping");

	rpcServer.addRPCPath("/RPC",cfg["rpc"]["clientPath"].getString(),1024*1024);
	rpcServer.setHostMapping(serverCfg["hosts"].getString());
	rpcServer.start();

	service.changeUser(serverCfg["user"].getString());

	service.enableRestart();

	service.dispatch();

	return 0;


}



}



int main(int argc, char **argv) {

	return simpleServer::ServiceControl::create(
		argc, argv, "QuarkBotsService", &quark_bots::serviceMain
	);

}
