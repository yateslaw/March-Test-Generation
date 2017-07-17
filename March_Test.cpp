#include "March_Test.h"

vec_fault_pair March_Test::get_fault_set(std::map<string, vec_fault_pair> &f_m,size_t &trans_flag)
{
	string init_state = m.get_state();
	string flip_state = get_expect_state(m.accessed_cell, m.get_state());//the flip state of memory
	string non_flip_v = m.accessed_cell + "_" + init_state + "_" + init_state;//the key of fault map
	string flip_v = m.accessed_cell + "_" + init_state + "_" + flip_state;//the key of fault map
	vec_fault_pair tmp;//a temporary variable 
	// select the fault pair with the key=non_filp_v with priority
	if (f_m.find(non_flip_v) != f_m.end() && (*f_m.find(non_flip_v)).second.v_fault.size())
	{
		tmp.v_fault = f_m[non_flip_v].v_fault;
		tmp.a_fault = f_m[non_flip_v].a_fault;
		f_m.erase(non_flip_v); 
		return tmp;
	}
	// if the fault pair with the key=non_filp_v is empty,
	//select the fault pair with the key=filp_v;
	else
	{
		tmp.v_fault = f_m[flip_v].v_fault;
		tmp.a_fault = f_m[flip_v].a_fault;
		trans_flag = 1;//the flag of alternate the March element
		f_m.erase(flip_v);
		return tmp;
	}
}
fault_pair March_Test::get_fault_tc(vec_fault_pair &vfp, size_t &is_sens,size_t &trans_flag,size_t &flag)
{
	fault_pair select_fault_pair;
	size_t index = GetIndexOfMe();
	//get the operations of the acessed ME
	string Ops_of_ME = accumulate(((*me)[index]).operations.cbegin(),
			((*me)[index]).operations.cend(), string("*")).substr(2);
	size_t sens_size = vfp.v_fault.back().get_s().substr(1).size();//get the type of the fault 
	if (vfp.v_fault.size() == 1 && trans_flag)//when traverse the last fault of the fault set 
	{
		flag = 1; 
	}
	if (!Ops_of_ME.size())//when the ME have none of operations 
	{
		is_sens = 0; 
		//select the fault of with different priority
		if (select_fault_process(vfp, "R0", select_fault_pair))
		{ 
			map_fault(vfp, select_fault_pair);
			return select_fault_pair; 
		}
		else if(select_fault_process(vfp, "W0", select_fault_pair))
		{
			map_fault(vfp, select_fault_pair);
			return select_fault_pair;
		}
		else if (select_fault_process(vfp, "R1", select_fault_pair))
		{
			map_fault(vfp, select_fault_pair);
			return select_fault_pair;
		}
		else if (select_fault_process(vfp, "W1", select_fault_pair))
		{
			map_fault(vfp, select_fault_pair);
			return select_fault_pair;
		}
	}
	else
	{
		string op_last = Ops_of_ME.substr(Ops_of_ME.size()-2,2);//get the last operation of ME 
		srand((unsigned)time(NULL));
		size_t num = rand() % (vfp.v_fault.size());// a random number 
		if (select_fault_process(vfp, op_last, select_fault_pair))//if get a fault with operation=op_last 
		{
			map_fault(vfp, select_fault_pair);
			is_sens = 1;
			return select_fault_pair;
		}
		else//randomly return a fault 
		{
			is_sens = 0;
			select_fault_pair._v = vfp.v_fault[num];
			map_fault(vfp, select_fault_pair);
			return select_fault_pair;
		}
					
	}
	
}

void March_Test::add_op_to_mt(fault_pair f, size_t &is_sens,size_t &flag)
{
	string op;
	march_element *acc_me = &(*me)[GetIndexOfMe()];//accessed ME(March Element) 
	march_element *acc_me_next = &(*me)[GetIndexOfMe()+1];//next accessed ME 
	//deal with the fault sensitized by v cell(v_fault) 
	if (!flag)
	{
	    if(!is_sens)// if select a fault  not be sensitized
	    {
	    	is_sens=0;
			//add the sensitive and dectective operation to the ME
			op = f._v.get_s().substr(1) + f._v.get_d().substr(1); 
		}
		else if(is_sens==1)// if select a fault be sensitized
		{
			is_sens=0;
			//just add the dectective operation to the ME
			op = f._v.get_d().substr(1);
		}
	    
	}
	else//when traverse the last fault of the fault set 
	{
		flag = 0;
		op =  f._v.get_s().substr(1);//just add the sensitive operation to the ME
	}
	(*acc_me).operations.push_back(op);
	updata_memory_state(op);//update the memory 
	//deal with the fault sensitized by a cell(a_fault) 
	if ((*acc_me).addr_order == "D")//if address order is down
	{
		string Ops_of_Next_ME = accumulate((*acc_me_next).operations.cbegin(),
			(*acc_me_next).operations.cend(), string("*")).substr(2);//get the operations of next ME
		if (!Ops_of_Next_ME.size())//if the next ME has not any operation
		{
			//add the dectective operation to the next ME 
			(*acc_me_next).operations.push_back(f._a.get_d().substr(1));
		}
		else //if the next ME has operations
		{
			//if the first operation of the next ME is equal to the dectective operation of a_fault 
			//return derectly
			if (Ops_of_Next_ME.substr(0, 2) == f._a.get_d().substr(1))
				return;
			else//add the dectective operation to the next ME
				(*acc_me_next).operations.push_back(f._a.get_d().substr(1));
		}
	}
	else//while the address order is up
	{
		string Ops_of_ME = accumulate((*acc_me).operations.cbegin(),
			(*acc_me).operations.cend(), string("*")).substr(2);
		if (Ops_of_ME.substr(0, 2) == f._a.get_d().substr(1))
		{
			return;//
		}
		else//add the dectective operation of a_fault to the first position of ME
		{
			(*acc_me).operations.insert((*acc_me).operations.begin() + 1,
				f._a.get_d().substr(1));
		}
	}
}

void March_Test::delete_fault(fault_pair f, vec_fault_pair & vfp,size_t &trans_flag,size_t &flag)
{
	del_accessed_fault(f,vfp);//delete the traversed fault 
	if (!vfp.v_fault.size() && !vfp.a_fault.size())//while the selected fault pair is empty 
	{
		if (trans_flag)// alternate to the next March element and upadate the memory
		{
			trans_flag = 0;
			if (m.get_state() == "01")
			{
				m.state_of_a_cell = m.state_of_v_cell = "1";
			}
			else if (m.get_state() == "10")
			{
				m.state_of_a_cell = "1";
				m.state_of_v_cell = "0";
			}
			else if (m.get_state() == "11")
			{
				m.state_of_a_cell = "0";
				m.state_of_v_cell = "1";
			}
			size_t index = GetIndexOfMe();
			(*me)[index].is_access = false;
			(*me)[index + 1].is_access = true;
		}
	}
}

void March_Test::print_mt()
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
string March_Test::get_expect_state(const string &cell, const string &state)
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
std::size_t March_Test::GetIndexOfMe() 
{

	for (size_t i = 0; i <= (*me).size();++i)
	{
		if ((*me)[i].is_access) return i;
	}
	return 0;
}
//select the fault has contain the operation=s
inline bool March_Test::select_fault_process(vec_fault_pair &vf,const string &s,fault_pair &f) 
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
inline bool March_Test::map_fault(vec_fault_pair &vf,fault_pair &f)
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
void March_Test::updata_memory_state(string op)
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
void March_Test::del_accessed_fault(fault_pair f,vec_fault_pair &vfp)
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
