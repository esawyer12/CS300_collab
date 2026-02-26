/*
* Course: CS300
* Assignment: Project Two
* Student: Eric Sawyer
*/

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

//=====================================================================
// Helper functions
//=====================================================================

// Trim whitespace from both ends of a string.
static inline string trim(const string& s) {
	string copy = s;
	// left trim
	copy.erase(copy.begin(), find_if(copy.begin(), copy.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
	// right trim
	copy.erase(find_if(copy.rbegin(), copy.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), copy.end());
	return copy;
}


//=====================================================================
// Structs
//=====================================================================

struct Course {
	string courseNumber;
	string courseName;
	vector<string> prerequisites;
};

struct Node {
	Course course;
	Node* left;
	Node* right;

	// Default constructor
	Node() {
		this->left = nullptr;
		this->right = nullptr;
	}

	// Initialize with a course
	Node(Course course) {
		this->left = nullptr;
		this->right = nullptr;
		this->course = course;
	}
};



//=====================================================================
// BST class definition
//=====================================================================

class BinarySearchTree {

private:
	Node* root;

	void addNode(Node* node, Course course);
	void inOrder(Node* node);
	void deleteAll(Node* node);

public:
	BinarySearchTree();
	virtual ~BinarySearchTree();
	void InOrder();
	void Insert(Course course);
	Course Search(string courseNumber);
	void Clear();
};




//=====================================================================
// BST class implementation
//=====================================================================

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

/**
 * Helper to recursively delete nodes
 */
void BinarySearchTree::deleteAll(Node* node) {
	if (node != nullptr) {
		deleteAll(node->left);
		deleteAll(node->right);
		delete node;
	}
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
	deleteAll(root);
	root = nullptr;
}

/**
 * Clear the tree contents (keeps object usable)
 */
void BinarySearchTree::Clear() {
	deleteAll(root);
	root = nullptr;
}

/**
* Traversal helper function, responsible for printing an ordered list of courses
* Sorted by course number alphanumerically (e.g., CSCI100, CSCI200, MATH201)
*/
void BinarySearchTree::inOrder(Node* node) {
	if (node != nullptr) {
		inOrder(node->left);
		// Print course number and course name
		cout << node->course.courseNumber << " : " << node->course.courseName << endl;
		inOrder(node->right);
	}
}

/**
 * Traverse the tree in order (calls helper)
 */
void BinarySearchTree::InOrder() {
	inOrder(root);
	cout << endl;
}

/**
 * Add a course to some node (recursive)
 * Comparison is alphanumeric by courseNumber
 */
void BinarySearchTree::addNode(Node* node, Course course) {
	if (course.courseNumber < node->course.courseNumber) {
		// go left
		if (node->left == nullptr) {
			node->left = new Node(course);
		}
		else {
			addNode(node->left, course);
		}
	}
	else {
		// go right (duplicates go to right)
		if (node->right == nullptr) {
			node->right = new Node(course);
		}
		else {
			addNode(node->right, course);
		}
	}
}

/*
* Insert a course
*/
void BinarySearchTree::Insert(Course course) {
	if (this->root == nullptr) {
		this->root = new Node(course);
	}
	else {
		addNode(this->root, course);
	}
}

/**
 * Search for a course by courseNumber
 * Returns a Course with empty courseNumber if not found.
 */
Course BinarySearchTree::Search(string courseNumber) {
	Course course; // empty means not found
	Node* current = root;

	while (current != nullptr) {
		if (courseNumber == current->course.courseNumber) {
			return current->course;
		}
		else if (courseNumber < current->course.courseNumber) {
			current = current->left;
		}
		else {
			current = current->right;
		}
	}

	return course;
}



//=====================================================================
// Global functions
//=====================================================================

// Parses the csv line by line, creates a course object for each line and inserts it into BST
void LoadCourseDataFromFile(string fileName, BinarySearchTree* bst) {

	string textBuffer;

	// Open file stream
	fstream inFile(fileName);
	if (!inFile.is_open()) {
		cout << "Failed to open file: \"" << fileName << "\"" << endl;
		return;
	}

	// Main Course object creation loop
	// one loop = one line = one course
	int counter = 0;
	while (getline(inFile, textBuffer)) {
		++counter;

		// convert the line to an sstream to access getline()
		stringstream currLine(textBuffer);

		// Locals
		string courseNumber;
		string courseName;
		vector<string> prerequisites;
		string token;

		// Parse courseNumber and courseName first
		if (!getline(currLine, courseNumber, ',')) {
			// malformed line, skip
			continue;
		}
		if (!getline(currLine, courseName, ',')) {
			// malformed line, skip
			continue;
		}

		// Trim parsed values
		courseNumber = trim(courseNumber);
		courseName = trim(courseName);

		// Read remaining comma-separated prerequisite tokens (if any)
		while (getline(currLine, token, ',')) {
			token = trim(token);
			if (!token.empty()) {
				prerequisites.push_back(token);
			}
		}

		// Construct the course object and add to bst
		Course course = Course{ courseNumber, courseName, prerequisites };
		bst->Insert(course);
	}

	cout << "*Loaded " << counter << " courses*" << endl << endl;

	// Clean-up
	inFile.close();
	return;
}


//=====================================================================
// Main function entry
//=====================================================================

int main() {

	string fileName; 

	BinarySearchTree* bst;
	bst = new BinarySearchTree();

	int choice = 0;
	while (choice != 9) {
		cout << "Menu:" << endl;
		cout << "  1. Load Course Data" << endl;
		cout << "  2. Print Ordered List of Courses" << endl;
		cout << "  3. Find Course" << endl;
		cout << "  9. Exit" << endl;
		cout << "Enter choice: ";
		if (!(cin >> choice)) {
			// non-numeric input
			cout << "Invalid entry - please enter a number from the menu." << endl << endl;
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}
		// consume remaining newline so getline() works later
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << endl;

		switch (choice) {

		case 1:
			// Clear any existing data before loading new file
			bst->Clear();
			cout << "Enter the name of the file containing course data" << endl;
			getline(cin, fileName);
			cout << "*Loading course data from " << fileName << "*" << endl;
			LoadCourseDataFromFile(fileName, bst);
			break;
		case 2:
			// Print alphanumeric list of all courses (courseNumber : courseName)
			bst->InOrder();
			break;
		case 3: {
			// Prompt user for a course number and display course title and prerequisites
			cout << "Enter course number to find (e.g., CSCI200): ";
			string searchNumber;
			getline(cin, searchNumber);
			searchNumber = trim(searchNumber);

			if (searchNumber.empty()) {
				cout << "No course number entered." << endl << endl;
				break;
			}

			Course found = bst->Search(searchNumber);
			if (!found.courseNumber.empty()) {
				cout << found.courseNumber << " : " << found.courseName << endl;
				if (found.prerequisites.empty()) {
					cout << "Prerequisites: None" << endl << endl;
				}
				else {
					cout << "Prerequisites:" << endl;
					for (const auto& prereqNum : found.prerequisites) {
						// Look up title for each prerequisite
						Course prereqCourse = bst->Search(prereqNum);
						if (!prereqCourse.courseNumber.empty()) {
							cout << "  " << prereqCourse.courseNumber << " : " << prereqCourse.courseName << endl;
						}
						else {
							// Prereq number present but course not found in data
							cout << "  " << prereqNum << " : " << "(not found in data)" << endl;
						}
					}
					cout << endl;
				}
			}
			else {
				cout << "Course " << searchNumber << " not found." << endl << endl;
			}
			break;
		}
		case 9:
			// Exit - loop condition will terminate
			cout << "Exiting program." << endl;
			break;
		default:
			cout << "Invalid menu option. Please choose 1, 2, 3, or 9." << endl << endl;
			break;
		}

	}
	// Cleanup tree and program exit
	delete bst;
	bst = nullptr;
	return 0;
}