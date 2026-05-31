#include <iostream>
#include <string>
#include <sstream>
using namespace std;

enum Color { RED, BLACK };

struct Node {
    int val;
    Color color;
    Node *left, *right, *parent;
    Node(int v) : val(v), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

Node* root = nullptr;

bool isRed(Node* n) { return n && n->color == RED; }

Node* rotateLeft(Node* h) {
    Node* x = h->right;
    h->right = x->left;
    if (x->left) x->left->parent = h;
    x->parent = h->parent;
    if (!h->parent) root = x;
    else if (h == h->parent->left) h->parent->left = x;
    else h->parent->right = x;
    x->left = h;
    h->parent = x;
    x->color = h->color;
    h->color = RED;
    return x;
}

Node* rotateRight(Node* h) {
    Node* x = h->left;
    h->left = x->right;
    if (x->right) x->right->parent = h;
    x->parent = h->parent;
    if (!h->parent) root = x;
    else if (h == h->parent->right) h->parent->right = x;
    else h->parent->left = x;
    x->right = h;
    h->parent = x;
    x->color = h->color;
    h->color = RED;
    return x;
}

void flipColors(Node* h) {
    h->color = (h->color == RED) ? BLACK : RED;
    if (h->left) h->left->color = (h->left->color == RED) ? BLACK : RED;
    if (h->right) h->right->color = (h->right->color == RED) ? BLACK : RED;
}

Node* insertFix(Node* h, int val) {
    if (!h) return new Node(val);
    if (val < h->val) h->left = insertFix(h->left, val);
    else if (val > h->val) h->right = insertFix(h->right, val);
    else return h;

    if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
    if (isRed(h->left) && isRed(h->left ? h->left->left : nullptr)) h = rotateRight(h);
    if (isRed(h->left) && isRed(h->right)) flipColors(h);

    return h;
}

void insert(int val) {
    root = insertFix(root, val);
    root->color = BLACK;
}

Node* findMin(Node* n) {
    while (n->left) n = n->left;
    return n;
}

Node* deleteNode(Node* n, int val) {
    if (!n) return nullptr;
    if (val < n->val) {
        n->left = deleteNode(n->left, val);
    } else if (val > n->val) {
        n->right = deleteNode(n->right, val);
    } else {
        if (!n->left && !n->right) return nullptr;
        if (!n->left) { Node* r = n->right; r->color = n->color; delete n; return r; }
        if (!n->right) { Node* l = n->left; l->color = n->color; delete n; return l; }
        Node* m = findMin(n->right);
        n->val = m->val;
        n->right = deleteNode(n->right, m->val);
    }
    return n;
}

void deleteVal(int val) {
    root = deleteNode(root, val);
    if (root) root->color = BLACK;
}

bool search(Node* n, int val) {
    if (!n) return false;
    if (n->val == val) return true;
    return val < n->val ? search(n->left, val) : search(n->right, val);
}

string colorStr(Color c) { return c == RED ? "R" : "B"; }

string toJSON(Node* n) {
    if (!n) return "null";
    stringstream ss;
    ss << "{\"value\":" << n->val
       << ",\"color\":\"" << colorStr(n->color) << "\""
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
            insert(val);
            cout << toJSON(root) << endl;
        } else if (command == "DELETE") {
            int val; ss >> val;
            deleteVal(val);
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
