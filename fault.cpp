#include "fault.h"
std::ostream &operator<<(std::ostream& os, const fault& f)
{
	os << "I: " << f.i << " ";
	os << "S: " << f.s << " ";
	os << "D: " << f.d << " ";
	os << "F: " << f.f << " ";
	os << "G: " << f.g << " ";
	return os;
}
fault get_fault(const string &s)//deal with the input file 
{
	fault f;
	std::size_t index_of_branch = s.find(";");
	std::size_t index_of_oblique = s.find("/");
	string a = s.substr(1,index_of_branch-1);//get the sensitive condition of a cell 
	//get the sensitive condition of v cell
	string v = s.substr(index_of_branch+1, index_of_oblique - index_of_branch-1); 
	//deal with the sensitive condition of a cell
	if (a.size() == 1)
	{
		f.i=f.f=(f.g += a);
	}
	else
	{
		f.i += a.substr(0, 1);
		f.s = "a" + a.substr(1);
		f.f = (f.g += a[a.size() - 1]);
	}
	//deal with the sensitive condition of v cell
	if (v.size() == 1)
	{
		f.i += v;
		f.g += v;
		f.f += ((v == "0") ? "1" : "0");
		f.d += ((v == "0") ? "vR0" : "vR1");
	}
	else
	{
		f.i += v.substr(0, 1);
		f.s = "v" + v.substr(1);
		f.g += v[v.size() - 1];
		f.f += ((v[v.size() - 1] == '0') ? "1" : "0");
		f.d += ((v[v.size() - 1] == '0') ? "vR0" : "vR1");
	}
	return f;
}
void read_fault(std::ifstream& ifs, vector<fault> &vf)
{
	string text;
	fault temp;
	while (getline(ifs, text))
	{
		vf.push_back(get_fault(text));
	}
}
//classify the input faults with 16 groups and then map the fault pair with strings.
void fault_classify(vector<fault> &vf, std::map<string, vec_fault_pair >&f_map)
{
	vector<fault> 
	v_00_00, v_00_01, v_01_01, v_01_00,
	v_10_10, v_10_11, v_11_11, v_11_10,
	a_00_00, a_00_10, a_01_01, a_01_11, 
	a_10_10, a_10_00, a_11_11, a_11_01;
	vector<vec_fault_pair> vfp;
	for (auto it = vf.begin(); it != vf.end(); ++it)
	{
		if ((*it).s.substr(0, 1) == "v" && (*it).i == "00" && (*it).g == "00")
		{
			v_00_00.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "00" && (*it).g == "01")
		{
			v_00_01.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "01" && (*it).g == "01")
		{
			v_01_01.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "01" && (*it).g == "00")
		{
			v_01_00.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "10" && (*it).g == "10")
		{
			v_10_10.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "10" && (*it).g == "11")
		{
			v_10_11.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "11" && (*it).g == "11")
		{
			v_11_11.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "v" && (*it).i == "11" && (*it).g == "10")
		{
			v_11_10.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "00" && (*it).g == "00")
		{
			a_00_00.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "00" && (*it).g == "10")
		{
			a_00_10.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "01" && (*it).g == "01")
		{
			a_01_01.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "01" && (*it).g == "11")
		{
			a_01_11.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "10" && (*it).g == "10")
		{
			a_10_10.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "10" && (*it).g == "00")
		{
			a_10_00.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "11" && (*it).g == "11")
		{
			a_11_11.push_back(*it);
		}
		else if ((*it).s.substr(0, 1) == "a" && (*it).i == "11" && (*it).g == "01")
		{
			a_11_01.push_back(*it);
		}
	}
	vfp.push_back({ v_00_00,a_01_01});
	vfp.push_back({ v_00_01,a_01_11 });
	vfp.push_back({ v_01_01,a_11_11 });
	vfp.push_back({ v_01_00,a_11_01 });
	vfp.push_back({ v_10_10,a_00_00 });
	vfp.push_back({ v_10_11,a_00_10 });
	vfp.push_back({ v_11_11,a_10_10 });
	vfp.push_back({ v_11_10,a_10_00 });
	f_map.insert({ "v_00_00",vfp[0]});
	f_map.insert({ "v_00_01",vfp[1]});
	f_map.insert({ "v_01_01",vfp[2]});
	f_map.insert({ "v_01_00",vfp[3]});
	f_map.insert({ "v_10_10",vfp[4]});
	f_map.insert({ "v_10_11",vfp[5]});
	f_map.insert({ "v_11_11",vfp[6]});
	f_map.insert({ "v_11_10",vfp[7]});
}
bool operator==(const fault &lhs, const fault &rhs)
{
	return lhs.s == rhs.s&&lhs.d == rhs.d;
}
