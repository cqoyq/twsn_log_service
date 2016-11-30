/*
 * main.cpp
 *
 *  Created on: 2016-9-22
 *      Author: root
 */

#include <boost/thread.hpp>
using namespace boost;

#include <bingo/log/log_level.h>
#include <bingo/log/log_factory.h>
#include <bingo/singleton.h>
#include <bingo/RabbitMQ/rabbitmq_factory.h>
using namespace bingo;
using namespace bingo::log;

#define LOG_TAG_ERROR 	"recevie_error"
#define LOG_TAG_INFO 		"recevie_data"

typedef bingo::singleton_v0<log::log_factory> LOCAL_LOG_VISITOR_TYPE;
typedef bingo::singleton_v0<rabbitmq_factory> RABBITMQ_SERVICE_TYPE;

void rev(string& msg){
	size_t size = msg.length() + 1;
	char out[size];
	memset(out, 0x00, size);
	memcpy(out, msg.c_str(), msg.length());

	// Get data an write to log's file.
	string tag = LOG_TAG_INFO;
	string info = out;
	LOCAL_LOG_VISITOR_TYPE::instance()->handle(LOG_LEVEL_INFO, LOG_TAG_INFO, info);
}

void make_receiver(){

	log_handler* p = LOCAL_LOG_VISITOR_TYPE::instance();
	// Make rabbitmq receiver.
	if(!RABBITMQ_SERVICE_TYPE::instance()->make_p2p_receiver(p, boost::bind(rev, _1))){
		cout << "make receiver fail!, err:" << RABBITMQ_SERVICE_TYPE::instance()->err().err_message() << endl;
	}else{
		cout << "make receiver succee!" << endl;
	}
}

int main (int argc, char *argv[]) {

	// Construct local logger.
	LOCAL_LOG_VISITOR_TYPE::construct();
	if(!LOCAL_LOG_VISITOR_TYPE::instance()->make_local_logger()){
		cout << "make local logger fail! error:" << LOCAL_LOG_VISITOR_TYPE::instance()->err().err_message() << endl;
		LOCAL_LOG_VISITOR_TYPE::release();
		return 0;
	}

	// Construct rabbitmq service.
	RABBITMQ_SERVICE_TYPE::construct();

	boost::thread t(make_receiver);
	t.join();

	// Free resource.
	LOCAL_LOG_VISITOR_TYPE::release();
	RABBITMQ_SERVICE_TYPE::release();

	return 0;
}


