#include <unistd.h>
#include "HttpResponse.h"

HttpResponse::HttpResponse(int sd) {
  rsd = sd;
}


void HttpResponse::Executor_handle_event(Events *events) {


	cout << "new httpresponse" << endl;
		
	char buf[200] = {0};
	int maxLen = 200-1;
	int ret = read(rsd.Socket_get_sd(), buf, maxLen);
	if (ret == 0) {
		cout << "Connection closed rsd : " << rsd.Socket_get_sd()  << endl;
		rsd.Socket_close();

		//FIXME : remove from Dispatcher

		return ;
	}

	if (ret == -1) {
		//FIXME : 
	}

	if (ret < maxLen) {
		cout << "recv from sd : " << rsd.Socket_get_sd() << " : "  <<  buf << endl;
		write(rsd.Socket_get_sd(), buf, ret);
	}
}


HttpResponse::~HttpResponse() {
	rsd.Socket_close();
}

