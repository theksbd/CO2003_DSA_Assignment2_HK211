#include "SymbolTable.h"

void SymbolTable::DivideLineInsert(string line, string &identifier_name, string &identifier_type, string &isStatic)
{
    if (line[0] == ' ')
        throw InvalidInstruction("INSERT " + line);
    int section = 1;
    for (string::size_type i = 0, j = 0; i <= line.size(); i++)
    {
        if (line[i] == ' ' && line[i + 1] == ' ')
            throw InvalidInstruction("INSERT " + line);
        if (line[i] == ' ' || line[i] == '\0')
        {
            switch (section)
            {
            case 1: // assign for identifier_name
                identifier_name = line.substr(0, i);
                j = i + 1;
                section++;
                break;
            case 2: // assign for identifier_type
                identifier_type = line.substr(j, i - j);
                j = i + 1;
                section++;
                break;
            case 3: // assign for isStatic
                isStatic = line.substr(j, i - j);
                section++;
                break;
            default:
                throw InvalidInstruction("INSERT " + line);
                break;
            }
        }
    }
}

int SymbolTable::CheckIdentifierName(string identifier_name, int BlockLevel)
{
    if (IsValidIdentifierName(identifier_name) == false)
        return 0;

    int temp_splay = num_splay;
    Node* searchNode = Search_No_Splay(identifier_name, BlockLevel);
    num_splay = temp_splay;
    /*while (searchNode != NULL && (searchNode->identifier_name != identifier_name))
    {
        if (identifier_name.compare(searchNode->identifier_name) < 0)
            searchNode = searchNode->left;
        else
            searchNode = searchNode->right;
    }
    if (searchNode != NULL && searchNode->BlockLevel == BlockLevel)
        return 1;*/
    if (searchNode != NULL)
        return 1;

    return 2;
    // return 0: Invalid, return 1: Redeclare, return 2: Good, this is valid name
}

bool SymbolTable::IsFunctionType(string identifier_type)
{
    string paraType = identifier_type.substr(0, identifier_type.size() - 8);
    string returnType = identifier_type.substr(identifier_type.size() - 8);

    if (returnType != "->string" && returnType != "->number")
        return false;
    if (paraType[0] != '(' || paraType[paraType.size() - 1] != ')')
        return false;
    for (string::size_type i = 1; i <= paraType.size() - 2; i += 7)
    {
        if (paraType[i] == 'n')
        {
            string s = paraType.substr(i, 7);
            if (s != "number," && s != "number)")
                return false;
        }
        else if (paraType[i] == 's')
        {
            string s = paraType.substr(i, 7);
            if (s != "string," && s != "string)")
                return false;
        }
        else
            return false;
    }
    return true;
    // return 0: InvalidInstruction, return 1: Good, this is function type
}

void SymbolTable::CheckIdentifierType(string line, string identifier_type, int BlockLevel)
{
    if (identifier_type == "string" || identifier_type == "number")
        return;
    if (identifier_type.size() < 10) // minimum length of function type has length of 10
        throw InvalidInstruction(line);
    int isFunc = IsFunctionType(identifier_type);
    if (isFunc == 0)
        throw InvalidInstruction(line);
    if (BlockLevel != 0)
    {
        if (identifier_type != "string" && identifier_type != "number")
        {
            throw InvalidDeclaration(line);
        }
    }
}

void SymbolTable::add(string identifier_name, string identifier_type, int BlockLevel)
{
    if (this->root == NULL)
        this->root = new Node(identifier_name, identifier_type, BlockLevel);
    else
    {
        Node *nNode = new Node(identifier_name, identifier_type, BlockLevel);
        Node *walkNode = this->root;
        Node *parentNode = this->root;
        while (walkNode != NULL)
        {
            parentNode = walkNode;
            if (nNode->BlockLevel != walkNode->BlockLevel)
            {
                if (nNode->BlockLevel < walkNode->BlockLevel)
                    walkNode = walkNode->left;
                else
                    walkNode = walkNode->right;
            }
            else
            {
                if (nNode->identifier_name.compare(walkNode->identifier_name) < 0)
                    walkNode = walkNode->left;
                else
                    walkNode = walkNode->right;
            }
            //num_comp++;
        }
        if (parentNode != NULL)
        {
            if (nNode->BlockLevel < parentNode->BlockLevel)
                parentNode->left = nNode;
            else if (nNode->BlockLevel > parentNode->BlockLevel)
                parentNode->right = nNode;
            else
            {
                if (nNode->identifier_name.compare(parentNode->identifier_name) < 0)
                    parentNode->left = nNode;
                else
                    parentNode->right = nNode;
            }
        }
        nNode->parent = parentNode;
        //this->root = Splay(this->root, nNode->identifier_name);
        Splay(nNode);
    }
}

void SymbolTable::Zig(Node* root)
{
    Node* parentNode = root->parent;

    if (parentNode->left == root)
    {
        Node* rightTreeOfRoot = root->right;

        // rotate parent to right
        root->right = parentNode;
        parentNode->left = rightTreeOfRoot;
        root->parent = NULL;
        parentNode->parent = root;

        if (rightTreeOfRoot != NULL) 
            rightTreeOfRoot->parent = parentNode;
    }
    else
    {
        Node* leftTreeOfRoot = root->left;

        // rotate parent to left
        root->left = parentNode;
        parentNode->right = leftTreeOfRoot;
        // update parent
        root->parent = NULL;
        parentNode->parent = root;

        if (leftTreeOfRoot != NULL) 
            leftTreeOfRoot->parent = parentNode;
    }
}

void SymbolTable::Zig_Zig(Node* root)
{
    Node* parentNode = root->parent;
    Node* grandNode = parentNode->parent;
    if (parentNode->left == root)
    {
        Node* rightTreeOfRoot = root->right;
        Node* rightTreeOfParent = parentNode->right;

        // rotate parent and grand to right
        root->right = parentNode;
        parentNode->left = rightTreeOfRoot;
        parentNode->right = grandNode;
        grandNode->left = rightTreeOfParent;
        // update parent
        root->parent = grandNode->parent;
        parentNode->parent = root;
        grandNode->parent = parentNode;

        if (root->parent != NULL) // update left or right pointer of (parent of grand) point to root
        {
            if (root->parent->left == grandNode)
                root->parent->left = root;
            else 
                root->parent->right = root;
        }

        if (rightTreeOfRoot != NULL)
            rightTreeOfRoot->parent = parentNode;
        if (rightTreeOfParent != NULL)
            rightTreeOfParent->parent = grandNode;
    }
    else
    {
        Node* leftTreeOfRoot = root->left;
        Node* leftTreeOfParent = parentNode->left;

        // rotate parent and grand to left
        root->left = parentNode;
        parentNode->right = leftTreeOfRoot;
        parentNode->left = grandNode;
        grandNode->right = leftTreeOfParent;
        // update parent
        root->parent = grandNode->parent;
        parentNode->parent = root;
        grandNode->parent = parentNode;

        if (root->parent != NULL) // update left or right pointer of (parent of grand) point to root
        {
            if (root->parent->left == grandNode)
                root->parent->left = root;
            else 
                root->parent->right = root;
        }

        if (leftTreeOfParent != NULL)
            leftTreeOfParent->parent = grandNode;
        if (leftTreeOfRoot != NULL)
            leftTreeOfRoot->parent = parentNode;
    }
}

void SymbolTable::Zig_Zag(Node* root)
{
    Node* parentNode = root->parent;
    Node* grandNode = parentNode->parent;

    if (parentNode->right == root)
    {
        Node* leftTreeOfRoot = root->left;
        Node* rightTreeOfRoot = root->right;

        // rotate parent to left then rotate grand to right
        root->left = parentNode;
        root->right = grandNode;
        parentNode->right = leftTreeOfRoot;
        grandNode->left = rightTreeOfRoot;
        // update parent
        root->parent = grandNode->parent;
        parentNode->parent = root;
        grandNode->parent = root;

        if (root->parent != NULL)
        {
            if (root->parent->left == grandNode)
                root->parent->left = root;
            else 
                root->parent->right = root;
        }

        if (leftTreeOfRoot != NULL)
            leftTreeOfRoot->parent = parentNode;
        if (rightTreeOfRoot != NULL)
            rightTreeOfRoot->parent = grandNode;
    }
    else
    {
        Node* leftTreeOfRoot = root->left;
        Node* rightTreeOfRoot = root->right;

        // rotate parent to right then rotate grand to left
        root->left = grandNode;
        root->right = parentNode;
        parentNode->left = rightTreeOfRoot;
        grandNode->right = leftTreeOfRoot;
        // update parent
        root->parent = grandNode->parent;
        parentNode->parent = root;
        grandNode->parent = root;

        if (root->parent != NULL)
        {
            if (root->parent->left == grandNode)
                root->parent->left = root;
            else 
                root->parent->right = root;
        }

        if (leftTreeOfRoot != NULL)
            leftTreeOfRoot->parent = grandNode;
        if (rightTreeOfRoot != NULL)
            rightTreeOfRoot->parent = parentNode;
    }
}

void SymbolTable::Splay(Node* root)
{
    bool IsSplay = false;
    while (root->parent != NULL)
    {
        IsSplay = true;
        Node* parentNode = root->parent;
        Node* grandNode = parentNode->parent;
        if (grandNode == NULL)
        {
            //num_comp++;
            Zig(root);
        }
        else if (grandNode->left == parentNode && parentNode->left == root)
        {
            //num_comp += 2;
            Zig_Zig(root);
        }
        else if (grandNode->right == parentNode && parentNode->right == root)
        {
            //num_comp += 2;
            Zig_Zig(root);
        }
        else 
        {
            //num_comp += 2;
            Zig_Zag(root);
        }
        // num_splay++;
    }
    this->root = root;
    if (IsSplay == true)
        num_splay++;
    //num_splay = IsSplay ? num_splay + 1 : num_splay;
}

void SymbolTable::DivideLineAssign(string line, string& identifier_name, string& value)
{
    if (line[0] == ' ')
        throw InvalidInstruction("ASSIGN " + line);
    string::size_type j = 0;
    for (string::size_type i = 0; i <= line.size(); i++)
    {
        if (line[i] == ' ') 
        {
            identifier_name = line.substr(0, i); // get identifier name
            j = i + 1;
            break;
        }
        if (line[i] == '\0')
            throw InvalidInstruction("ASSIGN " + line);
    }

    value = line.substr(j); // get value
    string::size_type size_value = value.size();
    // check if value is valid
    for (string::size_type i = 0; i < size_value; i++)
    {
        if (value[i] == ' ')
        {
            if (value[0] != '\'' || value[size_value - 1] != '\'')
                throw InvalidInstruction("ASSIGN " + line);
        }
    }
}

bool SymbolTable::IsValidIdentifierName(string identifier_name)
{
    // check if identifier name is valid
    if (identifier_name[0] < 'a' || identifier_name[0] > 'z')
    {
        return false;
    }
    string::size_type size = identifier_name.size();
    for (string::size_type i = 1; i < size; i++)
    {
        if (!((identifier_name[i] >= 'a' && identifier_name[i] <= 'z') || identifier_name[i] == '_' ||
            (identifier_name[i] >= 'A' && identifier_name[i] <= 'Z') ||
            (identifier_name[i] >= '0' && identifier_name[i] <= '9')))
        {
            return false;
        }
    }
    return true;
}

bool SymbolTable::IsFullDigit(string value, string::size_type size_value)
{
    for (string::size_type i = 0; i < size_value; i++)
    {
        if (value[i] < '0' || value[i] > '9')
            return false;
    }
    return true;
    // return false: There is at least 1 symbol that is not a digit, return true: Good
}

bool SymbolTable::IsValidStringType(string value, string::size_type size_value)
{
    for (string::size_type i = 1; i < size_value - 1; i++)
    {
        if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'z')
            || (value[i] >= 'A' && value[i] <= 'Z') || value[i] == ' '))
            return false;
    }
    return true;
    // return false: Invalid, return true: Good
}

Node* SymbolTable::Search_No_Splay(string identifier_name, int BlockLevel)
{
    // search a node without slaying
    Node* searchNode = this->root;
    while (searchNode != NULL)
    {
        if (searchNode->BlockLevel != BlockLevel)
        {
            if (BlockLevel < searchNode->BlockLevel)
                searchNode = searchNode->left;
            else
                searchNode = searchNode->right;
        }
        else
        {
            if (identifier_name.compare(searchNode->identifier_name) < 0)
                searchNode = searchNode->left;
            else if (identifier_name.compare(searchNode->identifier_name) > 0)
                searchNode = searchNode->right;
            else
            {
                num_comp++;
                break;
            }
        }
        num_comp++;
    }
    return (searchNode == NULL) ? NULL : searchNode;
}

void SymbolTable::DeleteBlock(string identifier_name, int BlockLevel)
{
    Node* deleteNode = Search_No_Splay(identifier_name, BlockLevel);
    if (deleteNode == NULL)
        return;
    Splay(deleteNode); // splay it to root
    Node* leftTree = deleteNode->left;
    Node* rightTree = deleteNode->right;
    delete deleteNode;

    if (leftTree != NULL)
        leftTree->parent = NULL; 
    if (rightTree != NULL)
        rightTree->parent = NULL;
    
    // find node that has largest key on left tree and choose it as new root
    if (leftTree != NULL)
    {
        while (leftTree->right != NULL)
        {
            leftTree = leftTree->right;
        }
        Splay(leftTree);
        if (rightTree != NULL)
        {
            leftTree->right = rightTree;
            rightTree->parent = leftTree;
        }
        this->root = leftTree;
    }
    else
    {
        if (rightTree != NULL)
            this->root = rightTree;
        else
            this->root = NULL;
    }
}

void SymbolTable::Preorder(Node *root)
{
    if (root != NULL)
    {
        cout << " " << root->identifier_name << "//";
        cout << root->BlockLevel;
        Preorder(root->left);
        Preorder(root->right);
    }
}


void SymbolTable::run(string filename)
{
    //cout << "success";
    ifstream input(filename);

    string line;
    num_comp = 0;
    num_splay = 0;
    int BlockLevel = 0;
    string* idenName = new string [100]; // contains all identifier with BlockLevel >= 1
    int idenIndex = 0;

    if (input.is_open())
    {
        while (getline(input, line))
        {
            if (line[line.size() - 1] == ' ')
                throw InvalidInstruction(line);
            if (line == "BEGIN")
            {
                BlockLevel++;
                continue;
            }
            else if (line == "END")
            {
                BlockLevel--;
                if (BlockLevel < 0)
                    throw UnknownBlock();
                string level = to_string(BlockLevel + 1);
                for (int i = 0; i < idenIndex; i++)
                {
                    if (idenName[i].substr(0, 1) == level)
                    {
                        DeleteBlock(idenName[i].substr(1), BlockLevel + 1);
                    }
                }
                continue;
            }
            else if (line.substr(0, 7) == "LOOKUP ")
            {
                string identifier_name = line.substr(7);
                if (IsValidIdentifierName(identifier_name) == false)
                    throw InvalidInstruction(line);
                Node* searchNode = NULL;
                for (int i = BlockLevel; i >= 0; i--)
                {
                    searchNode = Search_No_Splay(identifier_name, i);
                    if (searchNode != NULL)
                        break;
                }
                if (searchNode == NULL)
                    throw Undeclared(line);
                Splay(searchNode);
                cout << searchNode->BlockLevel << '\n';
                continue;
            }
            else if (line == "PRINT")
            {
                if (this->root != NULL)
                {
                    cout << this->root->identifier_name << "//" << this->root->BlockLevel;
                    //this->root->left == NULL ? Preorder(this->root->right) : Preorder(this->root->left);
                    if (this->root->left != NULL)
                    {
                        Preorder(this->root->left);
                    }
                    if (this->root->right != NULL)
                    {
                        Preorder(this->root->right);
                    }
                }
                continue;
            }
            else if (line.substr(0, 7) == "INSERT " || line.substr(0, 7) == "ASSIGN ")
            {
                string command = line.substr(0, 6);
                if (command == "INSERT")
                {
                    string identifier_name, identifier_type, isStatic;
                    DivideLineInsert(line.substr(7), identifier_name, identifier_type, isStatic);
                    if (identifier_type == "" || isStatic == "")
                        throw InvalidInstruction(line);
                    
                    // Check if identifier type is valid
                    if (isStatic == "true")
                        CheckIdentifierType(line, identifier_type, 0);
                    else
                        CheckIdentifierType(line, identifier_type, BlockLevel);

                    // Check if identifier name is valid
                    int resCheck;
                    if (isStatic == "true")
                        resCheck = CheckIdentifierName(identifier_name, 0);
                    else
                        resCheck = CheckIdentifierName(identifier_name, BlockLevel);
                    if (resCheck == 0)
                        throw InvalidInstruction(line);
                    else if (resCheck == 1)
                        throw Redeclared(line);

                    // Everything is ok, add new node to splay tree
                    if (isStatic == "true")
                        add(identifier_name, identifier_type, 0);
                    else
                        add(identifier_name, identifier_type, BlockLevel);

                    if (BlockLevel > 0 && isStatic == "false")
                    {
                        idenName[idenIndex++] = to_string(BlockLevel) + identifier_name;
                    }
                }
                else
                {
                    string identifier_name, value;
                    DivideLineAssign(line.substr(7), identifier_name, value);
                   
                    // check if identifier name is valid
                    if (IsValidIdentifierName(identifier_name) == false)
                        throw InvalidInstruction(line);
                    string::size_type size_value = value.size();
                    Node* idenNode = NULL;
                    //int temp_comp = num_comp;
                    if (IsFullDigit(value, size_value) == true) // assign for number type
                    {
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            idenNode = Search_No_Splay(identifier_name, i);
                            if (idenNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        // check if identifier is undeclared
                        if (idenNode == NULL)
                            throw Undeclared(line);
                        // check if assigned identifier is number type
                        if (idenNode->identifier_type != "number")
                            throw TypeMismatch(line);
                    }
                    else if (value[0] == '\'' && value[size_value - 1] == '\'') // assign for string type
                    {
                        // check if a string to assign is valid
                        if (IsValidStringType(value, size_value) == false)
                            throw InvalidInstruction(line);
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            idenNode = Search_No_Splay(identifier_name, i);
                            if (idenNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        // check if identifier is undeclared
                        if (idenNode == NULL)
                            throw Undeclared(line);
                        // check if assigned identifier is string type
                        if (idenNode->identifier_type != "string")
                            throw TypeMismatch(line);
                    }
                    else if (IsValidIdentifierName(value) == true) // value is identifier
                    {
                        // check if value is undeclared
                        Node* valueNode = NULL;
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            valueNode = Search_No_Splay(value, i);
                            if (valueNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        if (valueNode == NULL)
                            throw Undeclared(line);
                        Splay(valueNode);
                        // check if identifier is undeclared
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            idenNode = Search_No_Splay(identifier_name, i);
                            if (idenNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        if (idenNode == NULL)
                            throw Undeclared(line);
                        // check if identifier and value have the same type
                        if (idenNode->identifier_type != valueNode->identifier_type)
                            throw TypeMismatch(line);
                    }
                    else
                    {
                        string name = "", type = ""; // name of function, type is parameters of function
                        for (string::size_type i = 0; i < size_value; i++)
                        {
                            if (value[i] == '(')
                            {
                                name = value.substr(0, i); // get name of function
                                type = value.substr(i); // get parameters of function
                                break;
                            }
                        }
                        if (name == "" || type == "") // check if this value is approriate
                            throw InvalidInstruction(line);
                        if (IsValidIdentifierName(name) == false) // check if name is valid
                            throw InvalidInstruction(line);
                        string::size_type sizeType = type.size();
                        if (type[sizeType - 1] != ')')
                            throw InvalidInstruction(line);
                        // get node of name
                        Node* nameNode = NULL;
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            nameNode = Search_No_Splay(name, i);
                            if (nameNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        if (nameNode == NULL)
                            throw Undeclared(line);
                        else
                        {
                            // check if name is function type
                            if (nameNode->identifier_type == "string" || nameNode->identifier_type == "number")
                                throw TypeMismatch(line);
                        }
                        Splay(nameNode);
                        
                        // check if there is invalid thing in type (variable)
                        string s = "";
                        string::size_type k1 = 1, m1 = 1;
                        while (k1 < sizeType)
                        {
                            bool check = false;
                            if (type[k1] == ',' || type[k1] == ')')
                            {
                                s = type.substr(m1, k1 - m1);
                                m1 = k1 + 1;
                                check = true;
                            }
                            k1++;
                            if (check == true)
                            {
                                string::size_type size_s = s.size();
                                if (s[0] == '\'' && s[0] == '\'')
                                {
                                    if (IsValidStringType(s, size_s) == false)
                                        throw InvalidInstruction(line);
                                }
                                else if (IsFullDigit(s, size_s) == false && IsValidIdentifierName(s) == false)
                                {
                                    throw InvalidInstruction(line);
                                }
                            }
                        }
                        

                        // For example: nameNode has function type is (number,string)->string
                        // then paraType = (number,string)    returnType = string
                        string paraType = nameNode->identifier_type.substr(0, nameNode->identifier_type.size() - 8);
                        string returnType = nameNode->identifier_type.substr(nameNode->identifier_type.size() - 6);

                        string::size_type sizePara = paraType.size();

                        // check if the number of parameters of type (variables) is different from paraType's one
                        string::size_type greaterSize = sizePara > sizeType ? sizePara : sizeType;
                        // simply check the number of comma (",") if they have the same number
                        int countCommaOfPara = 0, countCommaOfType = 0;
                        bool stopCheckPara = false, stopCheckType = false;
                        for (string::size_type i = 1; i < greaterSize - 1; i++)
                        {
                            if (stopCheckPara == false && stopCheckType == false)
                            {
                                if (paraType[i] == ',') 
                                    countCommaOfPara++;
                                if (type[i] == ',') 
                                    countCommaOfType++;
                            }
                            else if (stopCheckPara == true)
                            {
                                if (type[i] == ',') countCommaOfType++;
                            }
                            else if (stopCheckType == true)
                            {
                                if (paraType[i] == ',') countCommaOfPara++;
                            }
                            if (stopCheckPara == false)
                            {
                                if (paraType[i] == ')')
                                    stopCheckPara = true;
                            }
                            if (stopCheckType == false)
                            {
                                if (type[i] == ')')
                                    stopCheckType = true;
                            }
                        }
                        if (countCommaOfPara != countCommaOfType)
                            throw InvalidInstruction(line);

                        if (sizePara == 2 && sizeType == sizePara) // paraType = (), that means no parameter
                        {
                            for (int i = BlockLevel; i >= 0; i--)
                            {
                                int temp_comp = num_comp;
                                idenNode = Search_No_Splay(identifier_name, i);
                                if (idenNode != NULL)
                                    break;
                                else
                                    num_comp = temp_comp;
                            }
                            if (idenNode != NULL)
                            {
                                if (idenNode->identifier_type != returnType)
                                    throw TypeMismatch(line);
                            }
                            else
                                throw Undeclared(line);
                            goto Splay_Step;
                        }
                        else if (sizePara == 2 && sizeType != sizePara)
                            throw InvalidInstruction(line);
                        else 
                        {
                            string TypeToCheck = ""; // for each type in parameters of paraType, 
                                                     // we check if it is correspondingly approriate in type (variable)
                            for (string::size_type i = 1, j = 1, k = 1, m = 1; i < sizePara; i++)
                            {
                                bool check = false;
                                if (paraType[i] == ',' || paraType[i] == ')')
                                {
                                    TypeToCheck = paraType.substr(j, 6); // TypeToCheck is either "number" or "string" only
                                    j = i + 1;
                                    check = true; // we got TypeToCheck, let's check in type (variable) to see if its value has
                                                  // the same type with TypeToCheck
                                }
                                if (check == true)
                                {
                                    string val = "";
                                    while (true)
                                    {
                                        if (type[k] == ',' || type[k] == ')')
                                        {
                                            val = type.substr(m, k - m);
                                            m = k + 1;
                                            k++;
                                            break; // we got val, let's check if val an TypeToCheck have the same type
                                        }
                                        k++;
                                    }
                                    string::size_type size_val = val.size();
                                    if (IsFullDigit(val, size_val) == true)
                                    {
                                        if (TypeToCheck != "number")
                                            throw TypeMismatch(line);
                                    }
                                    else if (val[0] == '\'' && val[size_val - 1] == '\'')
                                    {
                                        if (TypeToCheck != "string")
                                            throw TypeMismatch(line);
                                    }
                                    else if (IsValidIdentifierName(val) == true)
                                    {
                                        Node* valNode = NULL;
                                        for (int i = BlockLevel; i >= 0; i--)
                                        {
                                            int temp_comp = num_comp;
                                            valNode = Search_No_Splay(val, i);
                                            if (valNode != NULL)
                                                break;
                                            else
                                                num_comp = temp_comp;
                                        }
                                        if (valNode == NULL)
                                            throw Undeclared(line);
                                        if (valNode->identifier_type != TypeToCheck)
                                            throw TypeMismatch(line);
                                        Splay(valNode);
                                    }
                                    //temp_comp++;
                                }
                            }
                        }
                        for (int i = BlockLevel; i >= 0; i--)
                        {
                            int temp_comp = num_comp;
                            idenNode = Search_No_Splay(identifier_name, i);
                            if (idenNode != NULL)
                                break;
                            else
                                num_comp = temp_comp;
                        }
                        if (idenNode == NULL)
                            throw Undeclared(line);
                        if (idenNode->identifier_type != returnType)
                            throw TypeMismatch(line);
                    }
Splay_Step:         Splay(idenNode);
                    //num_comp = temp_comp;
                }
            }
            else
                throw InvalidInstruction(line);
            //cout << "success\n";
            cout << num_comp << " " << num_splay << '\n';
            num_comp = num_splay = 0;
        }
        if (BlockLevel > 0)
            throw UnclosedBlock(BlockLevel);
        delete[] idenName;
        //Preorder(this->root);
        input.close();
    }
}