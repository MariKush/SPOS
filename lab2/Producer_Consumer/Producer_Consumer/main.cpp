#include <iostream>;
#include <thread>;
#include <queue>; 
#include <chrono>;
#include <mutex>
#include <condition_variable>
#include "ThreadSafeQueue.h"

#define N 5 /* количество мест для записей в буфере */
ThreadSafeQueue<int> buffer;
ThreadSafeQueue<int> check;



std::mutex m;
std::condition_variable cv;

int currentValue = 0;
int countItem = 0;
int errors = 0;
int preErrors = 0;

int produce_item() {
	currentValue++;
	return currentValue;
}

void insert_item(int item) {
	countItem++;
	buffer.push(item);
}

int remove_item() {
	countItem--;
	return buffer.pop();
}

void consume_item(int item) {
	check.push(item);
	//std::cout << item << " ";
}

void sleep() {
	std::unique_lock<std::mutex> lk(m);
	cv.wait(lk);
}

void wakeup() {
	cv.notify_all();
}

void checkNotEqual() {
	if (abs( buffer.size() - countItem)>1) {
		errors++;
		countItem = buffer.size();
	}
}


void producer()
{
	int item;
	while (true) { // бесконечное повторение 
		checkNotEqual();
		std::this_thread::yield();
		item = produce_item(); // генерация новой записи 
		if (buffer.size() >= N) sleep(); // если буфер полон, заблокироваться 
		insert_item(item); // помещение записи в буфер 
		if (buffer.size() == 1) wakeup(); // был ли буфер пуст? 
	}
}

void consumer()
{
	int item;
	while (true) { // бесконечное повторение 
		checkNotEqual();
		if (buffer.size() == 0) sleep(); // если буфер пуст, заблокироваться 
		item = remove_item(); // извлечь запись из буфера 
		if (buffer.size() == N - 1) wakeup(); // был ли буфер полон? 
		consume_item(item); // распечатка записи 

	}
}

bool checkDeadLock() {
	int previos;
	std::cout << "current item " << currentValue << std::endl;
	previos = currentValue;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	if (previos == currentValue) {
		std::cout << "deadLock\n";
		return true;
	}
	return false;
}

bool checkLoseItem2(){
	if (preErrors != errors) {
		std::cout << "Errors " << errors << std::endl;
		preErrors = errors;
	}
	return false;
}

bool checkLoseItem() {
	int previousItem = 0, lostItem = -1;
	bool wasLoss = false;
	while (!check.empty()) {
		int item = check.pop();
		if (item == previousItem) {//doubling neighbor
			std::cout << "double item " << item << std::endl;
			return true;
		}
		else if (item == previousItem + 1) {//all is okay		
			previousItem = item;
		}
		else if (item == previousItem + 2) {
			if (wasLoss) {
				std::cout << "lose item " << lostItem << std::endl;
				return true;
			}
			wasLoss = true;
			lostItem = previousItem + 1;
			previousItem = item;

		}
		else if (item == lostItem && wasLoss) { //item found
			wasLoss = false;
		}
		else {
			std::cout << "double item "<<item<<" does not close\n";
			return true;
		}
	}
	return false;
}

void test() {
	do {

	} while (!checkDeadLock() && !checkLoseItem2());
}

int main() {
	std::thread producerThread(producer);
	std::thread consumerThread(consumer);
	//std::thread producerThread2(producer);
	//std::thread consumerThread2(consumer);

	test();
	exit(0);
}