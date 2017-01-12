#include <unistd.h>
#include "HttpResponse.h"

HttpResponse::HttpResponse(Socket_t sd) {
  rsd = sd;
}


void HttpResponse::handle_event(Events *events) {
    (void)events;

    cout << " new httpresponse " << endl;
		
	char buf[200] = {0};
	int maxLen = 200-1;
    int ret = read(rsd, buf, maxLen);
	if (ret == 0) {
        cout << "Connection closed rsd : " << rsd  << endl;
        close(rsd);

		//FIXME : remove from Dispatcher
        //this->get_dispatcher()->del_events(this);

        cout << "delete from Dispathcer" << endl;
		return ;
	}

	if (ret == -1) {
		//FIXME : 
	}

	if (ret < maxLen) {
        cout << "recv from sd : " << rsd << " : "  <<  buf << endl;
        write(rsd, buf, ret);
	}
}


HttpResponse::~HttpResponse() {
  close(rsd);
}

