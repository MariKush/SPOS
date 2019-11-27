#include <iostream>;
#include <thread>;
#include <queue>; 
#include <chrono>;
#include "ThreadSafeQueue.h"

#define N 5 /* количество мест для записей в буфере */
ThreadSafeQueue<int> buffer;
std::queue<int> check;


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
	check.push(item);
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
		item = produce_item(); // генерация новой записи 
		if (buffer.size() == N) sleep(); // если буфер полон, заблокироваться 
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
	int size =check.back()+1;
	bool* arr = new bool[size] {};
	
	while (!check.empty()) {

		if (check.front() > 0 && check.front() < size) {
			if (arr[check.front()]) {
				std::cout << "twice " << check.front() << '\n';
			}
			arr[check.front()] = true;
		}
		else {
			std::cout << "lose item, because found " << check.front() << '\n';
		}

		check.pop();
	}
	for (int i = 1; i < size; i++){
		if (!arr[i])
			std::cout << i << " not found\n";
	}
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

int main() {

	std::thread producerThread(producer);
	std::thread consumerThread(consumer);

	std::thread testThread(test);

	std::thread producerThread2(producer);
	//std::thread consumerThread2(consumer);

	producerThread.join();
	consumerThread.join();


	//producerThread2.join();
	//consumerThread2.join();

}