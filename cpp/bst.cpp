#include <iostream>
#include <string>
#include <sstream>
using namespace std;

struct Node {
    int val;
    Node* left;
    Node* right;
    Node(int v) : val(v), left(nullptr), right(nullptr) {}
};

Node* root = nullptr;

Node* insert(Node* node, int val) {
    if (!node) return new Node(val);
    if (val < node->val) node->left = insert(node->left, val);
    else if (val > node->val) node->right = insert(node->right, val);
    return node;
}

Node* findMin(Node* node) {
    while (node->left) node = node->left;
    return node;
}

Node* deleteNode(Node* node, int val) {
    if (!node) return nullptr;
    if (val < node->val) node->left = deleteNode(node->left, val);
    else if (val > node->val) node->right = deleteNode(node->right, val);
    else {
        if (!node->left) return node->right;
        if (!node->right) return node->left;
        Node* temp = findMin(node->right);
        node->val = temp->val;
        node->right = deleteNode(node->right, temp->val);
    }
    return node;
}

bool search(Node* node, int val) {
    if (!node) return false;
    if (node->val == val) return true;
    return val < node->val ? search(node->left, val) : search(node->right, val);
}

string toJSON(Node* node) {
    if (!node) return "null";
    stringstream ss;
    ss << "{\"value\":" << node->val
       << ",\"left\":" << toJSON(node->left)
       << ",\"right\":" << toJSON(node->right) << "}";
    return ss.str();
}

int main() {
    string line;
    while (getline(cin, line)) {
        stringstream ss(line);
        string command;
        ss >> command;
        if (command == "INSERT") {
            int val; ss >> val;
            root = insert(root, val);
            cout << toJSON(root) << endl;
        } else if (command == "DELETE") {
            int val; ss >> val;
            root = deleteNode(root, val);
            cout << toJSON(root) << endl;
        } else if (command == "SEARCH") {
            int val; ss >> val;
            cout << (search(root, val) ? "true" : "false") << endl;
        } else if (command == "GET") {
            cout << toJSON(root) << endl;
        } else {
            cout << "null" << endl;
        }
    }
    return 0;
}
