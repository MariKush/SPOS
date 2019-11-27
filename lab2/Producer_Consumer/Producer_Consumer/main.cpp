#include <iostream>;
#include <thread>;
#include <queue>; 
#include <chrono>;

#define N 5 /* количество мест для записей в буфере */
//int count = 0; /* количество записей в буфере */
std::queue<int> buffer;

bool isSleeping;

int currentValue = 0;

int produce_item() {
	currentValue++;
	return currentValue;
}

void insert_item(int item) {
	buffer.push(item);
}

int remove_item() {
	 int item = buffer.front();
	 buffer.pop();
	 return item;
}

void consume_item(int item) {
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
		item = produce_item(); // генерация новой записи 
		if (buffer.size() == N) sleep(); // если буфер полон, заблокироваться 
		insert_item(item); // помещение записи в буфер 
		//count = count + 1; // увеличение счетчика записей в буфере 
		if (buffer.size() == 1) wakeup(); // был ли буфер пуст? 
	}
}
void consumer()
{
	int item;
	while (true) { // бесконечное повторение 
		if (buffer.size() == 0) sleep(); // если буфер пуст, заблокироваться 
		item = remove_item(); // извлечь запись из буфера 
		//count = count -1; // уменьшение счетчика записей в буфере 
		if (buffer.size() == N - 1) wakeup(); // был ли буфер полон? 
		consume_item(item); // распечатка записи 
	}
}

void test() {
	int previos;

	do {
		std::cout << "current item" << currentValue << std::endl;
		previos = currentValue;
		std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (previos != currentValue);
	std::cout << "deadLock\n";
}

int main() {

	std::thread producerThread(producer);
	std::thread consumerThread(consumer);

	std::thread testThread(test);

	//std::thread producerThread2(producer);
	//std::thread consumerThread2(consumer);

	producerThread.join();
	consumerThread.join();


	//producerThread2.join();
	//consumerThread2.join();

}