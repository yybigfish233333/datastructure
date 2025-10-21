#include<iostream>
#include<list>
#include<chrono>
#include<thread>
#include<unordered_map>
using namespace std;

class Node {
public:
	int key;
	int value;
	long timestamp;
	Node(int key_, int value_, long timestamp_) :key(key_), value(value_), timestamp(timestamp_) {}
};

class LRUCache {
private:
	list<Node> cacheList;
	unordered_map<int, list<Node>::iterator> cacheMap;
	int capacity;

	//获取当前时间戳
	long getCurrentTimeMillis() {
		using namespace chrono;
		auto now = system_clock::now();
		return duration_cast<milliseconds>(now.time_since_epoch()).count();
	}

	bool isExpire(const Node& node) {
		if (node.timestamp == -1) {
			return false;
		}

		return getCurrentTimeMillis() > node.timestamp;//如果当前时间大于过期时间就是要淘汰了
	}

	void removeExpireNodes() {
		//删除所有过期节点
		auto it = cacheList.begin();
		while (it != cacheList.end()) {
			if (isExpire(*it)) {
				cacheMap.erase(it->key);
				it = cacheList.erase(it);
			}
			else {
				it++;//向后遍历
			}
		}
	}

public:
	LRUCache(int capacity_) :capacity(capacity_) {}
	int get(int key) {
		auto it = cacheMap.find(key);
		if (it == cacheMap.end()) {
			return -1;
		}

		Node node = *(it->second);
		if (isExpire(node)) {
			//如果过期了
			cacheList.erase(it->second);
			cacheMap.erase(it);
			return -1;
		}

		//移动到链表的尾部，最近使用
		cacheList.erase(it->second);
		cacheList.push_back(node);
		cacheMap[key] = prev(cacheList.end());//prev是获取前一个位置的迭代器，cacheList.end()不是有效位置，而是结束标记
		return node.value;
	}

	void put(int key, int value, long timestamp = -1) {
		auto it = cacheMap.find(key);
		if (it != cacheMap.end()) {
			cacheList.erase(it->second);
			cacheMap.erase(it);
		}

		//插入新节点前先删除过期节点
		if (cacheMap.size() >= capacity) {
			removeExpireNodes();
		}

		//空间仍然不足，删除LRU节点
		if (cacheMap.size() >= capacity) {
			int lruKey = cacheList.front().key;//因为是尾部插入新的，所以最久的在head
			cacheMap.erase(lruKey);
			cacheList.pop_front();
		}

		//插入新节点到链表尾部
		cacheList.push_back(Node(key, value, timestamp));
		cacheMap[key] = prev(cacheList.end());
	}
};
int main() {
	//当前时间>节点的timestamp，即为过期节点
	LRUCache cache(2);

	cache.put(1, 10, -1); // -1表示永不过期
	cache.put(2, 20, -1);
	cout << cache.get(1) << endl; // 10
	cache.put(3, 30, -1);
	cout << cache.get(2) << endl; // -1

	long futureTime = chrono::duration_cast<chrono::milliseconds>(
		chrono::system_clock::now().time_since_epoch()
	).count() + 1000;
	cache.put(4, 40, futureTime);

	cout << cache.get(1) << endl; // -1
	this_thread::sleep_for(chrono::milliseconds(1500));
	cout << cache.get(3) << endl; // 30
	cout << cache.get(4) << endl; // -1 (已过期)

	return 0;
}
