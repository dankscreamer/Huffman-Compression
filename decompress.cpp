#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

struct Node
{
    char character;
    int freq;
    Node *left;
    Node *right;
};

void insertheap(vector<Node *> &heap, Node *newNode)
{
    heap.push_back(newNode);
}

void minHeaping(vector<Node *> &heap)
{
    int n = heap.size();
    int start = (n / 2) - 1;

    while (start >= 0)
    {
        int current = start;

        while (true)
        {
            int left = (2 * current) + 1;
            int right = (2 * current) + 2;

            int smallest = current;

            if (left < n && heap[left]->freq < heap[smallest]->freq)
                smallest = left;

            if (right < n && heap[right]->freq < heap[smallest]->freq)
                smallest = right;

            if (smallest == current)
                break;

            swap(heap[current], heap[smallest]);
            current = smallest;
        }

        start--;
    }
}

void picksmallest(vector<Node *> &heap)
{
    if (heap.size() < 2)
        return;

    Node *minnode = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty())
        minHeaping(heap);

    Node *minnode2 = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty())
        minHeaping(heap);

    Node *combined_node = new Node;
    combined_node->character = '\0';
    combined_node->freq = minnode->freq + minnode2->freq;
    combined_node->left = minnode;
    combined_node->right = minnode2;

    insertheap(heap, combined_node);
    minHeaping(heap);
}

int main()
{
    ifstream compressedFile("compressed2.bin", ios::binary);
    if (!compressedFile)
    {
        cout << "Coudn't open compressed file.\n";
        return 1;
    }
    int uniqueCharacters;
    compressedFile.read(reinterpret_cast<char *>(&uniqueCharacters),
                        sizeof(uniqueCharacters));
    unordered_map<char, int> character_frequency;
    for (int i = 0; i < uniqueCharacters; i++)
    {
        char ch;
        int freq;

        compressedFile.read(&ch, sizeof(char));

        compressedFile.read(reinterpret_cast<char *>(&freq),
                            sizeof(int));

        character_frequency[ch] = freq;
    }
    int paddingBits;

    compressedFile.read(reinterpret_cast<char *>(&paddingBits),
                        sizeof(paddingBits));

    vector<Node *> minHeap;

    for (auto pair : character_frequency)
    {
        Node *newNode = new Node{
            pair.first,
            pair.second,
            nullptr,
            nullptr};

        insertheap(minHeap, newNode);
    }

    minHeaping(minHeap);

    while (minHeap.size() > 1)
    {
        picksmallest(minHeap);
    }

    Node *root = minHeap[0];

    ofstream outputFile("decompressed.txt", ios::binary);

    Node *current = root;

    unsigned char byte;

    compressedFile.seekg(0, ios::end);
    int fileSize = compressedFile.tellg();

    int metadataSize = sizeof(int) +
                       uniqueCharacters * (sizeof(char) + sizeof(int)) +
                       sizeof(int);

    int compressedBytes = fileSize - metadataSize;

    compressedFile.seekg(metadataSize);

    for (int j = 0; j < compressedBytes; j++)
    {
        compressedFile.read(reinterpret_cast<char *>(&byte), sizeof(byte));

        int bitsToRead = 8;

        if (j == compressedBytes - 1)
        {
            bitsToRead = 8 - paddingBits;
        }

        for (int i = 7; i >= 8 - bitsToRead; i--)
        {
            int bit = (byte >> i) & 1;

            if (bit == 0)
            {
                current = current->left;
            }
            else
            {
                current = current->right;
            }

            if (current->left == nullptr && current->right == nullptr)
            {
                outputFile.put(current->character);
                current = root;
            }
        }
    }

    outputFile.close();
}
