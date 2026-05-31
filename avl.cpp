#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

struct Node {
    int val, height;
    Node* left;
    Node* right;
    Node(int v) : val(v), height(1), left(nullptr), right(nullptr) {}
};

Node* root = nullptr;

int height(Node* n) { return n ? n->height : 0; }
int bf(Node* n) { return n ? height(n->left) - height(n->right) : 0; }

void updateHeight(Node* n) {
    if (n) n->height = 1 + max(height(n->left), height(n->right));
}

Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* T = x->right;
    x->right = y;
    y->left = T;
    updateHeight(y);
    updateHeight(x);
    return x;
}

Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T = y->left;
    y->left = x;
    x->right = T;
    updateHeight(x);
    updateHeight(y);
    return y;
}

Node* balance(Node* n) {
    updateHeight(n);
    int b = bf(n);
    if (b > 1) {
        if (bf(n->left) < 0) n->left = rotateLeft(n->left);
        return rotateRight(n);
    }
    if (b < -1) {
        if (bf(n->right) > 0) n->right = rotateRight(n->right);
        return rotateLeft(n);
    }
    return n;
}

Node* insert(Node* n, int val) {
    if (!n) return new Node(val);
    if (val < n->val) n->left = insert(n->left, val);
    else if (val > n->val) n->right = insert(n->right, val);
    else return n;
    return balance(n);
}

Node* findMin(Node* n) {
    while (n->left) n = n->left;
    return n;
}

Node* deleteNode(Node* n, int val) {
    if (!n) return nullptr;
    if (val < n->val) n->left = deleteNode(n->left, val);
    else if (val > n->val) n->right = deleteNode(n->right, val);
    else {
        if (!n->left) return n->right;
        if (!n->right) return n->left;
        Node* m = findMin(n->right);
        n->val = m->val;
        n->right = deleteNode(n->right, m->val);
    }
    return balance(n);
}

bool search(Node* n, int val) {
    if (!n) return false;
    if (n->val == val) return true;
    return val < n->val ? search(n->left, val) : search(n->right, val);
}

string toJSON(Node* n) {
    if (!n) return "null";
    stringstream ss;
    ss << "{\"value\":" << n->val
       << ",\"height\":" << n->height
       << ",\"bf\":" << bf(n)
       << ",\"left\":" << toJSON(n->left)
       << ",\"right\":" << toJSON(n->right) << "}";
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