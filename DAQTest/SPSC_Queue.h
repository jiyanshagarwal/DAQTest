#pragma once
#include <iostream>
#include <vector>
#include <stdexcept>

template <typename T>
class SPSC_Queue {
public:
	struct Node {
		T data;
		Node* next;
		Node* prev;
	};

	//-------------------------------[Constructors/Destructor]-------------------------------
	SPSC_Queue();
	SPSC_Queue(const SPSC_Queue<T>& ref);
	~SPSC_Queue();

	//-------------------------------[Operators]-------------------------------
	LinkedList<T>& operator=(const LinkedList<T>& ref);
	const T& operator[](unsigned long long int index) const;
	T& operator[](unsigned long long int index);
	bool operator==(const LinkedList<T>& ref) const;

	//-------------------------------[Insertion]-------------------------------
	void AddHead(const T& data);
	void AddTail(const T& data);
	void AddNodesHead(const T* data, unsigned long long int count);
	void AddNodesTail(const T* data, unsigned long long int count);
	void InsertAfter(Node* node, const T& data);
	void InsertBefore(Node* node, const T& data);
	void InsertAt(const T& data, unsigned long long int index);

	//-------------------------------[Deletion]-------------------------------
	bool RemoveHead();
	bool RemoveTail();
	unsigned long long int Remove(const T& data);
	bool RemoveAt(unsigned long long int index);
	void Clear();

	//-------------------------------[Accessors]-------------------------------
	unsigned long long int NodeCount() const;
	void FindAll(std::vector<Node*>& out, const T& data) const;
	const Node* Find(const T& data) const;
	Node* Find(const T& data);
	const Node* GetNode(unsigned long long int index) const;
	Node* GetNode(unsigned long long int index);
	const Node* Head() const;
	Node* Head();
	const Node* Tail() const;
	Node* Tail();

	//-------------------------------[Output]-------------------------------
	void PrintForward() const;
	void PrintReverse() const;
	void PrintForwardRecursive(const Node* node) const;
	void PrintReverseRecursive(const Node* node) const;

private:
	unsigned long long int nodeCount;
	Node* head;
	Node* tail;

	void copy(const LinkedList<T>& ref);
};

//-----------------------------------------[Constructors/Destructor]-----------------------------------------
template <typename T>
LinkedList<T>::LinkedList() {
	nodeCount = 0;
	head = nullptr;
	tail = nullptr;
}