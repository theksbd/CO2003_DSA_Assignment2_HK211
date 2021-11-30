#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

struct Node {
	string identifier_name;
	string identifier_type;
	int BlockLevel;
	//int key;
	Node* left;
	Node* right;
	Node* parent;

	Node(string identifier_name, string identifier_type, int Blocklevel = 0,
		Node* left = NULL, Node* right = NULL, Node* parent = NULL)
	{
		this->identifier_name = identifier_name;
		this->identifier_type = identifier_type;
		//this->key = key;
		this->BlockLevel = Blocklevel;
		this->left = left;
		this->right = right;
		this->parent = parent;
	}
};

class SymbolTable
{
//public:
    //class Node;
protected:
    Node* root;
	int num_comp;
	int num_splay;
public:
	SymbolTable() : root(NULL), num_comp(0), num_splay(0) {};
	~SymbolTable() {};
    void run(string filename);
	void DivideLineInsert(string line, string& identifier_name, string& identifier_type, string& isStatic);
	int CheckIdentifierName(string identifier_name, int BlockLevel);
	void CheckIdentifierType(string line, string identifier_type, int BlockLevel);
	bool IsFunctionType(string identifier_type);
	void add(string identifier_name, string identifier_type, int BlockLevel);
	void Zig(Node* root);
	void Zig_Zig(Node* root);
	void Zig_Zag(Node* root);
	void Splay(Node* root);
	void DivideLineAssign(string line, string& identifier_name, string& value);
	bool IsValidIdentifierName(string identifier_name);
	bool IsFullDigit(string value, string::size_type size_value);
	bool IsValidStringType(string value, string::size_type size_value);
	Node* Search_No_Splay(string identifier_name, int BlockLevel);
	void DeleteBlock(string identifier_name, int BlockLevel);
	void Preorder(Node* root);

 //   class Node {
	//private:
	//	string identifier_name;
	//	string identifier_type;
	//	int BlockLevel;
	//	//int key;
	//	Node* left;
	//	Node* right;
	//	friend class SymbolTable;
	//public:
	//	Node(string identifier_name, string identifier_type, int Blocklevel = 0,
	//		Node* left = NULL, Node* right = NULL)
	//	{
	//		this->identifier_name = identifier_name;
	//		this->identifier_type = identifier_type;
	//		//this->key = key;
	//		this->BlockLevel = Blocklevel;
	//		this->left = left;
	//		this->right = right;
	//	}
 //   };
};
#endif