#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_
#include "Executor.h"
#include "Socket.h"

class HttpResponse : public Executor {

private:
	Socket rsd;
public:
	HttpResponse(int sd);
	HttpResponse() = default;
	void Executor_handle_event(Events *events);
	~HttpResponse();	
};

#endif //HTTPRESPONSE_H_

