#ifndef _ASSEMLER_H_
#define _ASSEMLER_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class assemble
{
private:
	map<string, string> opmap, funcmap, reg;
	map<string, char> type;
	vector<string> scode, err, bicode;
	vector<int> line;
	map<string, unsigned int> paddr, daddr;
    map<unsigned int, string>label;
	vector<unsigned int> memory;
	unsigned int registers[32];

public:
    void Error(int line, string errinfo);   //error output
    string trans_d_s(string s, unsigned int cnt);   //transform the decimal number to binary number with sign extenion
    string trans_d_hex(unsigned int addr);  //transform the decimal number to hex number
    unsigned int trans_bin_dec(string s);   //tansform the binary number to decimal number
    unsigned int trans_hex_d(string s); //transform the hex number to decimal
    string trans_hex_bin(string s);     //transform the hex to binary number
    string spa_eli(string s);   //eliminate the space at front and at end
    void code_pre();
    void lab_presudo();
    void consmap(); //construct the map
    void translate();   //translate the source code to binary code
    void disasm();      //translate the binary code to source code
    void scode_read(vector<string> s);  //read source code from a vector
    void bicode_read(vector<string>s); //read binary code from a vector
    vector<string> bicode_out();    //output the binary code
    vector<string> err_out();
    unsigned int baseaddr_out();    //output the code base address
    unsigned int DataAddr_out();    //output the data base
    string bintext_out();
    void reg_clear();
    string getinstruction(unsigned int pc){return scode[pc];}
    int comptoreal(unsigned int num);       //
	unsigned int realtocomp(int num);
	unsigned int sra(unsigned int num, unsigned int shamt);
	void alu_calculate_R(string opcode, unsigned int rd_addr, unsigned int rs_addr, unsigned int rt_addr, unsigned int shamt);
	void alu_calculate_I(string opcode, unsigned int rt_addr, unsigned int rs_addr, unsigned int imme);
	void branch(string opcode, unsigned int rs_addr, unsigned int rt_addr, string la,unsigned int &i);
	void lw_sw(string opcode, unsigned int rt_addr, unsigned int rs_addr, unsigned int imme);
	unsigned int simulator(unsigned int i);
	void Datastore(vector<string>::iterator v_it, unsigned int li);
	string dataprocess(string s, unsigned int length, unsigned int li);
	unsigned int scodesize();
	unsigned int *registers_out();
	assemble();
	~assemble();
};

#endif
