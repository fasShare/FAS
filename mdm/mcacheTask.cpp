#include "mcacheTask.h"
#include <Log.h>
#include <utils/utilstring.h>
#include <NetAddress.h>
#include <Socket.h>
#include <Poll.h>
#include <Events.h>
#include <utils/utilfile.h>
#include <TcpConnection.h>
#include <unistd.h>
#include <list>

#include <boost/algorithm/string.hpp>

fas::mdm::mcacheTask::mcacheTask(hashSPtr hash) :
	taskState_(TaskState::BEGIN_NEW_TASK),
	head_(""),
	headerItems_(),
	method_(""),
	key_(""),
	flag_(""),
	exptime_(""),
	bytes_(0),
	hasReadBytes_(0),
	hash_(hash),
	server_(nullptr),
	protocol_() {
}

void fas::mdm::mcacheTask::renew() {
	taskState_ = TaskState::BEGIN_NEW_TASK;
	head_ = "";
	headerItems_.clear();
	method_ = "";
	key_ = "";
	keys_.clear();
	flag_ = "";
	exptime_ = "";
	bytes_ = 0;
	cas_unique_ = "";

	noreply_ = "";

	hasReadBytes_ = 0;

	server_ = nullptr;
}

bool fas::mdm::mcacheTask::taskCommunition(TcpConnShreadPtr conn, fas::Buffer *buffer) {
	LOGGER_TRACE("mcacheTask::taskCommunition");

	const char* end = NULL;

	if (getState() == TaskState::BEGIN_NEW_TASK) {
		LOGGER_TRACE("TaskState::BEGIN_NEW_TASK");
		end = buffer->findCRLF();
		if (end == NULL) {
			return false;
		}
		setState(TaskState::PARSE_PROTOCOL);
	}

	if (getState() == TaskState::PARSE_PROTOCOL) {
		LOGGER_TRACE("TaskState::PARSE_PROTOCOL");
		end = buffer->findCRLF();
		head_ = buffer->retrieveAsString(end - buffer->peek());
		buffer->retrieve(2); // 去掉首部的\r\n

		std::vector<std::string> strstemp;
		boost::split(strstemp, head_, boost::is_any_of(" "));

		for (auto iter = strstemp.begin(); iter != strstemp.end(); ++iter) {
			if (!iter->empty()) {
				headerItems_.push_back(*iter);
			}
		}

		LOGGER_TRACE("item :" << headerItems_.size());
		if (headerItems_.empty()) { //the head is empty.
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		if (!protocol_.inCmdSet(headerItems_[0])) { //the cmd is invaild.
			LOGGER_TRACE("The cmd is invaild.");
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		setState(TaskState::BEGIN_HANDLE_CMD);
	}
	LOGGER_TRACE("TaskState::BEGIN_HANDLE_CMD");
	if (protocol_.isStorageCommand(headerItems_[0])) {
		return handleStrogeCmd(conn, buffer);
	} else if (protocol_.isRetrievalCommand(headerItems_[0])) {
		return handleRetriveCmd(conn, buffer);
	} else if (protocol_.isDeleteCommand(headerItems_[0])) {
		return handleDeleteCmd(conn, buffer);
	} else if (protocol_.isIncrDecrCommand(headerItems_[0])) {
		return handleIncrDecrCmd(conn, buffer);
	} else if (protocol_.isOtherCommand(headerItems_[0])) {
		return handleOtherCmd(conn, buffer);
	} else {
		conn->sendString("ERROR\r\n");
		renew();
		return false;
	}

	return true;
}

bool fas::mdm::mcacheTask::handleOtherCmd(TcpConnShreadPtr conn, fas::Buffer *buffer) {
	if (getState() == TaskState::BEGIN_HANDLE_CMD) {
		method_ = headerItems_[0];
		key_ = "default key";
		//touch <key> <exptime> [noreply]\r\n
		//该命令用来更新数据的过期时间（不致数据因长时间未访问而过期）。
		if (method_ == "quit") {
			setState(TaskState::BAD);
			return false;
		} else if (method_ == "touch") {
			if ((headerItems_.size() != 3) && (headerItems_.size() != 4)) {
				conn->sendString("ERROR\r\n");
				renew();
				return false;
			}
			if (headerItems_.size() == 4) {
				noreply_ = headerItems_[4];
			}

			key_ = headerItems_[1];
		}

		fas::Socket sock(AF_INET, SOCK_STREAM, 0);

		if (!getSocketFromKey(key_, sock)) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		LOGGER_TRACE("connect succeed!");

		head_ = head_ + "\r\n";
		const char *buf = head_.data();

		ssize_t ret = writeSizeBytes2NoBlockSock(sock.getSocket(), buf, head_.size());
		if (ret == -1 || (ret != static_cast<int>(head_.size()))) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}


		LOGGER_TRACE("send header succeed!");

		if (noreply_ != protocol_.getNoreply()) {
			std::string retString;
			ret = readUntilStrWithEnd(sock.getSocket(), retString, "\r\n");
			if (ret <= 0) {
				setState(TaskState::BAD);
				conn->sendString("SERVER_ERROR\r\n");
				return false;
			}

			LOGGER_TRACE("after read response : " << retString);
			conn->sendData(retString.c_str(), ret + ::strlen("\r\n"));
		}
	}

	renew();
	return true;
}

bool fas::mdm::mcacheTask::handleIncrDecrCmd(TcpConnShreadPtr conn, fas::Buffer *buffer) {
	//  incr <key> <value> [noreply]"r"n
	//  decr <key> <value> [noreply]"r"n
	//  <key>是客户端要修改数据项的关键字
	//  <value>是对该数据行进行增加或者减少的操作数。它是一个32位的无符号整数。
	//注意到“decr”命令的下溢问题，如果客户端尝试减少的数量小于0，其结果是0。“incr”命令的溢出问题没有检查。另外减少一个数据而使它减少了长度，但不保证减少它返回时的长度。该数字可能是附加空格的数字，但这只是实现的优化，所以你不能相信它。
	if (getState() == TaskState::BEGIN_HANDLE_CMD) {
		if ((headerItems_.size() != 3) && (headerItems_.size() != 4)) {
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		if (headerItems_.size() == 4) { //incr, decr的noreply和delete的noreply处理方式不一样.
			noreply_ = headerItems_[3];
		}

		if (std::atoi(headerItems_[2].c_str()) < 0) { // value can't be negative
			if (noreply_ != protocol_.getNoreply()) {
				conn->sendString("CLIENT_ERROR invalid numeric delta argument\r\n");
			}
			renew();
			return false;
		}

		key_ = headerItems_[1];

		fas::Socket sock(AF_INET, SOCK_STREAM, 0);

		if (!getSocketFromKey(key_, sock)) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		LOGGER_TRACE("connect succeed!");

		head_ = head_ + "\r\n";
		const char *buf = head_.data();

		ssize_t ret = writeSizeBytes2NoBlockSock(sock.getSocket(), buf, head_.size());
		if (ret == -1 || (ret != static_cast<int>(head_.size()))) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}


		LOGGER_TRACE("send header succeed!");

		if (noreply_ != protocol_.getNoreply()) {//不同于delete和stroage cmds.
			std::string retString;
			ret = readUntilStrWithEnd(sock.getSocket(), retString, "\r\n");
			if (ret <= 0) {
				setState(TaskState::BAD);
				conn->sendString("SERVER_ERROR\r\n");
				return false;
			}

			LOGGER_TRACE("after read response : " << retString);
			conn->sendData(retString.c_str(), ret + ::strlen("\r\n"));
		}

	}
	renew();
	return true;
}

bool fas::mdm::mcacheTask::handleDeleteCmd(TcpConnShreadPtr conn, fas::Buffer *buffer) {
	//  delete <key> [noreply]\r\n
	//  <key>是客户端希望服务器删除数据项的关键字
	if (getState() == TaskState::BEGIN_HANDLE_CMD) {
		if (headerItems_.size() <= 1) {
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		if ((headerItems_.size() != 2) && (headerItems_.size() != 3)) {
			conn->sendString("CLIENT_ERROR bad command line format.  Usage: delete <key> [noreply]\r\n");
			renew();
			return false;
		}

		if (headerItems_.size() == 3) {
			if (headerItems_[2] != protocol_.getNoreply()) {
				conn->sendString("CLIENT_ERROR bad command line format.  Usage: delete <key> [noreply]\r\n");
				renew();
				return false;
			}
			noreply_ = headerItems_[2];
		}

		key_ = headerItems_[1];

		fas::Socket sock(AF_INET, SOCK_STREAM, 0);

		if (!getSocketFromKey(key_, sock)) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		head_ = head_ + "\r\n";
		const char *buf = head_.data();

		ssize_t ret = writeSizeBytes2NoBlockSock(sock.getSocket(), buf, head_.size());
		if (ret == -1 || (ret != static_cast<int>(head_.size()))) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}


		LOGGER_TRACE("send header succeed!");

		if (noreply_ != protocol_.getNoreply()) {
			std::string retString;
			ret = readUntilStrWithEnd(sock.getSocket(), retString, "\r\n");
			if (ret <= 0) {
				setState(TaskState::BAD);
				conn->sendString("SERVER_ERROR\r\n");
				return false;
			}

			LOGGER_TRACE("after read response : " << retString);
			conn->sendData(retString.c_str(), ret + ::strlen("\r\n"));
		}
	}

	renew();
	return true;
}

bool fas::mdm::mcacheTask::handleRetriveCmd(TcpConnShreadPtr conn, fas::Buffer *buffer) {
	if (getState() == TaskState::BEGIN_HANDLE_CMD) {
		//get/gets 至少是两个命令，get <key>\r\n,只有一个get视为错误.
		if (headerItems_.size() <= 1) {
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		method_ = headerItems_[0];

		for (auto iter = headerItems_.begin() + 1; iter != headerItems_.end(); ++iter) {
			keys_.push_back(*iter);
		}

		if (keys_.size() == 0) {
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		int countGetSucceed = 0;
		std::vector<serNodeSptr> gets_servers_v;
		std::map<int, std::string> gets_socks_v;
		fas::Poll poller;
		std::vector<fas::Events> rEvent;

		// 由于不同的key,可能分别存在不同的服务器上，所以要分别进行get key\r\n,最后只返回一个END\r\n
		for (auto itk = keys_.begin(); itk != keys_.end(); ++itk) {
			auto serverTemp = hash_->getMappingNodeFromKeyString(*itk);
			if (!serverTemp) {
				conn->sendString("SERVER_ERROR\r\n");
				setState(TaskState::BAD);
				return false;
			}
			gets_servers_v.push_back(serverTemp);

			//　在没有连接池的情况下
			fas::NetAddress addr(AF_INET, serverTemp->getPort(), serverTemp->getIp().c_str());
			fas::Socket sock(AF_INET, SOCK_STREAM, 0);
			sock.setNoBlocking();
			sock.setExecClose();
			if (!sock.connect(addr)) {
				continue;
			}
			LOGGER_TRACE("ip : " << serverTemp->getIp() << " port : " \
					<< serverTemp->getPort());

			gets_socks_v.insert({sock.getSocket(), *itk});
			fas::Events event(sock.getSocket(), fas::kWriteEvent|fas::kReadEvent);

			poller.EventsAdd(&event);
		}

		if (gets_socks_v.size() == 0) { //说明没有一个socket可用。
			conn->sendString("SERVER_ERROR\r\n");
			setState(TaskState::BAD);
			return false;
		}

		int timeout = 1000;

		poller.Loop(rEvent, timeout);

		if(rEvent.size() <= 0) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		LOGGER_TRACE("after pollloop");

		for (auto iter = rEvent.begin(); iter != rEvent.end(); ++iter) {
			if ((*iter).containsEvents(fas::kReadEvent) &&\
					(*iter).containsEvents(fas::kWriteEvent)) {
				int error = 0;
				socklen_t len = 0;

				getsockopt((*iter).getFd(), SOL_SOCKET, SO_ERROR, \
						reinterpret_cast<char *>(&error), &len);
				if (error != 0) {
					setState(TaskState::BAD);
					conn->sendString("SERVER_ERROR\r\n");
					return false;
				}
			}

			LOGGER_TRACE("connect succeed!");

			std::string reqHead = method_ + " " +gets_socks_v.find((*iter).getFd())->second + "\r\n";
			const char *buf = reqHead.data();

			ssize_t ret = writeSizeBytes2NoBlockSock((*iter).getFd(), buf, reqHead.size());
			if (ret == -1 || (ret != static_cast<int>(reqHead.size()))) {
				continue;
			}

			LOGGER_TRACE("send header succeed!");

			std::string retString;
			ret = readUntilStrWithEnd((*iter).getFd(), retString, "END\r\n");
			if (ret <= 0) {
				continue;
			}

			LOGGER_TRACE("after read response : " << retString);
			conn->sendData(retString.data(), ret);

			++countGetSucceed; //又有一个成功。
		} // for

		conn->sendString("END\r\n");

	}

	renew();
	return true;
}

bool fas::mdm::mcacheTask::handleStrogeCmd(TcpConnShreadPtr conn, fas::Buffer *buffer) {

	if (getState() == TaskState::BEGIN_HANDLE_CMD) {
		//cas 命令最多７个参数
		if ((headerItems_.size() != 5) && (headerItems_.size() != 6) && (headerItems_.size() != 7)) {
			LOGGER_ERROR(" head args size : " << headerItems_.size());
			conn->sendString("ERROR\r\n");
			renew();
			return false;
		}

		method_ = headerItems_[0];
		key_ = headerItems_[1];
		flag_ = headerItems_[2];
		exptime_ = headerItems_[3];
		bytes_ = std::atoi(headerItems_[4].c_str());
		if (method_ == "cas") {
			//stroage command 最少5个参数，cas 最少６个参数
			if (headerItems_.size() <= 5) {
				LOGGER_ERROR(" head args size : " << headerItems_.size());
				conn->sendString("ERROR\r\n");
				renew();
				return false;
			}
			cas_unique_ = headerItems_[5];
			if (headerItems_.size() == 7) {
				noreply_ = headerItems_[6];
			}

		} else {
			if (headerItems_.size() == 6) {
				noreply_ = headerItems_[5];
			}
		}

		LOGGER_TRACE(method_ << " " << key_ << " " << flag_ << " "\
				<< exptime_ << " " << bytes_);

		setState(TaskState::WAIT_ENOUGH_DATA);
	}


	if (getState() == TaskState::WAIT_ENOUGH_DATA) {
		// 暂时只在数据读完还进行后续操作。
		if (buffer->readableBytes() < getBytes() + strlen("\r\n")) {
			return true;
		}
		std::string checkend(buffer->peek(), getBytes(), strlen("\r\n"));
		if (strcmp(checkend.c_str(), "\r\n") != 0) {
			conn->sendString("CLIENT_ERROR bad data chunk\r\n");
			conn->sendString("ERROR\r\n");
			buffer->retrieveAll();
			renew();
			return false;
		}

		setState(TaskState::GET_ALL_DATA);
	}


	if (getState() == TaskState::GET_ALL_DATA) {
		fas::Socket sock(AF_INET, SOCK_STREAM, 0);

		if (!getSocketFromKey(key_, sock)) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		LOGGER_TRACE("connect succeed!");

		head_ = head_ + "\r\n";
		const char *buf = head_.c_str();

		ssize_t ret = writeSizeBytes2NoBlockSock(sock.getSocket(), buf, head_.size());
		if (ret == -1 || (ret != static_cast<int>(head_.size()))) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}


		LOGGER_TRACE("after write head : " << head_ );

		ret = writeSizeBytes2NoBlockSock(sock.getSocket(), buffer->peek(), \
				buffer->readableBytes());
		if (ret == -1 || (ret != static_cast<int>(buffer->readableBytes()))) {
			setState(TaskState::BAD);
			conn->sendString("SERVER_ERROR\r\n");
			return false;
		}

		LOGGER_TRACE("after write body : " << buffer->retrieveAllAsString());
		buffer->retrieveAll();

		if (noreply_ != protocol_.getNoreply()) {
			std::string retString;
			ret = readUntilStrWithEnd(sock.getSocket(), retString, "\r\n");
			if (ret <= 0) {
				setState(TaskState::BAD);
				conn->sendString("SERVER_ERROR\r\n");
				return false;
			}

			LOGGER_TRACE("after read response : " << retString);
			conn->sendData(retString.c_str(), ret + ::strlen("\r\n"));
		}

	}

	renew();
	return true;
}

bool fas::mdm::mcacheTask::getSocketFromKey(const string& key, fas::Socket& sock) {
	server_ = hash_->getMappingNodeFromKeyString(key_);
	if (!server_) {
		return false;
	}
	//　在没有连接池的情况下
	fas::NetAddress addr(AF_INET, server_->getPort(), server_->getIp().c_str());


	sock.setNoBlocking();
	sock.setExecClose();
	if (!sock.connect(addr)) {
		return false;
	}
	LOGGER_TRACE("ip : " << server_->getIp() << " port : " \
			<< server_->getPort());
	fas::Events event(sock.getSocket(), fas::kWriteEvent|fas::kReadEvent);
	fas::Poll poller;

	poller.EventsAdd(&event);

	int timeout = 1000;
	std::vector<fas::Events> rEvent;

	poller.Loop(rEvent, timeout);

	if(rEvent.size() <= 0) {
		return false;
	}

	LOGGER_TRACE("after pollloop");

	int error = 0;
	socklen_t len = sizeof(error);

	if (rEvent[0].containsAtLeastOneEvents(fas::kWriteEvent|fas::kReadEvent)) {
		if (getsockopt(sock.getSocket(), SOL_SOCKET, SO_ERROR, \
					reinterpret_cast<char *>(&error), &len) == 0 ){
			if (error != 0) {
				return false;
			}
		} else {
			LOGGER_ERROR(" getsockopt : " << ::strerror(errno));
			return false;
		}
	} else {
		LOGGER_ERROR(" poll connect error : " << ::strerror(errno));
		return false;
	}

	return true;
}

fas::mdm::mcacheTask::TaskState fas::mdm::mcacheTask::getState() const {
	return this->taskState_;
}

void fas::mdm::mcacheTask::setState(TaskState state) {
	this->taskState_ = state;
}

int fas::mdm::mcacheTask::getHasReadBytes() const {
	return hasReadBytes_;
}
void fas::mdm::mcacheTask::add2HasReadBytes(int bytes) {
	if (bytes <= 0) {
		return;
	}
	hasReadBytes_ += bytes;
}

std::string fas::mdm::mcacheTask::getMethod() const {
	return this->method_;
}

std::string fas::mdm::mcacheTask::getKey() const {
	return this->key_;
}

std::string fas::mdm::mcacheTask::getFlag() const {
	return this->flag_;
}

std::string fas::mdm::mcacheTask::getExptime() const {
	return this->exptime_;
}
int fas::mdm::mcacheTask::getBytes() const {
	return this->bytes_;
}

ssize_t writeSizeBytes2NoBlockSock(int sock, const char *buf, ssize_t size) {
	if (size <= 0) {
		return size;
	}

	ssize_t hasRet = 0, ret = 0;
	while (hasRet < size) {
		ret = write(sock, buf + hasRet, size - hasRet);
		if (ret < 0) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
				continue;
			}
		}
		if (ret == 0) {
			return -1;
		}

		hasRet += ret;
	}

	return size;
}

ssize_t readUntilStrWithEnd(int sock, std::string& str, std::string end) {
#define BUFSIZE_L 53
#define READSIZE_L 52
	char buf[BUFSIZE_L] = {0};
	while (true) {

		ssize_t ret = read(sock, buf, READSIZE_L);
		if (ret < 0) {
			if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
				continue;
			}
		}

		if (ret == 0) {
			return -1;
		}

		str = str + std::string(buf, 0, ret);

		const char *pos = std::search(str.data(), str.data() + str.size(), \
				end.data(), end.data() + end.size());
		if (pos != (str.data() + str.size())) {
			return pos - str.data();
		}
	}

	return 0;
}

