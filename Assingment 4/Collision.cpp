//Collision.cpp

#include "Collision.h"
#include "ObjLibrary/Vector3.h"
#include <iostream>
#include <stack>
#include <vector>
using namespace std;

	//constructor
BoundsList::BoundsList() {
	head = 0;
	tail = 0;

}
BoundsList::Node* BoundsList::createDefault() {
	Node* node = new Node();
	node->bound = 0;
	node->coords = 0.0;
	node->next = 0;
	node->prev = 0;

	return node;
}
//insert

void BoundsList::insertFront(double new_coords, int new_bound, int new_object) {
	Node* newNode = new Node;
	newNode->bound = new_bound;
	newNode->coords = new_coords;
	newNode->object = new_object;
	newNode->prev = 0;
	newNode->next = 0;

	//if no list existed yet
	if (head == 0) {
		head = newNode;
		tail = newNode;
		newNode->next = NULL;
	}
	else {
		newNode->next = head;
		head->prev = newNode;
		head = newNode;
	}
}
void BoundsList::insertEnd(double new_coords, int new_bound, int new_object) {
	Node* newNode = new Node;
	newNode->bound = new_bound;
	newNode->coords = new_coords;
	newNode->object = new_object;
	newNode-> next = NULL;

	//if no list existed yet
	if (head == NULL) {
		head = newNode;
		tail = newNode;
	}
	else {
		tail->next = newNode;
		newNode->prev = tail;
		tail = newNode;
	}
	return;
}
void BoundsList::insertMiddle(Node* first, Node* second, Node* new_node) {
	first->next = new_node;
	second->prev = new_node;
	new_node->next = second;
	new_node->prev = first;

	return;
}
void BoundsList::swapLeft(Node* first, Node* second) {
	//X-F-S-Y -> X-S-F-Y
	//need to change inbetween nodes if exist
	//node before first has to point to second
	if (first->prev) {
		second->prev = first->prev;
		first->prev->next = second;
		
	}
	else {
		second->prev = NULL;
		head = second;
	}
	if (second->next) { //node after second has to point to first
		first->next = second->next;
		second->next->prev = first;
	}
	else {
		first->next = NULL;
		tail = first;
	}

	second->next = first;
	first->prev = second;
}

//delete
void BoundsList::deleteObject(int Object) {
	Node* node = head;
	//search for object
	while (node) {
		if (node->object == Object) {
			break;
		}
		node = node->next;
	}
	if (node->prev && node->next) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	else if (node->prev){
		tail = node->prev;
		node->prev->next = NULL;	
	}
	else if (node->next) {
		head = node->next;
		node->next->prev = NULL;
	}

	delete node;
}
void BoundsList::deleteList() {
	tail = NULL;
	Node* temp;
	//iterate through all nodes until head is only one left
	while (head->next != NULL) {
		temp = head->next;
		delete head;
		head = temp;
	}
	delete head;

	return;
}
//based on code shown in https://www.prepbytes.com/blog/linked-list/bubble-sort-on-doubly-linked-list/#:~:text=%20Bubble%20Sort%20On%20Doubly%20Linked%20List%20,used.%20Let%20us%20check%20your%20understanding%21%21%20More%20
void BoundsList::bubbleSort() {
	int swap, i;
	Node* node = head;
	Node* temphead = head;
	Node* lastsorted = NULL;

	do {
		swap = 0;
		node = temphead;

		while (node && node->next && node->next != lastsorted) {
			if (node->coords > node->next->coords) {
				if (node == head) {
					head = node->next;
				}
				if (node->next == tail) {
					tail = node;
				}
				swapLeft(node, node->next);
				swap = 1;
			}
			if (node->coords == node->next->coords) {
				if (node->bound == 1 && node->next->coords == 0) {
					if (node == head) {
						head = node->next;
					}
					if (node->next == tail) {
						tail = node;
					}
					swapLeft(node, node->next);
					swap = 1;
				}
			}
			node = node->next;
		}
		lastsorted = node;

	} while (swap);
}
//sort - to be used once all objects are in lists. sends each dimension list through quick sort
//merge sort algorithm code is from  https://www.tutorialspoint.com/merge-sort-for-doubly-linked-list-using-cplusplus#:~:text=%20Merge%20Sort%20for%20Doubly%20Linked%20List%20using,the%20above%20program.%20%20...%20%20More%20 and slightly tweaked
void BoundsList::mergeSort() {
	mergeSortRecursive(&head);
	tail = findLast(head);
}
void BoundsList::mergeSortRecursive(Node **head) {
	Node* p = *head;
	Node* a = NULL;
	Node* b = NULL;
	if (p == NULL || p->next == NULL) {
		return;
	}
	splitList(p, &a, &b);
	mergeSortRecursive(&a);
	mergeSortRecursive(&b);
	*head = mergeSortedLists(a, b);
}
void BoundsList::splitList(Node* src, Node** fRef, Node** bRef) {
	Node* fast;
	Node* slow;
	slow = src;
	fast = src->next;
	while (fast != NULL) {
		fast = fast->next;
		if (fast != NULL) {
			slow = slow->next;
			fast = fast->next;
		}
	}
	*fRef = src;
	*bRef = slow->next;
	slow->next = NULL;
}
BoundsList::Node* BoundsList::mergeSortedLists(Node* head1, Node* head2) {
	Node* result = NULL;
	if (head1 == NULL) {
		return head2;
	}
	if (head2 == NULL) {
		return head1;
	}
	if (head1->coords < head2->coords) {
		head1->next = mergeSortedLists(head1->next, head2);
		head1->next->prev = head1;
		head1->prev = NULL;
		return head1;
	}
	else {
		head2->next = mergeSortedLists(head1, head2->next);
		head2->next->prev = head2;
		head2->prev = NULL;
		return head2;
	}

}

BoundsList::Node* BoundsList::findLast(Node* root) {
	while (root && root->next) {
		root = root->next;
	}
	return root;
}
//update - to be used when updating lists on physics checks after an object is updated
void BoundsList::updateList(Node* node) {
	double current = node->coords;
	if (node->next) {
		double next = node->next->coords;
		if (current > next) {
			//if size difference
			swapLeft(node, node->next);
			//updateList(node); // call again to check against peers
		}
			//if same size and the start of 1 object comes after the end of the next one, 
			//swap since don't want touching to cound as a collision
		else if (current == next) {
			if (node->object != node->next->object && node->bound == 1 && node->next->bound == 0) {
				//cout << "no collide swap with front" << endl;
				swapLeft(node, node->next);
			}
		}
	}
	if (node->prev) {
		double last = node->prev->coords;
		if (current < last) {
			//if size difference
			swap(node->prev, node);
			//updateList(node);
		}
		else if (current == last) {
			if (node->object != node->prev->object && node->bound == 0 && node->prev->bound == 1) {
				//cout << "no collide swap with prev" << endl;
				swap(node->prev, node);
			}
		}
	}
}
//update object, finds object and updates new coords. To be used each update
void BoundsList::updateObject(double new_coords, int bound, int object) {
	Node* node = head;
	//search for object
	while (node) {
		if (node->object == object && node->bound == bound){
			break;
		}
		node = node->next;
	}

	node->coords = new_coords;
}

void BoundsList::printListCoords() {
	Node* temp = head;
	while (temp) {
		cout << temp->coords << " bound: " << temp->bound << " Object: " << temp->object <<  endl;
		temp = temp->next;
	}
	
	cout << endl << "head coords: " << head->coords;
	cout << endl << "tail coords: " << tail->coords << endl << endl;
}

void BoundsList::findCollisions() {
	//search sorted list, if 0, add object to stack, if 1 remove it from stack
	//if 0 then encounter another 0, save two objects to flag for collison
	//pair up 0's then close pair once a 1 is encountered for each object
	//will need to keep track of each object as beggining is encountered then remove it if end is encountered without reaching another
	Node* traversal = head;

	while (traversal) {
		//check bound
		if (traversal->bound == 0) {
			//want to keep track until it's pair is found
			collisions.push_back(traversal->object);
		}

		if (traversal->bound == 1) {
			//if we found it's pair, remove it from the stack
			if (collisions.back() == traversal->object) {
				collisions.pop_back();
			}
			else {
				collisions.push_back(traversal->object);
			}
			//if no pair is found, then put on the stack so we have a better way of telling who's colliding
		}

		traversal = traversal->next;
	}
}

void BoundsList::findPairs() {
	//build collisions vector, want to pair up all nums between open and close
	findCollisions();
	//int spot points to open of the pair
	int spot = 0;
	//need to keep track of close of last pair - could build a list since multiple pairs
	vector<int> completedPairs;
	int size = collisions.size();

	if (collisions.size() > 0) { //make sure collisions exists
		while (spot < size) {
			if (!findPairObject(collisions[spot], completedPairs)) {
				buildPair(spot);
				completedPairs.push_back(collisions[spot]);
			}
			spot++;
		}
	}

	cout << "Collision Pairs: ";
	for (int i = 0; i < pairList.size(); i++) {
		cout << "Object1: " << pairList[i].object1 << " Object2: " << pairList[i].object2 << endl;
	}


}

void BoundsList::buildPair(int start) {
	int i = start + 1;
	while (collisions[start] != collisions[i]) {
		//build pair struct and add to list of pairs
		Pairs* temp = new Pairs;

		//add in object with smaller number as obj1
		if (collisions[start] > collisions[i]) {
			temp->object1 = collisions[i];
			temp->object2 = collisions[start];
		}
		else {
			temp->object1 = collisions[start];
			temp->object2 = collisions[i];
		}
		
		pairList.push_back(*(temp));
		i++;
		delete temp;
	}

}

bool BoundsList::findPairObject(int object, vector<int> pairs) {
	int size = pairs.size();
	//if object is in the list given, return true else return false
	for (int i = 0; i < size; i++) {
		if (pairs[i] == object) {
			return true;
		}
	}
	return false;
}