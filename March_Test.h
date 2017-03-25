#ifndef MARCH_TEST_H
#define MARCH_TEST_H
#include "fault.h"
#include <numeric>
#include <ctime>
#include <memory>
struct memory
{
	string get_state() { return state_of_a_cell + state_of_v_cell; }//get the current state of memory 
	string state_of_a_cell; 
	string state_of_v_cell;
	string accessed_cell; 
};//a two-cell memory 
struct march_element
{
	string addr_order;
	vector<string> operations;//the operations of March Test 
	bool is_access;//the flag of whether the March element is accessed or not.
};//March element of March Test 
struct fault_pair
{
	fault _v;
	fault _a;
};//definition of fault_pair 
class March_Test
{
public:
	March_Test(vector<march_element> me = { { "A",{"W0"},false },{ "D",{ "*" },true },
		{ "D",{ "*" },false },{ "U",{ "*" },false },
		{ "U",{ "*" },false },{ "A",{ "R0"},false } },
		memory m = { "0","0","v" })
		:me(std::make_shared<vector<march_element>>(me)), m(m) { }//constructor of March_Test 
	//get a fault pair of the fault map
	vec_fault_pair get_fault_set(std::map<string, vec_fault_pair> &,size_t &);
	//get the test condition of the selected fault pair 
    fault_pair get_fault_tc(vec_fault_pair &, size_t &,size_t &,size_t &);
    //add the sensitive and dectective operations to the  accessed ME 
	void add_op_to_mt(fault_pair f, size_t &is_sens,size_t &flag); 
	
	void delete_fault(fault_pair f, vec_fault_pair & vfp,size_t &trans_flag,size_t &flag);
	
	void print_mt()
	{
		vector<string> mt;
		mt.push_back((*me)[0].addr_order+" "+"W0");
		for (int i = 1; i < (*me).size()-1; ++i)
		{
			mt.push_back((*me)[i].addr_order+" "+accumulate(((*me)[i]).operations.cbegin(),
				((*me)[i]).operations.cend(), string(" ")).substr(2));
		}
		mt.push_back((*me)[5].addr_order +" "+ "R0");
		for (auto e : mt)
			std::cout << e << std::endl;
	}
private: 
	std::shared_ptr<vector<march_element>> me;
	memory m;
	string get_expect_state(const string &cell, const string &state)
	{
		if (cell == "v")
		{
			return (state.substr(1) == "0") ? state.substr(0, 1) + "1" : state.substr(0, 1) + "0";
		}
		else
		{
			return (state.substr(0, 1) == "0") ? "1" + state.substr(1) : "0" + state.substr(1);
		}

	}
	std::size_t GetIndexOfMe() 
	{
	
		for (size_t i = 0; i <= (*me).size();++i)
		{
			if ((*me)[i].is_access) return i;
		}
		return 0;
	}
	//select the fault has contain the operation=s
	inline bool select_fault_process(vec_fault_pair &vf,const string &s,fault_pair &f) 
	{
		for (auto it = vf.v_fault.begin(); it != vf.v_fault.end(); ++it)
		{
			if ((*it).get_s().substr(1,s.size()) == s)
			{
				f._v = *it;
				return true;
			}
		}
		return false;
	}
	//map the v_fault to a_fault,get a fault pair
	inline bool map_fault(vec_fault_pair &vf,fault_pair &f)
	{
		string sens = "a"+f._v.get_s().substr(1);
		for (auto it = vf.a_fault.begin(); it != vf.a_fault.end(); ++it)
		{
			if ((*it).get_s() == sens)
			{
				f._a = (*it);
				return true;
			}
		}
		return false;
	}
	void updata_memory_state(string op)
	{
		if (m.accessed_cell == "v")
		{
			for (int i = 0; i <op.size(); i = i + 2)
			{
				if (op.substr(i, 2) == "R0"|| op.substr(i, 2) == "W0")
				{
					m.state_of_v_cell = "0";
				}
				else if (op.substr(i, 2) == "R1" || op.substr(i, 2) == "W1")
				{
					m.state_of_v_cell = "1";
				}
			}
		}
		else
		{
			for (int i = 0; i <op.size(); i = i + 2)
			{
				if (op.substr(i, 2) == "R0" || op.substr(i, 2) == "W0")
				{
					m.state_of_a_cell = "0";
				}
				else if (op.substr(i, 2) == "R1" || op.substr(i, 2) == "W1")
				{
					m.state_of_a_cell = "1";
				}
			}
		}
	}
	void del_accessed_fault(fault_pair f,vec_fault_pair &vfp)
	{
		auto it = vfp.v_fault.begin();
		while (it != vfp.v_fault.end())
		{
			if ((*it) == f._v)
				it = vfp.v_fault.erase(it);
			else
				++it;
		}
		auto it1 = vfp.a_fault.begin();
		while (it1 != vfp.a_fault.end())
		{
			if ((*it1) == f._a)
				it1 = vfp.a_fault.erase(it1);
			else
				++it1;
		}
	}
};
#endif
