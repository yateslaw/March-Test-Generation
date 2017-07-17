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
	
	void print_mt();
private: 
	std::shared_ptr<vector<march_element>> me;
	memory m;
	string get_expect_state(const string &cell, const string &state);
	std::size_t GetIndexOfMe();
	//select the fault has contain the operation=s
	inline bool select_fault_process(vec_fault_pair &vf,const string &s,fault_pair &f);
	//map the v_fault to a_fault,get a fault pair
	inline bool map_fault(vec_fault_pair &vf,fault_pair &f);
	void updata_memory_state(string op);
	void del_accessed_fault(fault_pair f,vec_fault_pair &vfp);
};
#endif
