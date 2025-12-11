//
// Created by Manju Muralidharan on 11/22/25.
//

#ifndef FA25EC3_TREE_H
#define FA25EC3_TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cctype>

using namespace std;

/*
   TEMPLATE TREE CLASS
   Multiple parents may point to the same child.
   Nodes are allocated dynamically and tracked in a map to avoid duplicates and
   ensure proper deletion without double-freeing.

   Provided:
   - Node<U> with id, data, children vector
   - Tree<T> with:
       createRoot(id, value)
       addNode(parentID, childID, value)
       findNode(id)
       printAll()
       playGame()
       ~Tree()
*/

template <typename U>
class Node {
public:
    string id;
    U data;
    vector<Node<U>*> children;

    Node(const string &nodeID, const U &value) : id(nodeID), data(value) {}
};

// Helper trimming functions (internal)
static inline string trim_copy(const string &s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

template <typename T>
class Tree {
private:
    Node<T>* root;
    // map of id -> pointer to node (owns the nodes)
    unordered_map<string, Node<T>*> nodesMap;

public:
    Tree() : root(nullptr) {}

    // Create root node (if it exists, update its data and set as root)
    void createRoot(const string &id, const T &value) {
        Node<T>* node = nullptr;
        auto it = nodesMap.find(id);
        if (it != nodesMap.end()) {
            node = it->second;
            // attempt to update data if needed (for string types it's OK)
            node->data = value;
        } else {
            node = new Node<T>(id, value);
            nodesMap[id] = node;
        }
        root = node;
    }

    // Add a child node under a parent. If nodes don't exist, they are created.
    // If child exists already, it's reused; parent->children will contain a pointer
    // to that same child (supporting multiple parents).
    void addNode(const string &parentID, const string &childID, const T &value) {
        // Ensure parent exists (create with default data if missing)
        Node<T>* parent = nullptr;
        auto pit = nodesMap.find(parentID);
        if (pit != nodesMap.end()) {
            parent = pit->second;
        } else {
            parent = new Node<T>(parentID, T()); // default-constructed data
            nodesMap[parentID] = parent;
        }

        // Ensure child exists (create with provided data if missing)
        Node<T>* child = nullptr;
        auto cit = nodesMap.find(childID);
        if (cit != nodesMap.end()) {
            child = cit->second;
            // Do not overwrite existing child data blindly. If child->data is default
            // constructed (common for non-string or empty string), it's risky to detect.
            // We keep existing data as-is to avoid overwriting previously set text.
        } else {
            child = new Node<T>(childID, value);
            nodesMap[childID] = child;
        }

        // If parent already has this child pointer, don't push duplicate
        bool already = false;
        for (Node<T>* pch : parent->children) {
            if (pch == child) { already = true; break; }
        }
        if (!already) parent->children.push_back(child);
    }

    // Find a node by id (returns nullptr if not found)
    Node<T>* findNode(const string &id) {
        auto it = nodesMap.find(id);
        if (it != nodesMap.end()) return it->second;
        return nullptr;
    }

    // Print all nodes in a readable format.
    // Attempts to sort by numeric id if ids are numeric; otherwise lexicographic.
    void printAll() {
        if (nodesMap.empty()) {
            cout << "Tree is empty." << endl;
            return;
        }

        // Collect ids
        vector<string> ids;
        ids.reserve(nodesMap.size());
        for (const auto &p : nodesMap) ids.push_back(p.first);

        // Prepare vector of pairs (original id, numeric value or large)
        vector<pair<string, long long>> numericKeys;
        numericKeys.reserve(ids.size());
        for (const string &s : ids) {
            bool allDigits = !s.empty();
            for (char c : s) if (!isdigit(static_cast<unsigned char>(c))) { allDigits = false; break; }
            if (allDigits) {
                try {
                    long long val = stoll(s);
                    numericKeys.emplace_back(s, val);
                } catch (...) {
                    numericKeys.emplace_back(s, LLONG_MAX);
                }
            } else {
                numericKeys.emplace_back(s, LLONG_MAX);
            }
        }

        // Sort: numeric ascending first (by numeric value), then lexicographic for ties
        sort(numericKeys.begin(), numericKeys.end(),
             [](const pair<string,long long> &a, const pair<string,long long> &b) {
                 if (a.second != b.second) return a.second < b.second;
                 return a.first < b.first;
             });

        cout << "===== Story Tree =====" << endl;
        for (const auto &p : numericKeys) {
            const string &id = p.first;
            Node<T>* node = nodesMap[id];
            string dataStr;
            // Convert data to string representation if T is string; else use stringstream
            if constexpr (is_same<T, string>::value) {
                dataStr = trim_copy(node->data);
            } else {
                stringstream ss;
                ss << node->data;
                dataStr = trim_copy(ss.str());
            }

            cout << "Node " << id << ": " << dataStr << endl;
            if (node->children.empty()) {
                cout << "  Child -> (none)" << endl;
            } else {
                for (Node<T>* c : node->children) {
                    cout << "  Child -> " << c->id << endl;
                }
            }
            cout << endl;
        }
        cout << "======================" << endl;
    }

    // Play the story from the root node.
    // Behavior:
    //  - Display current node text
    //  - Display numbered options (using children's text)
    //  - Prompt for selection, validate input
    //  - Move to selected child
    //  - Stop when a node has no children
    void playGame() {
        if (!root) {
            cout << "No root node. Cannot play game." << endl;
            return;
        }

        Node<T>* current = root;

        cout << "===== Begin Adventure =====" << endl << endl;

        while (true) {
            // Print current node text
            string currentText;
            if constexpr (is_same<T, string>::value) {
                currentText = trim_copy(current->data);
            } else {
                stringstream ss;
                ss << current->data;
                currentText = trim_copy(ss.str());
            }

            cout << currentText << endl;

            if (current->children.empty()) {
                cout << "There are no further paths." << endl;
                cout << "Your journey ends here." << endl << endl;
                break;
            }

            // Show numbered options
            cout << "Choose your next action:" << endl;
            for (size_t i = 0; i < current->children.size(); ++i) {
                Node<T>* child = current->children[i];
                string childText;
                if constexpr (is_same<T, string>::value) {
                    childText = trim_copy(child->data);
                } else {
                    stringstream ss;
                    ss << child->data;
                    childText = trim_copy(ss.str());
                }
                // For readability only show first sentence if very long? Keep whole text per spec.
                cout << (i + 1) << ". " << childText << endl;
            }

            // Get user selection
            cout << "Selection: ";
            string input;
            if (!getline(cin, input)) {
                // EOF or error -> exit the game loop
                cout << endl << "Input error or EOF. Ending adventure." << endl;
                break;
            }
            input = trim_copy(input);
            if (input.empty()) {
                cout << "Please enter a number corresponding to your choice." << endl;
                continue;
            }

            // parse number
            bool validNumber = true;
            for (char ch : input) if (!isdigit(static_cast<unsigned char>(ch))) { validNumber = false; break; }
            if (!validNumber) {
                cout << "Invalid selection. Please enter a number." << endl;
                continue;
            }

            int choice = 0;
            try {
                choice = stoi(input);
            } catch (...) {
                cout << "Invalid selection. Please enter a valid number." << endl;
                continue;
            }

            if (choice < 1 || static_cast<size_t>(choice) > current->children.size()) {
                cout << "Choice out of range. Please select a valid option." << endl;
                continue;
            }

            // Move to selected child
            current = current->children[choice - 1];
            cout << endl;
        }

        cout << "===== Adventure Complete =====" << endl;
    }

    // Destructor: delete all allocated nodes exactly once
    ~Tree() {
        for (auto &p : nodesMap) {
            delete p.second;
        }
        nodesMap.clear();
        root = nullptr;
    }
};

#endif //FA25EC3_TREE_H
