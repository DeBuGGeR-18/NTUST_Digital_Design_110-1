#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <map>

using namespace std;
int main(int argc, char* argv[])
{
	ifstream file;
	ofstream out_file;
	string temp;
	//file.open("case5.kiss");
	file.open(argv[1]);

	// .start_kiss
	file >> temp;
	// .i 1
	int in_num;
	file >> temp;
	file >> in_num;
	// .o 1
	int out_num;
	file >> temp;
	file >> out_num;
	// .p 14
	int terms;
	file >> temp;
	file >> terms;
	// .s 7
	int states;
	file >> temp;
	file >> states;
	// .r a
	string reset;
	file >> temp;
	file >> reset;

	// build up a states table
	map<string, vector<pair<string, pair<string, string> > > >table;
	for (int i = 0; i < terms; i++)
	{
		string in, out;
		string state, next;
		file >> in >> state >> next >> out;
		pair<string, pair<string, string> > p;
		pair<string, string> io(in, out);
		p.first = next;
		p.second = io;
		table[state].push_back(p);
	}

	// .end_kiss
	file >> temp;
	file.close();
	
	// output input.dot
	ofstream in_dot;
	in_dot.open("input.dot");
	//in_dot.open(argv[2]);

	in_dot << "digraph STG {" << endl;
	in_dot << "\trankdir=LR;" << endl << endl;
	in_dot << "\tINIT [shape=point];" << endl;
	for (auto a : table)
	{
		in_dot << "\t" << a.first << " [label=\"" << a.first << "\"];" << endl;
	}
	in_dot << endl;
	in_dot << "\tINIT -> " << reset << ";" << endl;
	for (auto a : table)
	{
		vector<pair<string, vector<string> > > vt;
		for (auto b : a.second)
		{
			bool flag = false;
			int index = 0;
			for (int i = 0; i < vt.size(); i++)
			{
				if (vt[i].first == b.first)
				{
					flag = true;
					index = i;
					break;
				}
			}
			string s = b.second.first + "/" + b.second.second;
			if (flag) vt[index].second.push_back(s);
			else
			{
				vector<string> v{ s };
				pair<string, vector<string> > p(b.first, v);
				vt.push_back(p);
			}
		}
		for (auto b : vt)
		{
			in_dot << "\t" << a.first << " -> " << b.first << " [label=\"";
			for (int i = 0; i < b.second.size(); i++)
			{
				if (i == b.second.size() - 1) in_dot << b.second[i];
				else in_dot << b.second[i] << ",";
			}
			in_dot << "\"];" << endl;
		}
	}
	in_dot << "}" << endl;

	// build up a reducing table
	map<string, map<string, vector<pair<string, string> > > > reducing;
	int index = 0;
	for (auto a : table)
	{
		++index;
		int _index = 0;
		for (auto b : table)
		{
			++_index;
			if (index < _index)
			{
				string output1 = "";
				string output2 = "";
				for (auto c : a.second)
				{
					output1 += c.second.second;
				}
				for (auto c : b.second)
				{
					output2 += c.second.second;
				}
				if (output1 == output2 && !output1.empty() && !output2.empty())
				{
					map<string, vector<pair<string, string> > > m = reducing[a.first];
					vector<pair<string, string> > v(pow(2,in_num));
					pair<string, string> p;
					for (int i = 0; i < b.second.size(); i++)
					{
						v[i].first = b.second[i].first;
					}
					for (int i = 0; i < a.second.size(); i++)
					{
						v[i].second = a.second[i].first;
					}
					m[b.first] = v;
					reducing[a.first] = m;
				}
			}
		}
	}
	/*for (auto a : reducing)
	{
		cout << a.first << ":" << endl;
		for (auto b : a.second)
		{
			cout << " " << b.first << ": ";
			for (auto c : b.second)
			{
				cout << c.first << "-" << c.second;
				cout << ", ";
			}
			cout << endl;
		}
		cout << endl;
	}
	cout << "=============================================" << endl;*/
	bool reduced = true;
	while (reduced)
	{
		reduced = false;
		for (auto a : reducing)
		{
			for (auto b : a.second)
			{
				for (auto c : b.second)
				{
					string state1 = c.first;
					string state2 = c.second;
					//cout << state1 << "  " << state2 << endl;
					if (state1 == state2) continue;
					/*for (auto d : reducing[state1][state2])
					{
						cout << d.first << "  " << d.second << endl;
					}
					for (auto d : reducing[state2][state1])
					{
						cout << d.first << "  " << d.second << endl;
					}
					cout << "--------------------------------------" << endl;*/
					if (reducing[state1][state2].empty() && reducing[state2][state1].empty())
					{
						
						reducing[a.first][b.first].clear();
						reduced = true;
					}
				}
			}
		}
	}
	/*cout << "=============================================" << endl;
	for (auto a : reducing)
	{
		cout << a.first << ":" << endl;
		for (auto b : a.second)
		{
			cout << " " << b.first << ": ";
			for (auto c : b.second)
			{
				cout << c.first << "-" << c.second;
				cout << ", ";
			}
			cout << endl;
		}
		cout << endl;
	}*/
	for (auto a : reducing)
	{
		for (auto b : a.second)
		{
			if (!b.second.empty())
			{
				string s = b.first;
				table.erase(s);
				for (auto c : table)
				{
					vector<pair<string, pair<string, string> > > v = c.second;
					for (int i = 0; i < v.size(); i++)
					{
						if (v[i].first == s)
						{
							v[i].first = a.first;
						}
					}
					table[c.first] = v;
				}
			}
		}
	}
	//for (auto a : table)
	//{
	//	cout << a.first << ":  ";
	//	for (auto b : a.second)
	//	{
	//		cout << b.first << " ";
	//		cout << b.second.first << " ";
	//		cout << b.second.second << " ";
	//	}
	//	cout << endl;
	//}

	// output result.kiss
	out_file.open("result.kiss");
	out_file << ".start_kiss" << endl;
	out_file << ".i " << in_num << endl;
	out_file << ".o " << out_num << endl;
	out_file << ".p " << table.size() * pow(2, in_num) << endl;
	out_file << ".s " << table.size() << endl;
	out_file << ".r " << reset << endl;
	for (auto a : table)
	{
		for (int i = 0; i < a.second.size(); i++)
		{
			out_file << a.second[i].second.first << " ";
			out_file << a.first << " ";
			out_file << a.second[i].first << " ";
			out_file << a.second[i].second.second << endl;
		}
	}
	out_file << ".end_kiss";
	out_file.close();

	// output output.dot
	ofstream out_dot;
	out_dot.open("output.dot");
	//out_dot.open(argv[3]);

	out_dot << "digraph STG {" << endl;
	out_dot << "\trankdir=LR;" << endl << endl;
	out_dot << "\tINIT [shape=point];" << endl;
	for (auto a : table)
	{
		out_dot << "\t" << a.first << " [label=\"" << a.first << "\"];" << endl;
	}
	out_dot << endl;
	out_dot << "\tINIT -> " << reset << ";" << endl;
	for (auto a : table)
	{
		vector<pair<string, vector<string> > > vt;
		for (auto b : a.second)
		{
			bool flag = false;
			int index = 0;
			for (int i = 0; i < vt.size(); i++)
			{
				if (vt[i].first == b.first)
				{
					flag = true;
					index = i;
					break;
				}
			}
			string s = b.second.first + "/" + b.second.second;
			if (flag) vt[index].second.push_back(s);
			else
			{
				vector<string> v{ s };
				pair<string, vector<string> > p(b.first, v);
				vt.push_back(p);
			}
		}
		for (auto b : vt)
		{
			out_dot << "\t" << a.first << " -> " << b.first << " [label=\"";
			for (int i = 0; i < b.second.size(); i++)
			{
				if (i == b.second.size() - 1) out_dot << b.second[i];
				else out_dot << b.second[i] << ",";
			}
			out_dot << "\"];" << endl;
		}
	}
	out_dot << "}" << endl;
}