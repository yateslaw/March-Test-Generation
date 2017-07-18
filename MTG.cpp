#include "March_Test.h"
/*
The project is designed to automatically generate the March algorithm to detect
the memory fault.It takes the test condition of the input fault set as input and uses a 
heuristic algorithm to generate a March test to cover the target fault set.
*/
int main()
{
	fault selected_fault;
	vector<fault> vf;
	std::map<string, vec_fault_pair > f_m;
	std::ifstream input_file_of_fault("F:/C++/dynamic_fault.txt");
	//deal with the input target fault 
	read_fault(input_file_of_fault,vf);
	fault_classify(vf,f_m);
	March_Test mt;
	vec_fault_pair tmp;
	fault_pair fp;
	// the flag  
	size_t is_sens = 0, trans_flag = 0, flag = 0;
	//loop of fill the March element with operations 
	//which take account of the test condition of the target fault
	while (f_m.size()) 
	{
		tmp = mt.get_fault_set(f_m,trans_flag);
		while (tmp.v_fault.size() || tmp.a_fault.size())
		{
			fp = mt.get_fault_tc(tmp, is_sens,trans_flag,flag);
			mt.add_op_to_mt(fp, is_sens,flag);
			mt.delete_fault(fp, tmp, trans_flag,flag);
		}
	}
	mt.print_mt();//print the generation March algorithm 
	return 0;
}
