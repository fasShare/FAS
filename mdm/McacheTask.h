#ifndef FAS_MCACHETASK_H
#define FAS_MCACHETASK_H
#include <Buffer.h>
#include <Socket.h>
#include <TcpConnection.h>
#include <MemProtocol.h>
#include <ConsistencyHash.h>

namespace fas {

namespace mdm {

class McacheTask
{
public:
    using HashSPtr = ConsistencyHash::HashSPtr;
    using SerNodeSptr = ConsistencyHash::SerNodeSptr;
    using TcpConnShreadPtr = fas::TcpConnection::TcpConnShreadPtr;
    enum TaskState {
        BAD,     //TcpConnection will be closed!
        BEGIN_NEW_TASK,
        PARSE_PROTOCOL,
        BEGIN_HANDLE_CMD,
        WAIT_ENOUGH_DATA,
        GET_ALL_DATA,
    };
    McacheTask(HashSPtr hash);

    void renew();

    TaskState getState() const;
    void setState(TaskState state);

    int getHasReadBytes() const;
    void add2HasReadBytes(int bytes);

    bool handleStrogeCmd(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);
    bool handleRetriveCmd(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);
    bool handleDeleteCmd(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);
    bool handleIncrDecrCmd(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);
    bool handleOtherCmd(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);

    bool getSocketFromKey(const string& key, fas::Socket& sock);
    std::string getMethod() const;
    std::string getKey() const;
    std::string getFlag() const;
    std::string getExptime() const;
    int getBytes() const;

    bool taskCommunition(TcpConnShreadPtr conn, boost::shared_ptr<Buffer> buffer);
private:
    TaskState taskState_; //链接应该被关闭

    std::string head_;
    std::vector<std::string> headerItems_;


    std::string method_;
    std::string key_;
    std::vector<std::string> keys_;
    std::string flag_;
    std::string exptime_;
    int bytes_;
    std::string cas_unique_;
    std::string noreply_;

    int hasReadBytes_;

    HashSPtr hash_;
    SerNodeSptr server_;

    MemProtocol protocol_;
};
}

}

ssize_t writeSizeBytes2NoBlockSock(int sock, const char *buf, ssize_t size);
ssize_t readUntilStrWithEnd(int sock, std::string& str, std::string end);

#endif // FAS_MCACHETASK_H
