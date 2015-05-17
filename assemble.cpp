//translate the mips source code into binary code
//author: wangkejie
//finishing time:2015/4/18

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <cmath>
#include "assemble.h"

using namespace std;

//translate a decimal string to a binary string
string assemble::trans_d_s(string s, unsigned int cnt)
{
    int sa = 0, remainder;
    string res, tmp;
    char c, sign = '0';

    string::iterator it;
    for (it = s.begin(); it != s.end(); it++)
        if (*it >= '0'&&*it <= '9')
            sa = sa * 10 + *it - '0';
    if (s[0] == '-')
    {
        sa = pow(2, cnt) - sa;
        sign = '1';
    }
    while (sa>0)
    {
        remainder = sa % 2;
        sa /= 2;
        c = remainder + '0';
        tmp.insert((int)tmp.size(), 1, c);
    }
    for (unsigned int i = 0; i<cnt - tmp.size(); i++)	//sign extend
        res.insert((int)res.size(), 1, sign);

    for (int i = tmp.size() - 1; i >= 0; i--)	//reverse the string
        res.insert((int)res.size(), 1, tmp[i]);

    return res;
}

string assemble::trans_d_hex(unsigned int addr)
{
    string res, tmp;
    unsigned int remainder;
    char c;

    while (addr>0)
    {
        remainder = addr % 16;
        addr /= 16;
        if (remainder<10)
            c = remainder + '0';
        else
            c = remainder - 10 + 'a';
        tmp.insert((int)tmp.size(), 1, c);
    }

    for (unsigned int i = 0; i<8 - tmp.size(); i++)	//unsign extend
        res.insert((int)res.size(), 1, '0');

    for (int i = tmp.size() - 1; i >= 0; i--)	//reverse the string
        res.insert((int)res.size(), 1, tmp[i]);

    return res;
}

unsigned int assemble::trans_hex_d(string s)
{
    unsigned int res = 0, bit = 0;

    for (unsigned int i = 0; i < s.size(); i++)
    {
        if (s[i] >= '0'&&s[i] <= '9')
            bit = s[i] - '0';
        else if (s[i] >= 'a'&&s[i] <= 'f')
            bit = s[i] - 'a' + 10;
        else if (s[i] >= 'A'&&s[i] <= 'F')
            bit = s[i] - 'A' + 10;

        res = res * 16 + bit;
    }

    return res;
}

unsigned int assemble::trans_bin_dec(string s)
{
    unsigned int res = 0;

    for (unsigned int i = 0; i != s.size(); i++)
        res = 2 * res + s[i] - '0';

    return res;
}

string assemble::trans_hex_bin(string s)
{
    string bin;
    for (unsigned int i = 0; i<s.size(); i++)
    {
        stringstream ss;
        ss << trans_hex_d(s.substr(i, 1));
        bin += trans_d_s(ss.str(), 4);
    }
    return bin;
}

//eliminate the space at the front and end
string assemble::spa_eli(string s)
{
    string::iterator it;

    it = s.begin();
    while ((it != s.end()) && (*it == ' ' || *it == '\t'))
        s.erase(it);
    for (it = s.end() - 1; (it != s.end()) && (*it == ' ' || *it == '\t'); it--)
        s.erase(it);

    return s;
}

void assemble::scode_read(vector<string> s)	//read the source code
{
    scode = s;
}

void assemble::bicode_read(vector<string> s)
{
    bicode = s;
}

unsigned int assemble::baseaddr_out()
{
    return paddr["baseAddr"];
}

unsigned int assemble::DataAddr_out()
{
    return daddr["DataAddr"];
}

void assemble::code_pre()	// space and comment
{
    vector<string>::iterator v_it;

    for (v_it = scode.begin(); v_it != scode.end(); v_it++)
    {
        string::iterator s_it;
        s_it = (*v_it).begin();

        if (*v_it == "")	//newline
            continue;
        if ((*v_it).size() >= 2)
        {
            //erase the comments
            for (s_it = (*v_it).begin() + 1; s_it != (*v_it).end(); s_it++)
                if (*s_it == '/' && *(s_it - 1) == '/')
                {
                (*v_it).erase(s_it - 1, (*v_it).end());
                break;
                }
        }

        //erase the space at fornt
        s_it = (*v_it).begin();
        while (*s_it == ' ' || *s_it == '\t')
            s_it = (*v_it).erase(s_it);

        if (*s_it == '\n')	//newline
            continue;

        //erase the extra character in the end
        for (s_it = (*v_it).end() - 1; *s_it == ' ' || *s_it == '\t' || *s_it == '\n'; s_it--)
            (*v_it).erase(s_it);
    }
}

void assemble::lab_presudo()	//label and presudo-insruction
{
    unsigned int li = 1, pc, dc;

    vector<string>::iterator v_it;
    v_it = scode.begin();
    while (v_it != scode.end())
    {
        string opcode, baseAddr, label;

        string::iterator s_it;
        s_it = (*v_it).begin();

        //newline
        if (*v_it == "")
        {
            v_it = scode.erase(v_it);
            li++;
            continue;
        }

        //baseAddr
        if (*s_it == '#')
        {
            s_it++;
			while (s_it != (*v_it).end() && *s_it != ' ' && *s_it != '\t')
                opcode += *s_it++;
            if (opcode != "baseAddr" && opcode != "DataAddre")//wrong opcode
            {
                Error(li, "unknown operator");
                li++;
                v_it = scode.erase(v_it);
                continue;
            }
            else
            {
                //jump the space
                while (*s_it != ' '&&*s_it != '\t'&&s_it != (*v_it).end())
                    s_it++;
                //get the base address
                while (s_it != (*v_it).end())
                {
                    if ((*s_it <= '9'&&*s_it >= '0') || (*s_it <= 'f'&&*s_it >= 'a') || (*s_it <= 'A'&&*s_it >= 'F'))
                        baseAddr += *s_it;
                    s_it++;
                }
            }
            if (opcode == "baseAddr")//code segement
            {
                pc = trans_hex_d(baseAddr);
                paddr["baseAddr"] = pc;
                v_it = scode.erase(v_it);
                li++;
                continue;
            }
            else    //data segement
            {
                dc = trans_hex_d(baseAddr);
                daddr["DataAddr"] = dc;
                //v_it = scode.erase(v_it);
				//if (dc < pc)
                    //err.push_back("memory overflow or data address is too small!");
                //else
                //{
                    Datastore(v_it, li);
                    return;
                //}
            }
        }

        //label
        while (s_it != (*v_it).end() && *s_it != ':')
        {
            label += *s_it;
            s_it++;
        }
        if (s_it != (*v_it).end() && *s_it == ':')
        {
            paddr[spa_eli(label)] = pc;
            s_it++;
            while (s_it != (*v_it).end() && (*s_it == ' ' || *s_it == '\t'))
                s_it++;
            (*v_it).erase((*v_it).begin(), s_it); // erase the label and space
            continue;
        }
        else
            s_it = (*v_it).begin();


        //instruction
        //semicolon check
        if (*((*v_it).end() - 1) != ';')
        {
            Error(li, "syntax error");
            v_it++;
            li++;
            continue;
        }

        line.push_back(li);
        pc += 4;
        v_it++;
        li++;
    }
}

void assemble::translate()	//translate the instruction into binary code
{
    string baseAddr;
    vector<int> line;
    unsigned l = 0, pc;

	bicode.clear();
	memory.clear();
	err.clear();
    code_pre();
    lab_presudo();	//label and presudo-instructions   
    pc = paddr["baseAddr"];
    if (err.size() != 0)
        return;
    vector<string>::iterator v_it;
    //for every instruction
    for (v_it = scode.begin(); v_it != scode.end(); v_it++)
    {
        string opcode, rs, rt, rd, shamt, imme, target, label, s_tmp;
        vector<string> key;

        string::iterator s_it;
        s_it = (*v_it).begin();
        pc += 4;
        while (*s_it != ' ' && *s_it != '\t' && s_it < (*v_it).end()) //opcode
            s_tmp += *s_it++;
        key.push_back(spa_eli(s_tmp));
        s_tmp.erase(s_tmp.begin(), s_tmp.end());

        while (1)
        {
            while (*s_it != ',' && *s_it != ';')
                s_tmp += *s_it++;
            if (s_tmp.size())
                key.push_back(spa_eli(s_tmp));
            s_tmp.erase(s_tmp.begin(), s_tmp.end());
            if (*s_it++ == ';')
                break;
        }

        opcode = key[0];
        //operator fault
        if (type.find(opcode) == type.end())
        {
            Error(line[l++], ": unknown operator");
            continue;
        }

        if (opmap.find(opcode) != opmap.end() && ((opmap[opcode].substr(0, 3) == "100" || opmap[opcode].substr(0, 3) == "101"))) //load and store
        {
            if (key[2].size() == 0)
            {
                Error(line[l++], "syntax error");
                continue;
            }

            string s_tmp, key3;
            string::iterator it_tmp;
            it_tmp = key[2].begin();

            while (*it_tmp != '(' && it_tmp != key[2].end())
                if (*it_tmp >= '0' && *it_tmp <= '9')
                    s_tmp += *it_tmp++;
            if (it_tmp == key[2].end())
            {
                Error(line[l++], "syntax error");
                continue;
            }
            else
                key3 = spa_eli(s_tmp);
            it_tmp++;
            s_tmp.erase(s_tmp.begin(), s_tmp.end());
            while (*it_tmp != ')' && it_tmp != key[2].end())
                s_tmp += *it_tmp++;
            if (*it_tmp != ')')
            {
                Error(line[l++], "syntax error");
                continue;
            }
            key[2] = s_tmp;
            key.push_back(key3);
        }

        if (type[opcode] == 'R') //R-format
        {
            if (opcode == "jr") //jr
            {
                if (key.size() != 2)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
                else
                {
                    rs = key[1];
                    rt = "$zero";
                    rd = "$zero";
                    shamt = "00000";
                }
            }
            else if (opcode == "jalr")	//jalr
            {
                if (key.size() != 3)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
                else
                {
                    rs = key[1];
                    rt = "$zero";
                    rd = key[2];
                    shamt = "00000";
                }
            }
            else if (opcode == "sll" || opcode == "srl" || opcode == "sra")  //Shift instruction
            {

                if (key.size() != 4)
                {
                    Error(line[l++], "syntax error");	//
                    continue;
                }
                else
                {
                    rd = key[1];
                    rt = key[2];
                    if (trans_d_s(key[3], 5).size() > 5)
                    {
                        Error(line[l++], ": shift amount is too large");
                        continue;
                    }
                    shamt = trans_d_s(key[3], 5);
                    rs = "$zero";
                }
            }
            else
            {
                if (key.size() != 4)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
                else
                {
                    rd = key[1];
                    rs = key[2];
                    rt = key[3];
                    shamt = "00000";
                }
            }
            if (reg.find(rs) == reg.end() || reg.find(rt) == reg.end() || reg.find(rd) == reg.end())
            {
                Error(line[l++], ": unkonwn register");
                continue;
            }
            bicode.push_back("000000" + reg[rs] + reg[rt] + reg[rd] + shamt + funcmap[opcode]);
        }
        else if (type[opcode] == 'I')   //I-format
        {
			if (opmap[opcode] == "001111") //lui
            {
                if (key.size() != 3)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
                rs = key[1];
                rt = "$zero";
                if (trans_d_s(key[2], 16).size() > 16)
                {
                    Error(line[l++], ": the immediate is too large");
                    continue;
                }
                imme = trans_d_s(key[2], 16);
            }
            else if (opcode == "blez" || opcode == "bgtz")	//blez and bgtz instructions
            {
                if (key.size() != 3)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
                rs = key[1];
                rt = "$zero";

                stringstream s;
                int t = ((paddr[key[2]] - pc)/4);
                s << t;
                key[2] = s.str();
                if (trans_d_s(key[2], 16).size() > 16)
                {
                    Error(line[l++], ": the immediate is too large");
                    continue;
                }
                imme = trans_d_s(key[2], 16);
            }
            else  //ALU-I format instructions
            {
                if (key.size() != 4)
                {
                    Error(line[l++], "syntax error");
                    continue;
                }
				if (key[0] == "lw" || key[0] == "sw")
				{
					rs = key[2];
					rt = key[1];
				}
				else
				{
					rt = key[2];
					rs = key[1];
				}
                if (opmap[opcode].substr(0, 3) == "000")	//branch instruction
                {
                    stringstream s;
                    int t = ((paddr[key[3]] - pc)/4);
                    s << t;
                    key[3] = s.str();
                }
                if (trans_d_s(key[3], 16).size() > 16)
                {
                    Error(line[l++], ": the immediate is too large");
                    continue;
                }
                imme = trans_d_s(key[3], 16);
            }
            shamt = "00000";

            if (reg.find(rs) == reg.end() || reg.find(rt) == reg.end())
            {
                Error(line[l++], ": unkonwn register");
                continue;
            }

            bicode.push_back(opmap[opcode] + reg[rs] + reg[rt] + imme);
        }
        else if (type[opcode] == 'J')    //J-format
        {
            if (key.size() != 2)
            {
                Error(line[l++], "syntax error");
                continue;
            }
            stringstream s;
            int t = paddr[key[1]]/4;
            s << t;

            if (trans_d_s(s.str(), 26).size() > 26)
            {
                Error(line[l++], "syntax error");
                continue;
            }
            else
                imme = trans_d_s(s.str(), 26);
            bicode.push_back(opmap[opcode] + imme);
        }
        else
            Error(line[l++], ": unknown operator");

        l++;
    }
}

void assemble::disasm()
{
    scode.clear();//clear out the instruction in the source code

    map<string, string> disopmap, disfuncmap, disregmap;

    map<string, string> ::iterator it;
    for (it = opmap.begin(); it != opmap.end(); it++)
        disopmap[it->second] = it->first;
    for (it = funcmap.begin(); it != funcmap.end(); it++)
        disfuncmap[it->second] = it->first;
    for (it = reg.begin(); it != reg.end(); it++)
        if ((it->first).substr(0, 1) == "$")
            disregmap[it->second] = it->first;

    for (unsigned int i = 0; i<bicode.size(); i++)
    {
        string instruction = "";
        string binopcode, binrs, binrt, binrd, binshamt, binfunc, binimme, binjumpimme;
        string opcode, rs, rt, rd, shamt, func, imme;
        int imme_int;

        binopcode = bicode[i].substr(0, 6);
        binrs = bicode[i].substr(6, 5);
        binrt = bicode[i].substr(11, 5);
        binrd = bicode[i].substr(16, 5);
        binshamt = bicode[i].substr(21, 5);
        binfunc = bicode[i].substr(26, 6);
        binimme = bicode[i].substr(16, 16);
        binjumpimme = bicode[i].substr(6, 26);

        if (disopmap.find(binopcode) != disopmap.end())
            opcode = disopmap[binopcode];

        if (binopcode == "000000")   //R-format
        {
            opcode = disfuncmap[binfunc];
            if (opcode == "jr" || opcode == "jalr")  //jr and jalr
                instruction = opcode + "  " + disregmap[binrs];
            else if (opcode == "sll" || opcode == "srl" || opcode == "sra")	//sll, srl, sra
            {
                rs = disregmap[binrs];
                rt = disregmap[binrt];
                shamt = trans_bin_dec(binshamt);
                instruction = opcode + "  " + rt + ", " + rs + ", " + shamt + ";";
            }
            else    //ALU-R
            {
                rs = disregmap[binrs];
                rt = disregmap[binrt];
                rd = disregmap[binrd];
                instruction = opcode + "  " + rd + ", " + rs + ", " + rt + ";";
            }
        }
        else if (type[opcode] == 'I')
        {
            stringstream ss;
            if (opcode[opcode.size() - 1] == 'u' || opcode == "lui")	//unsigned opcode
                imme_int = trans_bin_dec(binimme);
            else	//signed opcode
            {
                imme_int = trans_bin_dec(binimme);
                if (imme_int >= pow(2, 15))
                    imme_int -= pow(2, 16);
            }
            if (binopcode.substr(0, 3) == "100" || binopcode.substr(0, 3) == "101")  //load and store
            {
                ss << imme_int;
                imme = ss.str();
                rs = disregmap[binrs];
                rt = disregmap[binrt];
                instruction = opcode + "  " + rt + "," + imme + "(" + rs + ")" + ";";
            }
            else if (binopcode == "001111") //lui
            {
                ss << imme_int;
                imme = ss.str();
                rt = disregmap[binrt];
                instruction = opcode + "  " + ", " + imme + ";";
            }
            else if (binopcode.substr(0, 3) == "001")   //ALU-I
            {
                ss << imme_int;
                imme = ss.str();
                rs = disregmap[binrs];
                rt = disregmap[binrt];
                instruction = opcode + "  " + rt + ", " + rs + ", " + imme + ";";
            }
            else if (binopcode.substr(0, 3) == "000")    //branch instruction
            {
                imme_int = imme_int + i + 1;
                string la;
                stringstream sss;
                sss << (label.size() + 1);
                la = "loop" + sss.str();
                if (label.find(imme_int) == label.end())
                    label[imme_int] = la;
				else
					la = label[trans_bin_dec(binjumpimme)];
                rs = disregmap[binrs];
                rt = disregmap[binrt];
                instruction = opcode + "  " + rs + ", " + rt + ", " + la + ";";
            }
        }
        else if (type[opcode] == 'J')
        {
            string la;
            stringstream sss;
            sss << (label.size() + 1);
            la = "loop" + sss.str();
			if (label.find(trans_bin_dec(binjumpimme)) == label.end())
				label[trans_bin_dec(binjumpimme)] = la;
			else
				la = label[trans_bin_dec(binjumpimme)];

            instruction = opcode + "  " + la + ";";
        }

        scode.push_back(instruction);
    }


}

string assemble::bintext_out()
{
    string bin;
    //unsigned int cnt = 1;
    bin += "#baseAddr 0000\n";
    for (unsigned int i = 0; i<scode.size(); i++)
    {
        if (label.find(i) != label.end())
            bin += (label[i] + ":\n");
        if (label.size() != 0)
        bin += "    ";
        bin += (scode[i] + "\n");
    }

    return bin;
}

void assemble::Error(int line, string errinfo)
{
    stringstream s;
    s << line;
    err.push_back("line" + s.str() + ':' + errinfo);

}

int assemble::comptoreal(unsigned int num)
{
    if (num >= pow(2, 31))
        return (int)(num - pow(2, 32));
    else
        return (int)num;
}

unsigned int assemble::realtocomp(int num)
{
    if (num<0)
        num += 0x100000000;

    return num;
}

unsigned int assemble::sra(unsigned int num, unsigned int shamt)
{
    if (num<pow(2, 32))
        num = num >> shamt;
    else
        for (unsigned int i = 0; i<shamt; i++)
            num = num / 2 + pow(2, 31);

    return num;
}

void assemble::alu_calculate_R(string opcode, unsigned int rd_addr, unsigned int rs_addr, unsigned int rt_addr, unsigned int shamt)
{
    int s_rs, s_rt;
    unsigned int uns_rs, uns_rt;

    s_rs = comptoreal(registers[rs_addr]);
    s_rt = comptoreal(registers[rt_addr]);
    uns_rs = registers[rs_addr];
    uns_rt = registers[rt_addr];

    if (opcode == "add")
        registers[rd_addr] = realtocomp(s_rs + s_rt);
    else if (opcode == "addu")
        registers[rd_addr] = uns_rs + uns_rt;
    else if (opcode == "sub")
        registers[rd_addr] = realtocomp(s_rs - s_rt);
    else if (opcode == "subu")
        registers[rd_addr] = realtocomp(uns_rs - uns_rt);
    else if (opcode == "and")
        registers[rd_addr] = uns_rs & uns_rt;
    else if (opcode == "or")
        registers[rd_addr] = uns_rs | uns_rt;
    else if (opcode == "xor")
        registers[rd_addr] = uns_rs ^ uns_rt;
    else if (opcode == "nor")
        registers[rd_addr] = ~(uns_rs | uns_rt);
    else if (opcode == "sll")
        registers[rd_addr] = uns_rt << shamt;
    else if (opcode == "srl")
        registers[rd_addr] = uns_rt >> shamt;
    else if (opcode == "sra")
        registers[rd_addr] = sra(uns_rt, shamt);
    else if (opcode == "sllv")
        registers[rd_addr] = uns_rt << uns_rs;
    else if (opcode == "srlv")
        registers[rd_addr] = uns_rt >> uns_rs;
    else if (opcode == "srav")
        registers[rd_addr] = sra(uns_rt, uns_rs);
    else if (opcode == "slt")
        registers[rd_addr] = (s_rs<s_rt) ? 1 : 0;
    else if (opcode == "sltu")
        registers[rd_addr] = (uns_rs<uns_rt) ? 1 : 0;
    else if (opcode == "mul")
        registers[rd_addr] = realtocomp(s_rs*s_rt);
    else if (opcode == "mulu")
        registers[rd_addr] = uns_rs*uns_rt;
    else if (opcode == "div")
        registers[rd_addr] = realtocomp(s_rs / s_rt);
    else if (opcode == "divu")
        registers[rd_addr] = uns_rs / uns_rt;

}

void assemble::alu_calculate_I(string opcode, unsigned int rt_addr, unsigned int rs_addr, unsigned int imme)
{
    int s_rs, s_imme;
    unsigned int uns_rs, uns_imme;

    s_rs = comptoreal(registers[rs_addr]);
    uns_rs = registers[rs_addr];
    s_imme = comptoreal(imme);
    uns_imme = imme;

    if (opcode == "addi")
        registers[rt_addr] = realtocomp(s_rs + s_imme);
    else if (opcode == "addiu")
        registers[rt_addr] = uns_rs + uns_imme;
    else if (opcode == "andi")
        registers[rt_addr] = uns_rs & uns_imme;
    else if (opcode == "ori")
        registers[rt_addr] = uns_rs | uns_imme;
    else if (opcode == "xori")
        registers[rt_addr] = uns_rs ^ uns_imme;
    else if (opcode == "slti")
        registers[rt_addr] = s_rs<s_imme ? 1 : 0;
    else if (opcode == "sltiu")
        registers[rt_addr] = uns_rs<uns_imme ? 1 : 0;
}

void assemble::branch(string opcode, unsigned int rs_addr, unsigned int rt_addr, string la, unsigned int &i)
{
    unsigned int base;

    base = baseaddr_out();
    if (opcode == "beq")
    {
		if (registers[rs_addr] == registers[rt_addr])
			i = (paddr[la] - base) / 4;
    }
    else if (opcode == "bne")
    {
		if (registers[rs_addr] != registers[rt_addr])
			i = (paddr[la] - base) / 4;
    }
    else if (opcode == "blez")
    {
		if (registers[rs_addr] <= 0)
			i = (paddr[la] - base) / 4;
    }
    else if (opcode == "bgtz")
    {
		if (registers[rs_addr] > 0)
			i = (paddr[la] - base) / 4;
    }
    /*else if(opcode == "bltz")
    {
    if(registers[rs_addr] <0)
    i = (paddr[la] - base)/4;
    }
    else if(opcode == "bgez")
    {
    if(registers[rs_addr] >=0)
    i = (paddr[la] - base)/4;
    }*/
}

void assemble::lw_sw(string opcode, unsigned int rs_addr, unsigned int rt_addr, unsigned int imme)
{
	unsigned int dc = DataAddr_out();
	unsigned int uns_rt,uns_rs, uns_imme, address;
	int s_rs, s_imme;

	s_rs = comptoreal(registers[rs_addr]);
	uns_rs = registers[rs_addr];
	uns_rt = registers[rt_addr];
	s_imme = comptoreal(imme);
	uns_imme = imme;
	address = s_imme + uns_rs - dc;
	unsigned int data;
	string da;
	
	if (opcode == "lw")
	{
		data = memory[address] * pow(16, 6) + memory[address + 1] * pow(16, 4) + memory[address + 2] * 16 * 16 + memory[address + 3];
		registers[rt_addr] = data;
	}
	else if (opcode == "sw")
	{
		data = registers[rt_addr];
		da = trans_d_hex(data);
		//stringstream ss;
		//ss << data;
		memory[address] = trans_hex_d(da.substr(0, 2));
		memory[address + 1] = trans_hex_d(da.substr(2, 2));
		memory[address + 2] = trans_hex_d(da.substr(4, 2));
		memory[address + 3] = trans_hex_d(da.substr(6, 2));
	}
	//else if (opcode == "lb")
}

unsigned int assemble::simulator(unsigned int i)
{
    unsigned int base;
    string instruction;

    base = paddr["baseAddr"];

    unsigned int rs_addr = 0, rt_addr = 0, rd_addr = 0, shamt = 0, rs, rt, imme;
    string opcode, s_tmp;
    vector<string> key;

    instruction = scode[i];	//fetch the instrucion
    i++;	//next instruction

    string::iterator s_it;

    s_it = instruction.begin();
    while (*s_it != ' ' && *s_it != '\t' && s_it < instruction.end()) //opcode
        s_tmp += *s_it++;
    key.push_back(spa_eli(s_tmp));
    s_tmp.erase(s_tmp.begin(), s_tmp.end());

    while (1)
    {
        while (*s_it != ',' && *s_it != ';')
            s_tmp += *s_it++;
        if (s_tmp.size())
            key.push_back(spa_eli(s_tmp));
        s_tmp.erase(s_tmp.begin(), s_tmp.end());
        if (*s_it++ == ';')
            break;
    }

    opcode = key[0];

    if (opmap.find(opcode) != opmap.end() && ((opmap[opcode].substr(0, 3) == "100" || opmap[opcode].substr(0, 3) == "101"))) //load and store
    {
        string s_tmp, key3;
        string::iterator it_tmp;
        it_tmp = key[2].begin();

        while (*it_tmp != '(' && it_tmp != key[2].end())
            if (*it_tmp >= '0' && *it_tmp <= '9')
                s_tmp += *it_tmp++;

        key3 = spa_eli(s_tmp);
        it_tmp++;
        s_tmp.erase(s_tmp.begin(), s_tmp.end());
        while (*it_tmp != ')' && it_tmp != key[2].end())
            s_tmp += *it_tmp++;

        key[2] = s_tmp;
        key.push_back(key3);
        s_tmp.erase(s_tmp.begin(), s_tmp.end());
    }

    if (type[opcode] == 'R')	//R-format
    {
        if (opcode == "jr")	//jr
        {
            rs_addr = trans_bin_dec(reg[key[1]]);
            rs = registers[rs_addr];
            i = (rs - base) / 4;
        }
        else if (opcode == "jalr")	//jalr
        {
            rs_addr = trans_bin_dec(reg[key[1]]);
            rd_addr = trans_bin_dec(reg[key[2]]);
            rs = registers[rs_addr];
            registers[rd_addr] = i * 4 + base;
            i = (rs - base) / 4;
        }
        else
        {
            if (opcode == "sll" || opcode == "srl" || opcode == "sra")	//sll, srl, sra
            {
                rd_addr = trans_bin_dec(reg[key[1]]);
                rt_addr = trans_bin_dec(reg[key[2]]);
                shamt = trans_bin_dec(trans_d_s(key[3], 5));
            }
            else
            {
                rd_addr = trans_bin_dec(reg[key[1]]);
                rs_addr = trans_bin_dec(reg[key[2]]);
                rt_addr = trans_bin_dec(reg[key[3]]);
            }
            alu_calculate_R(opcode, rd_addr, rs_addr, rt_addr, shamt);
        }
    }
    else if (type[opcode] == 'I')	//I-format
    {
        if (opmap[opcode] == "001111")	//lui
        {
            rt_addr = trans_bin_dec(reg[key[1]]);
            rt = registers[rt_addr];
            imme = trans_bin_dec(trans_d_s(key[2], 16)) << 16;
            registers[rt_addr] = imme;
        }
        else if (opmap[opcode].substr(0, 3) == "001")	//ALU-I instructions
        {
            rt_addr = trans_bin_dec(reg[key[1]]);
            rs_addr = trans_bin_dec(reg[key[2]]);
            imme = trans_bin_dec(trans_d_s(key[2], 16));
            alu_calculate_I(opcode, rt_addr, rs_addr, imme);
        }
        else if (opmap[opcode].substr(0, 3) == "000")	//branch instuctions
        {
            rs_addr = trans_bin_dec(reg[key[1]]);
            rt_addr = (key.size() == 4) ? (trans_bin_dec(reg[key[2]])) : 0;	//take the bgtz into consideration
            branch(opcode, rs_addr, rt_addr, key[3], i);
        }
        else //load and	store instructions
        {
			rt_addr = trans_bin_dec(reg[key[1]]);
			rs_addr = trans_bin_dec(reg[key[2]]);
			imme = trans_bin_dec(trans_d_s(key[3], 16));
			lw_sw(opcode, rs_addr, rt_addr, imme);
        }
    }
    else if (type[opcode] == 'J')	//J-format
    {
        if (opcode == "j")
            i = (paddr[key[1]] - base)/4;
        else
        {
            registers[31] = i * 4 + base;
            i = (paddr[key[1]] - base)/4;
        }
    }

    return i;
}

void assemble::consmap()
{
    //register
    reg["R0"] = "00000";	reg["r0"] = "00000";	reg["$zero"] = "00000"; //0
    reg["R1"] = "00001";	reg["r1"] = "00001";	reg["$at"] = "00001"; //1
    reg["R2"] = "00010";	reg["r2"] = "00010";	reg["$v0"] = "00010"; //2
    reg["R3"] = "00011";	reg["r3"] = "00011";	reg["$v1"] = "00011"; //3
    reg["R4"] = "00100";	reg["r4"] = "00100";	reg["$a0"] = "00100"; //4
    reg["R5"] = "00101";	reg["r5"] = "00101";	reg["$a1"] = "00101"; //5
    reg["R6"] = "00110";	reg["r6"] = "00110";	reg["$a2"] = "00110"; //6
    reg["R7"] = "00111";	reg["r7"] = "00111";	reg["$a3"] = "00111"; //7
    reg["R8"] = "01000";	reg["r8"] = "01000";	reg["$t0"] = "01000"; //8
    reg["R9"] = "01001";	reg["r9"] = "01001";	reg["$t1"] = "01001"; //9
    reg["R10"] = "01010";	reg["r10"] = "01010";	reg["$t2"] = "01010"; //10
    reg["R11"] = "01011";	reg["r11"] = "01011";	reg["$t3"] = "01011"; //11
    reg["R12"] = "01100";	reg["r12"] = "01100";	reg["$t4"] = "01100"; //12
    reg["R13"] = "01101";	reg["r13"] = "01101";	reg["$t5"] = "01101"; //13
    reg["R14"] = "01110";	reg["r14"] = "01110";	reg["$t6"] = "01110"; //14
    reg["R15"] = "01111";	reg["r15"] = "01111";	reg["$t7"] = "01111"; //15
    reg["R16"] = "10000";	reg["r16"] = "10000";	reg["$s0"] = "10000"; //16
    reg["R17"] = "10001";	reg["r17"] = "10001";	reg["$s1"] = "10001"; //17
    reg["R18"] = "10010";	reg["r18"] = "10010";	reg["$s2"] = "10010"; //18
    reg["R19"] = "10011";	reg["r19"] = "10011";	reg["$s3"] = "10011"; //19
    reg["R20"] = "10100";	reg["r20"] = "10100";	reg["$s4"] = "10100"; //20
    reg["R21"] = "10101";	reg["r21"] = "10101";	reg["$s5"] = "10101"; //21
    reg["R22"] = "10110";	reg["r22"] = "10110";	reg["$s6"] = "10110"; //22
    reg["R23"] = "10111";	reg["r23"] = "10111";	reg["$s7"] = "10111"; //23
    reg["R24"] = "11000";	reg["r24"] = "11000";	reg["$t8"] = "11000"; //24
    reg["R25"] = "11001";	reg["r25"] = "11001";	reg["$t9"] = "11001"; //25
    reg["R26"] = "11010";	reg["r26"] = "11010";	reg["$k0"] = "11010"; //26
    reg["R27"] = "11011";	reg["r27"] = "11011";	reg["$k1"] = "11011"; //27
    reg["R28"] = "11100";	reg["r28"] = "11100";	reg["$gp"] = "11100"; //28
    reg["R29"] = "11101";	reg["r29"] = "11101";	reg["$sp"] = "11101"; //29
    reg["R30"] = "11110";	reg["r30"] = "11110";	reg["$fp"] = "11110"; //30
    reg["R31"] = "11111";	reg["r31"] = "11111";	reg["$ra"] = "11111"; //31

    //R-format
    type["add"] = 'R';	 funcmap["add"] = "100000";  //ALU-R
    type["addu"] = 'R';	 funcmap["addu"] = "100001";
    type["sub"] = 'R';	 funcmap["sub"] = "100010";
    type["subu"] = 'R';	 funcmap["subu"] = "100011";
    type["and"] = 'R';	 funcmap["and"] = "100100";
    type["or"] = 'R';	 funcmap["or"] = "100101";
    type["xor"] = 'R';	 funcmap["xor"] = "100110";
    type["nor"] = 'R';	 funcmap["nor"] = "100111";

    type["sll"] = 'R';	 funcmap["sll"] = "000000";
    type["srl"] = 'R';	 funcmap["srl"] = "000010";
    type["sra"] = 'R';	 funcmap["sra"] = "000011";
    type["sllv"] = 'R';	 funcmap["sllv"] = "000100";
    type["srlv"] = 'R';	 funcmap["srlv"] = "000110";
    type["srav"] = 'R';	 funcmap["srav"] = "000111";
    type["slt"] = 'R';	 funcmap["slt"] = "101010";
    type["sltu"] = 'R';	 funcmap["sltu"] = "101011";

    type["mul"] = 'R';  funcmap["mul"] = "011000"; //mul and div
    type["multu"] = 'R';  funcmap["multu"] = "011001";
    type["div"] = 'R';  funcmap["div"] = "011010";
    type["divu"] = 'R';  funcmap["divu"] = "011011";

    type["jr"] = 'R';  funcmap["jr"] = "001000"; //jump R-format
    type["jalr"] = 'R';  funcmap["jalr"] = "001001";

    //I-format
    type["lw"] = 'I';	 opmap["lw"] = "100011"; //load instructons
    type["lb"] = 'I';	 opmap["lb"] = "100000";
    type["lbu"] = 'I';  opmap["lbu"] = "100100";
    type["lh"] = 'I';	 opmap["lh"] = "100001";
    type["lhu"] = 'I';  opmap["lhu"] = "100101";

    type["sw"] = 'I';	 opmap["sw"] = "101011"; //store instructions
    type["sb"] = 'I';	 opmap["sb"] = "101000";
    type["sh"] = 'I';	 opmap["sh"] = "101001";

    type["addi"] = 'I';	 opmap["addi"] = "001000"; //ALU-I
    type["addiu"] = 'I';  opmap["addiu"] = "001001";
    type["andi"] = 'I';	 opmap["andi"] = "001100";
    type["ori"] = 'I';  opmap["ori"] = "001101";
    type["xori"] = 'I';	 opmap["xori"] = "001110";
    type["lui"] = 'I';  opmap["lui"] = "001111";
    type["slti"] = 'I';	 opmap["slti"] = "001010";
    type["sltiu"] = 'I';  opmap["sltiu"] = "001011";

    type["beq"] = 'I';	 opmap["beq"] = "000100"; //branch instructions
    type["bne"] = 'I';  opmap["bne"] = "000101";
    type["blez"] = 'I';	 opmap["blez"] = "000110";
    type["bgtz"] = 'I';	 opmap["bgtz"] = "000111";

    //J-format
    type["j"] = 'J';   opmap["j"] = "000010";
    type["jal"] = 'J';   opmap["jal"] = "000011";
}

string assemble::dataprocess(string s, unsigned int length, unsigned int li)
{
    string s_tmp;

    if (s[0] == '\'')	//string
    {
        if (s[s.size() - 1] != '\'')
        {
            Error(li, "syntax error");
            return "err";
        }
        else
        {
            for (unsigned int i = 0; i<s.size(); i++)
            {
                if (s[i] == '\'')
                    continue;
                s_tmp += trans_d_hex(s[i]).substr(6, 8);
            }
        }
    }
    else
    {
        string::iterator it;
        it = s.begin();
        if (*it == '0'&&*(it + 1) == 'x')
            s.erase(it, it + 2);
        s_tmp = s;
    }
    while (s_tmp.size()<length)
        s_tmp += '0';

    return s_tmp;
}

void assemble::Datastore(vector<string>::iterator v_it, unsigned int li)
{
    string::iterator s_it;
    string dc, baseAddr;

    dc = DataAddr_out();

    while (v_it != scode.end())
    {
        string label, opcode, s_tmp;
        s_it = (*v_it).begin();

        //newline
        if (*v_it=="")
        {
            v_it = scode.erase(v_it);
            li++;
            continue;
        }
        if (*s_it == '#')
        {
            while (*s_it != ' '&&*s_it != '\t'&&s_it != (*v_it).end())
                s_it++;
            s_it++;
            while (s_it != (*v_it).end())
            {
                if ((*s_it <= '9'&&*s_it >= '0') || (*s_it <= 'f'&&*s_it >= 'a') || (*s_it <= 'A'&&*s_it >= 'F'))
                    baseAddr += *s_it;
                else
                {
                    Error(li, "Wrong data");
                    li++;
                    continue;
                }
                s_it++;
            }
            v_it = scode.erase(v_it);
            li++;
            continue;
        }
        while (s_it != (*v_it).end() && *s_it != ':')	//fetch the label if it exits
        {
            label += *s_it;
            s_it++;
        }

        //label
        if (s_it != (*v_it).end() && *s_it == ':')
        {
            daddr[spa_eli(label)] = trans_hex_d(baseAddr);
            baseAddr.erase(baseAddr.begin(), baseAddr.end());
            s_it++;
            while (s_it != (*v_it).end() && (*s_it == ' ' || *s_it == '\t'))
                s_it++;
            (*v_it).erase((*v_it).begin(), s_it); // erase the label and space

            if ((*v_it).size() == 0)	//a sigle line with label
                (*v_it) += '\n';

            continue;
        }
        else
            s_it = (*v_it).begin();


        //data defination

        //semicolon check
        if (*((*v_it).end() - 1) != ';')
        {
            Error(li, "syntax error");
            v_it++;
            li++;
            continue;
        }
        while (*s_it != ' ' && *s_it != '\t' && s_it < (*v_it).end()) //opcode
            s_tmp += *s_it++;
        opcode = (spa_eli(s_tmp));
        s_tmp.erase(s_tmp.begin(), s_tmp.end());
        if (s_it == (*v_it).end())
        {
            Error(li, "syntax error");
            li++;
            continue;
        }


        while (1)
        {
            unsigned int length;

            while (*s_it != ',' && *s_it != ';')	//data into the s_tmp
                s_tmp += *s_it++;
            s_tmp = spa_eli(s_tmp);
            if (opcode == "dd")
                length = 8;
            else if (opcode == "dw")
                length = 4;
            else if (opcode == "db")
                length = 2;
            else
                Error(li, "Unknown operation code");

            s_tmp = dataprocess(s_tmp, length, li);

            if (err.size() != 0)	//Error exits
                return;

            while (length>0)
            {
                if (s_tmp.size() >= 2)
                    memory.push_back(trans_hex_d(s_tmp.substr(0, 2)));
                s_tmp.erase(s_tmp.begin(), s_tmp.begin() + 2);
                length -= 2;
            }
            s_tmp.erase(s_tmp.begin(), s_tmp.end());
            if (*s_it == ';')
                break;
            s_it++;
        }

        v_it = scode.erase(v_it);
    }
}

unsigned int assemble::scodesize()
{
    return scode.size();
}

unsigned int * assemble::registers_out()
{
    return registers;
}

vector<string> assemble::bicode_out()
{
    return bicode;
}

vector<string> assemble::err_out()
{
    return err;
}

void assemble::reg_clear()
{
    for (unsigned int i = 0; i<32; i++)
        registers[i] = 0;
}

assemble::assemble()
{
    consmap();
    reg_clear();
}

assemble::~assemble()
{

}
