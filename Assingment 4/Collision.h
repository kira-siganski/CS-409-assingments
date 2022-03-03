#pragma once
#include <iostream>
#include <stack>
#include <vector>
using namespace std;
/*object quick ref :
asteroids 0 - asteroidcount-1
ship asteroid count
crystals num > asteroidcount*/ 
class BoundsList {
private:
	struct Node {
		double coords;
		int bound;
		int object;

		Node* next;
		Node* prev;
	};

	struct Pairs {
		int object1;
		int object2;

		Node* next;
		Node* prev;
	};

	Node* head = NULL;
	Node* tail = NULL;
	vector<Pairs> pairList;
	vector<int> collisions;

	
public:
	//constructor
	BoundsList();
	Node* createDefault();
	//insert
	void insertFront(double new_coords, int new_bound, int new_object);
	void insertEnd(double new_coords, int new_bound, int new_object);
	void insertMiddle(Node* first, Node* second, Node *new_node);
	void swapLeft(Node* first, Node* second);
	//delete
	void deleteObject(int Object);
	void deleteList();
	//bubble sort to update lists each physics check
	void bubbleSort();
	//sort - to be used once all objects are in lists. sends each dimension list through quick sort
	//merge sort alg code is from https://www.tutorialspoint.com/merge-sort-for-doubly-linked-list-using-cplusplus#:~:text=%20Merge%20Sort%20for%20Doubly%20Linked%20List%20using,the%20above%20program.%20%20...%20%20More%20
	void mergeSort();
	void mergeSortRecursive(Node** head);
	void splitList(Node* src, Node **fRef, Node **bRef);
	Node* mergeSortedLists(Node* head1, Node* head2);
	Node* findLast(Node* root);
	//update - to be used when updating lists on physics checks after an object is updated
	void updateList(Node* node);
	//update object, finds object and updates new coords. To be used when collisions happen and only a few objects need to be altered
	void updateObject(double new_coords, int bound, int object);
	//testing to see if sorting works
	void printListCoords();
	void findCollisions();
	void findPairs();
	void buildPair(int start);
	bool findPairObject(int object, vector<int> pairs);
};
