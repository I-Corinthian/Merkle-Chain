#include <iostream>
#include <string>
#include <vector>
#include <cryptopp/sha.h>
#include "cryptopp/filters.h"
#include "cryptopp/hex.h"

const int block_size = 16; // the block size

class MerkleTree
{
private:
    std::vector<std::string> data;
    std::string node_root;

public:
    void add_data(std::string d)
    {
        data.push_back(d);
        if (data.size() == block_size)
        {
            generate_root();
        }
    }

    std::string hash(std::string input)
    {
        CryptoPP::SHA256 hash;
        std::string result;
        CryptoPP::StringSource s(input, true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(result))));
        return result;
    }

    void generate_root()
    {
        if (data.empty())
        {
            return;
        }

        std::vector<std::string> temp_data(data);
        while (temp_data.size() > 1)
        {
            std::vector<std::string> new_temp_data;
            for (unsigned int i = 0; i < temp_data.size(); i += 2)
            {
                std::string left = temp_data[i];
                std::string right = (i + 1 == temp_data.size() ? temp_data[i] : temp_data[i + 1]);
                std::string combined = left + right;
                new_temp_data.push_back(hash(combined));
            }
            temp_data = new_temp_data;
        }

        node_root = temp_data[0];
    }

    std::string get_root() const
    {
        return node_root;
    }
};

class MerkleChain
{
private:
    struct Node
    {
        MerkleTree tree;
        Node *next;
    };

    Node *head;
    Node *tail;

public:
    MerkleChain()
    {
        head = tail = nullptr;
    }

    ~MerkleChain()
    {
        Node *current = head;
        while (current != nullptr)
        {
            Node *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void add_data(std::string d)
    {
        if (head == nullptr)
        {
            head = tail = new Node();
        }

        tail->tree.add_data(d);
        if (tail->tree.get_root() != "")
        {
            Node *new_node = new Node();
            tail->next = new_node;
            tail = new_node;
        }
    }
};

int main()
{
    MerkleChain chain;
    std::string data;
    while (true)
    {
        std::cout << "Enter data (q to quit): ";
       std::cin >> data;
        if (data == "q")
        {
            break;
        }

        chain.add_data(data);
    }

    return 0;
}

