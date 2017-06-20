#include "memProtocol.h"


const std::string fas::mdm::memProtocol::noreply_ = "noreply";

fas::mdm::memProtocol::memProtocol() {
	stroage_cmd_.insert({
			"set", "add", "replace", "append", "prepend", "cas",
			});
	//  “append”和”prepend”命令仅更新已经存在数据，不会更新flag及过期时间设置。
	//  CAS协议解决的问题

	//  模拟多个Memcached client并发set同一个key的场景。如clientA想把当前key的value set为"x"，且操作成功；clientB却把当前key的value值由"x"覆盖set为"y"，这时clientA再根据key去取value时得到"y"而不是期望的"x"，它使用这个值，但不知道这个值已经被其它线程修改过，就可能会出现问题。

	//  CAS协议解决这种并发修改问题。有线程试图修改当前key-value对的value时，先由gets方法得到item的版本号，操作完成提交数据时，使用cas方法谨慎变更，如果在本地对item操作过程中这个key-value对在Memcached server端被其它线程更改过，就放弃此次修改( 乐观锁概念)。
	retriveal_cmd_.insert({
			"get", "gets",
			});
	delete_cmd_.insert({
			"delete",
			});
	incr_decr_cmd_.insert({
			"incr", "decr"
			});
	other_cmd_.insert({
			"touch", "flush_all", "version", "quit"
			});
}

bool fas::mdm::memProtocol::inCmdSet(const std::string& cmd) const {
	if (isStorageCommand(cmd) || isRetrievalCommand(cmd) || isOtherCommand(cmd) || isIncrDecrCommand(cmd) || isDeleteCommand(cmd)) {
		return true;
	}
	return false;
}

bool fas::mdm::memProtocol::isStorageCommand(const std::string& cmd) const {
	return stroage_cmd_.find(cmd) != stroage_cmd_.end();
}

bool fas::mdm::memProtocol::isRetrievalCommand(const std::string& cmd) const {
	return retriveal_cmd_.find(cmd) != retriveal_cmd_.end();
}

bool fas::mdm::memProtocol::isOtherCommand(const std::string& cmd) const {
	return other_cmd_.find(cmd) != other_cmd_.end();
}

bool fas::mdm::memProtocol::isDeleteCommand(const std::string& cmd) const {
	return delete_cmd_.find(cmd) != delete_cmd_.end();
}

bool fas::mdm::memProtocol::isIncrDecrCommand(const std::string& cmd) const {
	return incr_decr_cmd_.find(cmd) != incr_decr_cmd_.end();
}

const std::string& fas::mdm::memProtocol::getNoreply() const {
	return noreply_;
}
