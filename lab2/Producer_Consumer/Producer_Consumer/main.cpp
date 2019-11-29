#include <iostream>;
#include <thread>;
#include <queue>; 
#include <chrono>;
#include "ThreadSafeQueue.h"

#define N 5 /* количество мест для записей в буфере */
ThreadSafeQueue<int> buffer;
std::vector<int> check;


volatile bool isSleeping=false;

int currentValue = 0;

int produce_item() {
	currentValue++;
	return currentValue;
}

void insert_item(int item) {
	buffer.push(item);
}

int remove_item() {
	 return buffer.pop();
}

void consume_item(int item) {
	check.push_back(item);
	//std::cout << item << " ";
}

void sleep() {
	isSleeping = true;
	while (isSleeping) {
		std::this_thread::yield();
	}
}

void wakeup() {
	isSleeping = false;
}


void producer()
{
	int item;
	while (true) { // бесконечное повторение 
		std::this_thread::yield();
		//std::this_thread::yield();
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
		if (buffer.size() == 0) sleep(); // если буфер пуст, заблокироваться 
		item = remove_item(); // извлечь запись из буфера 
		if (buffer.size() == N - 1) wakeup(); // был ли буфер полон? 
		consume_item(item); // распечатка записи 
	}
}

void test2() {
	if (check.empty())return;
	
	auto it = std::max_element(std::begin(check), std::end(check));
	int size = *it+1;
	bool* arr = new bool[size] {};
	
	for (auto var : check)
	{
		if (var > 0 && var < size) {
			if (arr[var]) {
				std::cout << "twice " << var << '\n';
			}
			arr[var] = true;
		}
		else {
			std::cout << "lose item, because found " << var << '\n';
		}
	}

	for (int i = 1; i < size; i++){
		if (!arr[i])
			std::cout << i << " not found\n";
	}
	delete[]arr;
	std::cout << "end check lose item\n";
}

void test() {
	int previos;

	do {
		std::cout << "current item " << currentValue << std::endl;
		previos = currentValue;
		std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (previos != currentValue);
	std::cout << "deadLock\n";
	test2();
}

void restart() {
	isSleeping = false;
	currentValue = 0;
	check.clear();
	buffer.clear();
}

int main() {
	std::thread producerThread(producer);
	std::thread consumerThread(consumer);
	std::thread producerThread2(producer);

	std::thread testThread(test);

	testThread.join();
	
	exit(0);
}