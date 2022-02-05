#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <stdlib.h>

using namespace std;

string reduce_literal(string a, string b)	// reduce n-literal term to n-1 "s term
{
	int diff = 0;
	int index = -1;
	string output = "";
	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
		{
			++diff;
			index = i;
		}
	}
	if (diff == 1)
	{
		for (int i = 0; i < a.size(); i++)
		{
			if (i == index) output += "-";
			else output += a[i];
		}
	}
	return output;
}

void find_literal(vector<string> &a,string b)	// find literal term form input lines
{
	size_t index = b.find("-");
	if (index == string::npos)
	{
		a.push_back(b);
		return;
	}
	string x = b.replace(index, 1, "0");
	string y = b.replace(index, 1, "1");
	find_literal(a, x);
	find_literal(a, y);
}

void sort_reduce(string& s)
{
	sort(s.begin(), s.end());
	string t;
	t += s[0];
	for (int i = 1; i < s.size(); i++)
	{
		bool duplicate = false;
		for (int j = 0; j < t.size(); j++)
		{
			if (s[i] == t[j])
			{
				duplicate = true;
				break;
			}
		}
		if (!duplicate) t += s[i];
	}
	s = t;
}


int main(int argc, char* argv[])
{
	ifstream file;
	ofstream out_file;
	string temp;
	//file.open("6input.pla");
	file.open(argv[1]);

	// .i 4
	int in_num = 0;
	file >> temp >> in_num;

	// .o 1
	int out_num = 0;
	file >> temp >> out_num;

	// .ilb a b c d
	vector<string>in_var;
	file >> temp;
	for (int i = 0; i < in_num; i++)
	{
		file >> temp;
		in_var.push_back(temp);
	}

	// .ob f
	vector<string>out_var;
	file >> temp;
	for (int i = 0; i < out_num; i++)
	{
		file >> temp;
		out_var.push_back(temp);
	}

	/*  .p 7
		00-0 1
		0-11 1
		1-01 1
		0101 1
		1111 -
		100- 1
		-01- 1  
	*/
	file >> temp;
	int lines;
	char result;
	file >> lines;
	vector<string>rules;
	vector<char>results;
	vector<string>dont_care;
	for (int i = 0; i < lines; i++)
	{
		file >> temp >> result;
		rules.push_back(temp);
		results.push_back(result);
		if (result == '-') dont_care.push_back(temp);
	}

	// .e
	file >> temp;
	if (temp == ".e") file.close();

	//Let's start a jounery
	vector<string>literal;
	vector<int>minterms;
	set<string>exact_literal;
	for (int i = 0; i < lines; i++)		// use input file to find the minterm
	{
		string t = rules[i];
		find_literal(literal, t);
	}
	for (int i = 0; i < literal.size(); i++)	// reduce the duplicate minterm
	{
		exact_literal.insert(literal[i]);
	}
	vector<vector<pair<string,bool> > >group_literal(in_num + 1);
	for (auto a : exact_literal)				// group up the literal with how many 1 in it
	{
		int count_1 = count(a.begin(), a.end(), '1');
		pair<string, bool> p(a, true);
		group_literal[count_1].push_back(p);
	}
	string term;
	bool _prime = true;
	int dash = 0;
	while (_prime)
	{
		_prime = false;
		for (int i = 0; i < in_num - dash; i++)
		{
			int size = group_literal[i].size();
			for (int j = 0; j < size; j++)
			{
				if (count(group_literal[i][j].first.begin(), group_literal[i][j].first.end(), '-') == dash)
				{
					for (int k = 0; k < group_literal[i + 1].size(); k++)
					{	
						if (count(group_literal[i + 1][k].first.begin(), group_literal[i + 1][k].first.end(), '-') == dash)
						{
							term = reduce_literal(group_literal[i][j].first, group_literal[i + 1][k].first);
							if (term != "")
							{
								_prime = true;
								group_literal[i][j].second = false;
								group_literal[i + 1][k].second = false;
								pair<string, bool> p(term, true);
								group_literal[i].push_back(p);
							}
						}
					}
				}
			}
		}
		++dash;
	}
	vector<string> dontcare_literal;
	for (int i = 0; i < dont_care.size(); i++)
	{
		find_literal(dontcare_literal, dont_care[i]);
	}
	set<string> prime_implicant;
	for (int i = 0; i < group_literal.size(); i++)
	{
		for (int j = 0; j < group_literal[i].size(); j++)
		{
			if (group_literal[i][j].second == true)
			{
				bool dontcare = false;
				for (int k = 0; k < dontcare_literal.size(); k++)
				{
					if (group_literal[i][j].first == dontcare_literal[k])
					{
						dontcare = true;
						break;
					}
				}
				if (!dontcare) prime_implicant.insert(group_literal[i][j].first);
			}
		}
	}
	vector<vector<string> >PI_table(prime_implicant.size());
	int table_size = 0;
	for (auto a : prime_implicant)
	{
		PI_table[table_size].push_back(a);
		find_literal(PI_table[table_size], a);
		++table_size;
	}
	vector<vector<string> >PI(prime_implicant.size());
	for (int i = 0; i < PI_table.size(); i++)
	{
		for (int j = 0; j < PI_table[i].size(); j++)
		{
			bool dontcare = false;
			for (int k = 0; k < dontcare_literal.size(); k++)
			{
				if (PI_table[i][j] == dontcare_literal[k])
				{
					dontcare = true;
					break;
				}
			}
			if (!dontcare) PI[i].push_back(PI_table[i][j]);
		}
	}
	set<string>count;
	for (int i = 0; i < PI.size(); i++)
	{
		for (int j = 1; j < PI[i].size(); j++)
		{
			count.insert(PI[i][j]);
		}
	}
	vector<vector<string> >prime_table(prime_implicant.size() + 1);
	prime_table[0].push_back("");
	for (auto a : count)
	{
		prime_table[0].push_back(a);
	}
	int p = 1;
	for (auto a : prime_implicant)
	{
		prime_table[p].push_back(a);
		++p;
	}
	for (int i = 1; i < prime_table.size(); i++)
	{
		for (int j = 1; j < prime_table[0].size(); j++)
		{
			for (int k = 1; k < PI[i - 1].size(); k++)
			{
				if (prime_table[0][j] == PI[i - 1][k])
				{
					prime_table[i].push_back("true");
					break;
				}
				if(k == PI[i - 1].size() - 1) prime_table[i].push_back("false");
			}
		}
	}
	vector<string>EPI;
	vector<int>epi_row;
	for (int i = 1; i < prime_table[0].size(); i++)
	{
		int epi_count = 0;
		int j, _j;
		for (j = 1; j < prime_table.size(); j++)
		{
			if (prime_table[j][i] == "true")
			{
				++epi_count;
				_j = j;
			}
		}
		if (epi_count == 1)
		{
			EPI.push_back(prime_table[_j][0]);
			epi_row.push_back(_j);
		}
	}
	set<int>reduce_column;
	for (int i = 0; i < epi_row.size(); i++)
	{
		for (int j = 1; j < prime_table[0].size(); j++)
		{
			if (prime_table[epi_row[i]][j] == "true")
			{
				reduce_column.insert(j);
			}
		}
	}
	if (reduce_column.size() != prime_table[0].size() - 1)
	{
		int final_term = prime_table[0].size() - reduce_column.size() - 1;
		vector<vector<string> >final_table(final_term + 2);
		for (int j = 1; j < prime_table.size(); j++)
		{
			bool not_epi = true;
			for (int k : epi_row)
			{
				if (j == k)
				{
					not_epi = false;
					break;
				}
			}
			if (not_epi)
			{
				final_table[0].push_back(prime_table[j][0]);
			}
		}
		int final_row = 1;
		for (int i = 1; i < prime_table[0].size(); i++)
		{
			bool is_final = true;
			for (int k : reduce_column)
			{
				if (k == i)
				{
					is_final = false;
					break;
				}
			}
			if (is_final)
			{
				for (int j = 1; j < prime_table.size(); j++)
				{
					bool not_epi = true;
					for (int k : epi_row)
					{
						if (j == k)
						{
							not_epi = false;
							break;
						}
					}
					if (not_epi)
					{
						final_table[final_row].push_back(prime_table[j][i]);
					}
				}
				++final_row;
			}
		}
		string symbol[26] = { "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z" };
		for (int i = 0; i < final_table[0].size(); i++)
		{
			final_table[final_table.size() - 1].push_back((symbol[i]));
		}

		vector<vector<string> >function(final_table.size() - 2);
		for (int i = 0; i < function.size(); i++)
		{
			for (int j = 0; j < final_table[0].size(); j++)
			{
				if (final_table[i + 1][j] == "true")
				{
					function[i].push_back(final_table[final_term + 1][j]);
				}
			}
		}
		for (int i = 0; i < function.size() - 1; i++)
		{
			vector<string>temp;
			for (int j = 0; j < function[i].size(); j++)
			{
				for (int k = 0; k < function[i + 1].size(); k++)
				{
					string t = function[i][j] + function[i + 1][k];
					temp.push_back(t);
				}
			}
			function[i + 1] = temp;
		}
		for (int i = 0; i < function[function.size() - 1].size(); i++)
		{
			sort_reduce(function[function.size() - 1][i]);
		}
		set<string>final_result;
		for (string s : function[function.size() - 1]) final_result.insert(s);
		string minimum;
		int length = 99;
		for (string s : final_result)
		{
			if (s.size() < length)
			{
				length = s.size();
				minimum = s;
			}
		}
		for (int i = 0; i < minimum.size(); i++)
		{
			string s;
			s.push_back(minimum[i]);
			for (int j = 0; j < final_table[0].size(); j++)
			{
				if (final_table[final_table.size() - 1][j] == s)
				{
					EPI.push_back(final_table[0][j]);
				}
			}
		}
	}
	
	//out_file.open("output_6input.pla");
	out_file.open(argv[2]);

	out_file << ".i " << in_num << endl;
	out_file << ".o " << out_num << endl;

	out_file << ".lib ";
	for (int i = 0; i < in_num; i++)
	{
		out_file << in_var[i] << " ";
	}
	out_file << endl;

	out_file << ".ob ";
	for (int i = 0; i < out_num; i++)
	{
		out_file << out_var[i] << " ";
	}
	out_file << endl;

	set<string>_EPI;
	for (int i = 0; i < EPI.size(); i++)
	{
		_EPI.insert(EPI[i]);
	}
	out_file << ".p " << _EPI.size() << endl;
	for (string s : _EPI) out_file << s << " 1" << endl;

	out_file << ".e" << endl;

}