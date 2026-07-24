#include <iostream>
#include <thread>
#include <vector>
// #include <mutex>
#include <atomic>

constexpr int NUM_THREADS = 4;
constexpr int INSERTIONS_PER_THREAD = 10000;

class LinkedList {
private:
    struct Node {
        int val;
        Node* nextNode;
        Node(int v, Node* next = nullptr) : val(v), nextNode(next) {}
    };
    std::atomic<Node*> head{nullptr};

public:
    // destructor
    ~LinkedList() {
        while (head != nullptr) {
            Node* temp = head;
            head = temp->nextNode;
            delete temp;
        }
    }

    void addBeginningNode(int val) {
        Node* newNode = new Node(val);
        newNode->nextNode = head.load(std::memory_order_relaxed);

        // Compare-and-Swap (CAS) - atomic check-and-update hardware instr.
        while(!head.compare_exchange_weak(
                newNode->nextNode, // expected value
                newNode,           // desired value
                std::memory_order_release, // success memory ordering
                std::memory_order_relaxed  // failure memory ordering
        )) {
            // if fails, newNode->nextNode updated to current head,
            // (loop retries on failure)
        }
    }

    // void addEndNode(int val) {
    //     Node* newNode = new Node(val);
    //     std::lock_guard<std::mutex> lock(listLock);
    //     if (head == nullptr) {
    //         head = newNode;
    //         return;
    //     };
    //     Node* temp = head;
    //     while (temp->nextNode != nullptr) {
    //         temp = temp->nextNode;
    //     }
    //     temp->nextNode = newNode;
    // }

    void deleteFirst(){
        if (head == nullptr) {
            std::cout << "empty list\n";
            return;
        }
        struct Node* temp = head;
        head = temp->nextNode;
        delete temp;
    }

    void print() const {
        Node* temp = head;
        while (temp != nullptr) {
            std::cout << temp->val << " -> ";
            temp = temp->nextNode;
        }
        std::cout << "NULL\n";
    }

    int countNodes() {
        int count = 0;
        Node* temp = head;
        while (temp != nullptr) {
            count++;
            temp = temp->nextNode;
        }
        return count;
    }
};

LinkedList list;

void threadInsert() {
    for (int i = 0; i < INSERTIONS_PER_THREAD; i++) {
        list.addBeginningNode(i);
    }
}

int main() {
    std::vector<std::thread> threads;
    threads.reserve(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(threadInsert);
    }
    for (auto& t: threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    int expected = NUM_THREADS * INSERTIONS_PER_THREAD;
    int actual = list.countNodes();

    std::cout << "******TEST********\n";
    std::cout << "Expected nodes: " << expected << "\n";
    std::cout << "Actual nodes:   " << actual << "\n";

    if (actual != expected) {
        std::cout << "RACE CONDITION DETECTED!\n";
    } else {
        std::cout << "SUCCESS: All nodes safely inserted with lock-free system!\n";
    }

    return 0;
}