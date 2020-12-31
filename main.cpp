#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>


typedef unsigned long long ull;
const int BUFFER_SIZE = 4096;
const int LEAF_NODE = -1;

struct Node {
	unsigned char m_value;
	ull m_counter;
	int m_left_index;
	int m_right_index;
	int m_index;
};

std::vector<Node> nodes;
int index_counter = 0;


struct Comparator {

	bool operator()(const ull lhs, const ull rhs) {
		return nodes[lhs].m_counter > nodes[rhs].m_counter;
	}
};


// Creates new node, return it's index
int create_node(unsigned char value, ull counter, int left, int right) {

	Node temp;
	temp.m_value = value;
	temp.m_counter = counter;
	temp.m_index = index_counter++;
	temp.m_left_index = left;
	temp.m_right_index = right;

	nodes.push_back(temp);
	return temp.m_index;
}


// Loads characters and their frequencies to the unordered map
void load_data(std::ifstream& file, std::unordered_map<unsigned char, ull>& char_frequency) {

	
	if (file.peek() == std::ifstream::traits_type::eof()) {
		std::cout << "File is empty ...\n";
		exit(1);
	}

	char buffer[BUFFER_SIZE];
	size_t size = 0;

	while (true) {

		file.read(buffer, BUFFER_SIZE);
		size = file.gcount();

		for (size_t i = 0; i < size; ++i) {
			char_frequency[+static_cast<unsigned char>(buffer[i])]++;
		}
		if (size < BUFFER_SIZE) {
			break;
		}
	}
}


// Creates huffman code for all charcters
void generate_code(int current, std::string code, std::map<unsigned char, std::string>& char_codes) {

	if (nodes[current].m_left_index == LEAF_NODE && nodes[current].m_right_index == LEAF_NODE) {
		char_codes[nodes[current].m_value] = code;
		return;
	}

	generate_code(nodes[current].m_left_index, code + "0", char_codes);
	generate_code(nodes[current].m_right_index, code + "1", char_codes);
}


// Testing function, checks size of original and encoded text (not 100% correct)
void check_size(std::ifstream& file, std::map<unsigned char, std::string>& char_codes) {

	char buffer[BUFFER_SIZE];
	size_t size = 0;
	std::string text = "";
	
	file.clear();
	file.seekg(0, std::ios::beg);

	size_t encoded_size = 0;
	size_t origin_size = 0;

	while (true) {

		file.read(buffer, BUFFER_SIZE);
		size = file.gcount();
		origin_size += size * sizeof(char);

		for (size_t i = 0; i < size; ++i) {
			text += char_codes[buffer[i]];
		}
		encoded_size += text.size()/8;
		text = "";

		if (size < BUFFER_SIZE) {
			break;
		}
	}
	std::cout << "\nOriginal size: " << origin_size <<  " B\n";
	std::cout << "Encoded size:  " << encoded_size << " B\n";
}


void huffman(std::ifstream& file) {

	std::unordered_map<unsigned char, ull> char_frequency;
	load_data(file, char_frequency);

	// Create Min Heap and insert node indexes into it
	std::priority_queue<int, std::vector<int>, Comparator> queue;

	for (auto& node : char_frequency) {
		queue.push(create_node(node.first, node.second, LEAF_NODE, LEAF_NODE));
	}

	while (queue.size() != 1) {

		auto left = queue.top();
		queue.pop();
		auto right = queue.top();
		queue.pop();

		auto counter = nodes[left].m_counter + nodes[right].m_counter;
		queue.push(create_node('\0', counter, left, right));
	}

	// map containing Huffman's code of all occuring characters
	std::map<unsigned char, std::string> char_codes;
	auto root = queue.top();
	generate_code(root, "", char_codes);

	for (auto& i : char_codes) {
		std::cout << +i.first << ": " << i.second << "\n";
	}

	// Testing, not 100% correct, just to see effectivity of Huffman's code
	//check_size(file, char_codes);
}


int main(int argc, char* argv[]) {

	std::ifstream in_file;

	if (argc == 2) {
		in_file.open(argv[1], std::ios::binary);
		if (!in_file.is_open()) {
			std::cout << "Error: " << argv[1] << " coudn't be opened ...\n";
			return 1;
		}
	}
	else {
		std::cout << "Invalid number of parameters ...\n";
		return 1;
	}

	huffman(in_file);

	return 0;
}
