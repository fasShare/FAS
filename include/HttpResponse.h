#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_
#include "Handle.h"
#include "Socket.h"

class HttpResponse {

private:
	Socket rsd;
public:
	HttpResponse(int sd);
	HttpResponse() = default;
    void handle_event(Events *events);
	~HttpResponse();	
};

#endif //HTTPRESPONSE_H_

