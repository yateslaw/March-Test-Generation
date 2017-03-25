#pragma once
#include "head.h"
class fault;
struct vec_fault_pair
{
	vector<fault> v_fault;
	vector<fault> a_fault;
};
class fault
{ 
	friend void fault_classify(vector<fault> &, std::map<string, vec_fault_pair>&);
	friend std::ostream &operator<<(std::ostream& os, const fault& f);
	friend bool operator==(const fault&rhs, const fault&lhs);
	friend fault get_fault(const string &s);
public:
	fault() = default;
	fault(string i, string s, string d, string f, string g) :
		i(i), s(s), d(d), f(f), g(g) { }//constructor 
	string get_s() { return s; }//get the  sensitize operations
	string get_d() { return d; }//get detective operation 
private:
/* test condition of memory fault
i£ºinitial state
s: sensitize operation
d: detect operation
f: fault state
g£ºexpect state
*/
	string i, s, d, f, g;
};
void read_fault(std::ifstream& input, vector<fault> &f);//read the fault from the input files
bool operator==(const fault&rhs,const fault&lhs);//overload the operator == 
fault get_fault(const string &s);
void fault_classify(vector<fault>&, std::map<string, vec_fault_pair>&);



