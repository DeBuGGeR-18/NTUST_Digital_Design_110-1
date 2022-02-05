#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;
int main(int argc, char* argv[])
{
	ifstream file;
	ofstream out_file;
	string temp;
	//file.open("05.pla");
	file.open(argv[1]);

	//.i 3
	int in_num = 0;
	file >> temp >> in_num;
	
	int node_num = (int)pow(2, in_num + 1);
	/*int** node = new int* [node_num];
	for (int i = 0; i < 2; i++)
		node[i] = new int[2];*/
	int node[512][2];

	for (int i = 0; i < node_num; i++)
	{
		node[i][0] = i + 1;		// serial number of node starting with 1
		node[i][1] = 0;			// initialize to zero
	}

	for (int i = (node_num / 2) - 1; i < node_num - 1; i++)
		node[i][0] = 999;		//999 & 1000 means '0' & '1' respectively

	//.o 1
	int out_num = 0;
	file >> temp >> out_num;

	//.ilb a b c
	vector<string>in_var;
	file >> temp;
	for (int i = 0; i < in_num; i++)
	{
		file >> temp;
		in_var.push_back(temp);
	}

	//.ob F
	vector<string>out_var;
	file >> temp;
	for (int i = 0; i < out_num; i++)
	{
		file >> temp;
		out_var.push_back(temp);
	}

	// .p 2
	// 11- 1
	// --1 1
	file >> temp;
	int lines = 0, result = 0;
	file >> lines;
	string rule;
	vector<int>out_bit;
	for (int i = 0; i < lines; i++)
	{
		file >> rule >> result;
		out_bit.push_back(0);
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j] == '0')
				for (unsigned int k = 0; k < out_bit.size(); k++)
					out_bit[k] *= 2;

			else if (rule[j] == '1')
				for (unsigned int k = 0; k < out_bit.size(); k++)
					(out_bit[k] *= 2)++;		//*2 +1

			else if (rule[j] == '-')
			{
				int size = out_bit.size();
				for (int k = 0; k < size; k++)
				{
					out_bit[k] *= 2;
					out_bit.push_back(out_bit[k] + 1);
				}	
			}	
		}
		/*for (auto a : out_bit) cout << a << " ";
		cout << endl;*/
		for (unsigned int j = 0; j < out_bit.size(); j++)
		{
			if (result == 1) node[(node_num / 2) - 1 + out_bit[j]][0] = 1000;
			else node[(node_num / 2) - 1 + out_bit[j]][0] = 999;
		}
		out_bit.clear();
	}
	/*for (int i = 0; i < node_num - 1; i++)
	{
		cout << node[i][0] << " ";
	}
	cout << endl;*/
	
	//.e
	file >> temp;
	if (temp == ".e") file.close();


	//generate ROBDD
	for (int i = 0; i < (int)pow(2, in_num - 1) - 1; i++)
	{
		node[i][0] = (i + 1) * 2;
		node[i][1] = (i + 1) * 2 + 1;
	}
	for (int i = (int)pow(2, in_num-1) - 1; i < (int)pow(2, in_num) - 1; i++)
	{
		node[i][0] = node[i * 2 + 1][0];
		node[i][1] = node[i * 2 + 2][0];
	}
	/*for (int i = 0; i < (int)pow(2, in_num) - 1; i++)
	{
		cout << node[i][0] << " " << node[i][1];
		cout << endl;
	}*/

	bool same_node = true;
	bool same_value = true;
	while (same_node || same_value)
	{
		same_node = false;
		same_value = false;
		for (int i = 0; i < (int)pow(2, in_num) - 1; i++)
		{
			if (node[i][0] == node[i][1])
			{
				same_value = true;
				int index = i + 1;
				for (int j = 0; j < (int)pow(2, in_num) - 1; j++)
				{
					if (node[j][0] == index) node[j][0] = node[i][0];
					if (node[j][1] == index) node[j][1] = node[i][1];
				}
				/*if (index % 2 == 0 && node[index / 2 - 1][0] > 0)
					node[index / 2 - 1][0] = node[i][0];

				else node[(index - 3) / 2][1] = node[i][1];*/

				node[i][0] = -1;
				node[i][1] = -2;
			}
		}
		for (int k = 1; k < in_num; k++)
		{
			for (int i = (int)pow(2, k) - 1; i < (int)pow(2, k + 1) - 1; i++)
			{
				for (int j = i + 1; j < (int)pow(2, k + 1) - 1; j++)
				{
					if (node[i][0] == node[j][0] && node[i][1] == node[j][1] && node[i][0] > 0)
					{
						same_node = true;
						int index = j + 1;
						for (int l = 0; l < (int)pow(2, in_num) - 1; l++)
						{
							if (node[l][0] == index) node[l][0] = i+1;
							if (node[l][1] == index) node[l][1] = i+1;
						}
						/*if (index % 2 == 0)
							node[index / 2 - 1][0] = i + 1;

						else node[(index - 3) / 2][1] = i + 1;*/

						node[j][0] = -1;
						node[j][1] = -2;
					}
				}
			}
		}
	}
	/*for (int i = 0; i < (int)pow(2, in_num) - 1; i++)
	{
		cout << node[i][0] << " " << node[i][1];
		cout << endl;
	}*/

	//output a dot file
	//out_file.open(out_var[0] + ".dot");
	out_file.open(argv[2]);
	out_file << "digraph ROBDD {\n";
	for (int i = 0; i < in_num; i++)
	{
		bool have_value = false;
		int index = (int)pow(2, i) - 1;
		for (int j = 0; j < (int)pow(2, i); j++)
		{
			if (node[index][0] > 0) have_value = true;
			index++;
		}
		if(have_value) out_file << "   {rank=same";
		index = (int)pow(2, i) - 1;
		for (int j = 0; j < (int)pow(2, i); j++)
		{
			if (node[index][0] > 0) out_file << " " << index + 1;
			index++;
		}
		if (have_value) out_file << "}\n";
	}
	out_file << "\n";

	out_file << "   0 [label=\"0\", shape=box]\n";
	string color[] = { "red","blue","green","gold","pink" };
	for (int i = 0; i < in_num; i++)
	{
		for (int j = (int)pow(2, i) - 1; j < (int)pow(2, i + 1) - 1; j++)
		{
			if (node[j][0] > 0)
				out_file <<"   " << j + 1 << " [label=\"" << in_var[i] << "\", color=\""<<color[i]<<"\"]\n";
		}
	}
	out_file <<"   " << (int)pow(2, in_num) << " [label=\"1\", shape=box]\n";
	out_file << "\n";

	string s0 = " [label=\"0\", style=dotted]";
	string s1 = " [label=\"1\", style=solid]";
	int max = (int)pow(2, in_num);
	for (int i = 0; i < (int)pow(2, in_num) - 1; i++)
	{
		if (node[i][0] > 0)
		{
			if (node[i][0] == 999)
				out_file << "   " << i + 1 << " -> " << "0" << s0 << "\n";
			else if (node[i][0] == 1000)
				out_file << "   " << i + 1 << " -> " << max << s0 << "\n";
			else out_file << "   " << i + 1 << " -> " << node[i][0] << s0 << "\n";

			if (node[i][1] == 999)
				out_file << "   " << i + 1 << " -> " << "0" << s1 << "\n";
			else if (node[i][1] == 1000)
				out_file << "   " << i + 1 << " -> " << max << s1 << "\n";
			else out_file << "   " << i + 1 << " -> " << node[i][1] << s1 << "\n";
		}
	}
	out_file << "}";
	out_file.close();
}