#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <fstream> // Required for Append-Only Logging

using namespace std;

struct Contact {
    string name;
    string phone;
    string email;
};

struct Node {
    Contact contact;
    int height;
    Node *left, *right;
    Node(Contact c) : contact(c), height(1), left(nullptr), right(nullptr) {}
};

Node* root = nullptr;
const string LOG_FILE = "engine_transactions.log";

// --- CORE AVL MATHEMATICS ---
int height(Node* n) { return n ? n->height : 0; }
int bf(Node* n) { return n ? height(n->left) - height(n->right) : 0; }

void updateHeight(Node* n) {
    if (n) n->height = 1 + max(height(n->left), height(n->right));
}

Node* rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y; y->left = T2;
    updateHeight(y); updateHeight(x);
    return x;
}

Node* rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x; x->right = T2;
    updateHeight(x); updateHeight(y);
    return y;
}

Node* balance(Node* n) {
    updateHeight(n);
    int b = bf(n);
    if (b > 1)  { if (bf(n->left)  < 0) n->left  = rotateLeft(n->left);  return rotateRight(n); }
    if (b < -1) { if (bf(n->right) > 0) n->right = rotateRight(n->right); return rotateLeft(n);  }
    return n;
}

// --- ENGINE LOGGING SUB-SYSTEM (Level 3 Feature) ---
void logTransaction(const string& command, const string& name, const string& phone = "", const string& email = "") {
    ofstream log(LOG_FILE, ios::app);
    if (log.is_open()) {
        log << command << " " << name;
        if (!phone.empty()) log << " " << phone << " " << email;
        log << "\n";
    }
}

// --- DATABASE OPERATIONS ---
Node* insert(Node* n, Contact c) {
    if (!n) return new Node(c);
    string lo = c.name, lo2 = n->contact.name;
    transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
    transform(lo2.begin(), lo2.end(), lo2.begin(), ::tolower);
    if (lo < lo2) n->left = insert(n->left, c);
    else if (lo > lo2) n->right = insert(n->right, c);
    else return n; 
    return balance(n);
}

Node* findMin(Node* n) {
    while (n->left) n = n->left;
    return n;
}

Node* deleteNode(Node* n, string name) {
    if (!n) return nullptr;
    string lo = name, lo2 = n->contact.name;
    transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
    transform(lo2.begin(), lo2.end(), lo2.begin(), ::tolower);
    if (lo < lo2) n->left = deleteNode(n->left, name);
    else if (lo > lo2) n->right = deleteNode(n->right, name);
    else {
        if (!n->left || !n->right) {
            Node* tmp = n->left ? n->left : n->right;
            delete n; return tmp;
        }
        Node* tmp = findMin(n->right);
        n->contact = tmp->contact;
        n->right = deleteNode(n->right, tmp->contact.name);
    }
    return balance(n);
}

// --- ACYCLIC SEARCH ENGINES ---
Node* searchByName(Node* n, const string& name) {
    if (!n) return nullptr;
    string lo = name, lo2 = n->contact.name;
    transform(lo.begin(), lo.end(), lo.begin(), ::tolower);
    transform(lo2.begin(), lo2.end(), lo2.begin(), ::tolower);
    if (lo == lo2) return n;
    return lo < lo2 ? searchByName(n->left, name) : searchByName(n->right, name);
}

void collectAll(Node* n, vector<Node*>& out) {
    if (!n) return;
    collectAll(n->left, out);
    out.push_back(n);
    collectAll(n->right, out);
}

// --- STARTUP LOG REPLAY CRASH RECOVERY ---
void recoverFromCrash() {
    ifstream log(LOG_FILE);
    if (!log.is_open()) return;
    string cmd, name, phone, email;
    while (log >> cmd) {
        if (cmd == "TX_INSERT") {
            log >> name >> phone >> email;
            replace(name.begin(), name.end(), '_', ' ');
            root = insert(root, {name, phone, email});
        } else if (cmd == "TX_DELETE") {
            log >> name;
            replace(name.begin(), name.end(), '_', ' ');
            root = deleteNode(root, name);
        }
    }
}

// --- JSON UTILITIES ---
string jsonEsc(const string& s) {
    string r;
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}

string toJSON(Node* n) {
    if (!n) return "null";
    stringstream ss;
    ss << "{\"value\":\"" << jsonEsc(n->contact.name) << "\""
       << ",\"phone\":\"" << jsonEsc(n->contact.phone) << "\""
       << ",\"email\":\"" << jsonEsc(n->contact.email) << "\""
       << ",\"height\":" << n->height
       << ",\"bf\":" << bf(n)
       << ",\"left\":" << toJSON(n->left)
       << ",\"right\":" << toJSON(n->right) << "}";
    return ss.str();
}

string contactJSON(const Contact& c) {
    stringstream ss;
    ss << "{\"name\":\"" << jsonEsc(c.name) << "\""
       << ",\"phone\":\"" << jsonEsc(c.phone) << "\""
       << ",\"email\":\"" << jsonEsc(c.email) << "\"}";
    return ss.str();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    // Execute Level 3 recovery scan on microservice startup
    recoverFromCrash();

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "INSERT") {
            string n, p, e;
            ss >> n >> p >> e;
            logTransaction("TX_INSERT", n, p, e); // Write-Ahead Commit
            replace(n.begin(), n.end(), '_', ' ');
            root = insert(root, {n, p, e});
            cout << toJSON(root) << endl;

        } else if (command == "DELETE") {
            string n; ss >> n;
            logTransaction("TX_DELETE", n); // Write-Ahead Commit
            replace(n.begin(), n.end(), '_', ' ');
            root = deleteNode(root, n);
            cout << toJSON(root) << endl;

        } else if (command == "SEARCH_NAME") {
            string n; ss >> n;
            replace(n.begin(), n.end(), '_', ' ');
            Node* found = searchByName(root, n);
            if (found) cout << contactJSON(found->contact) << endl;
            else cout << "null" << endl;

        } else if (command == "SEARCH_PHONE") {
            string query; ss >> query;
            vector<Node*> all;
            collectAll(root, all);
            bool found = false;
            for (Node* nd : all) {
                if (nd->contact.phone == query) {
                    cout << contactJSON(nd->contact) << endl;
                    found = true; break;
                }
            }
            if (!found) cout << "null" << endl;

        } else if (command == "SEARCH_EMAIL") {
            string query; ss >> query;
            vector<Node*> all;
            collectAll(root, all);
            bool found = false;
            for (Node* nd : all) {
                string lo1 = nd->contact.email, lo2 = query;
                transform(lo1.begin(), lo1.end(), lo1.begin(), ::tolower);
                transform(lo2.begin(), lo2.end(), lo2.begin(), ::tolower);
                if (lo1 == lo2) {
                    cout << contactJSON(nd->contact) << endl;
                    found = true; break;
                }
            }
            if (!found) cout << "null" << endl;

        } else if (command == "UPDATE") {
            string oldN, newN, p, e;
            ss >> oldN >> newN >> p >> e;
            logTransaction("TX_DELETE", oldN);
            logTransaction("TX_INSERT", newN, p, e);
            replace(oldN.begin(), oldN.end(), '_', ' ');
            replace(newN.begin(), newN.end(), '_', ' ');
            root = deleteNode(root, oldN);
            root = insert(root, {newN, p, e});
            cout << toJSON(root) << endl;

        } else if (command == "GET") {
            cout << toJSON(root) << endl;

        } else if (command == "LIST") {
            vector<Node*> all;
            collectAll(root, all);
            cout << "[";
            for (size_t i = 0; i < all.size(); i++) {
                if (i) cout << ",";
                cout << contactJSON(all[i]->contact);
            }
            cout << "]" << endl;

        } else if (command == "CLEAR_LOG") {
            remove(LOG_FILE.c_str());
            root = nullptr;
            cout << "null" << endl;
        } else {
            cout << "null" << endl;
        }
    }
    return 0;
}
