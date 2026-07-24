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

fstream readFile(const string &filename)
{
    fstream file(filename, ios::in);

    if (!file)
    {
        cout << "Couldn't open file.\n";
    }

    return file;
}

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

            
            if (left < n)
            {
                if (heap[left]->freq < heap[smallest]->freq)
                {
                    smallest = left;
                }
                else if (heap[left]->freq == heap[smallest]->freq)
                {
                    
                    if (heap[left]->character < heap[smallest]->character)
                    {
                        smallest = left;
                    }
                }
            }

            
            if (right < n)
            {
                if (heap[right]->freq < heap[smallest]->freq)
                {
                    smallest = right;
                }
                else if (heap[right]->freq == heap[smallest]->freq)
                {
                    
                    if (heap[right]->character < heap[smallest]->character)
                    {
                        smallest = right;
                    }
                }
            }

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
void huffmanalgo(unordered_map<char, string> &hfc, Node *root, string str)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->left == nullptr && root->right == nullptr)
    {
        if (str.empty())
        {
            str = "0";
        }

        hfc[root->character] = str;
        return;
    }

    huffmanalgo(hfc, root->left, str + "0");
    huffmanalgo(hfc, root->right, str + "1");
}

int main()

{
    vector<Node *> minHeap;
    unordered_map<char, string> huffmanCodes;

    char c;
    string filename;
    cin >> filename;

    unordered_map<char, int> character_frequency;

    fstream file = readFile(filename);

    if (!file)
    {
        return 1;
    }

    while (file.get(c))
    {
        if (character_frequency.find(c) != character_frequency.end())
        {
            character_frequency[c]++;
        }
        else
        {
            character_frequency[c] = 1;
        }
    }

    for (auto pair : character_frequency)
    {
        Node *newNode = new Node{
            pair.first,
            pair.second,
            nullptr,
            nullptr};

        insertheap(minHeap, newNode);
    }

    if (minHeap.empty())
    {
        cout << "File is empty.\n";
        return 0;
    }

    minHeaping(minHeap);

    while (minHeap.size() > 1)
    {
        picksmallest(minHeap);
    }

    Node *root = minHeap[0];
    huffmanalgo(huffmanCodes, root, "");

    cout << "Huffman Tree created successfully.\n";
    cout << "Root Frequency: " << root->freq << endl;

    ofstream compressedFile("compressed2.bin");

    if (!compressedFile)
    {
        cout << "Couldn't create output file.\n";
        return 1;
    }
    int uniqueCharacters = character_frequency.size();

    compressedFile.write(reinterpret_cast<char *>(&uniqueCharacters),
                         sizeof(uniqueCharacters));

    for (auto pair : character_frequency)
    {
        compressedFile.write(&pair.first, sizeof(char));

        compressedFile.write(reinterpret_cast<char *>(&pair.second),
                             sizeof(int));
    }
    int paddingBits = 0;

    streampos paddingPosition = compressedFile.tellp();

    compressedFile.write(reinterpret_cast<char *>(&paddingBits),
                         sizeof(paddingBits));

    file.close();
    file = readFile(filename);

    unsigned char buffer = 0;
    int bitCount = 0;

    while (file.get(c))
    {
        string code = huffmanCodes[c];

        for (int i = 0; i < code.length(); i++)
        {
            char bit = code[i];

            buffer <<= 1;

            if (bit == '1')
            {
                buffer |= 1;
            }

            bitCount++;

            if (bitCount == 8)
            {
                compressedFile.put(buffer);

                buffer = 0;
                bitCount = 0;
            }
        }
    }

    if (bitCount > 0)
    {
        paddingBits = 8 - bitCount;

        buffer <<= paddingBits;

        compressedFile.put(buffer);
    }
    else
    {
        paddingBits = 0;
    }
    compressedFile.seekp(paddingPosition);

    compressedFile.write(reinterpret_cast<char *>(&paddingBits),
                         sizeof(paddingBits));

    compressedFile.close();
    file.close();
    return 0;
}