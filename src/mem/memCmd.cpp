/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest; // defined in memTest.cpp

bool initMemCmd()
{
	if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
		  cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
		  cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
		  cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)))
	{
		cerr << "Registering \"mem\" commands fails... exiting" << endl;
		return false;
	}
	return true;
}

//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string &option)
{
	// check option
	string token;
	if (!CmdExec::lexSingleOption(option, token))
		return CMD_EXEC_ERROR;
	if (token.size())
	{
		int b;
		if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj))))
		{
			cerr << "Illegal block size (" << token << ")!!" << endl;
			return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
		}
#ifdef MEM_MGR_H
		mtest.reset(toSizeT(b));
#else
		mtest.reset();
#endif // MEM_MGR_H
	}
	else
		mtest.reset();
	return CMD_EXEC_DONE;
}

void MTResetCmd::usage(ostream &os) const
{
	os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void MTResetCmd::help() const
{
	cout << setw(15) << left << "MTReset: "
		 << "(memory test) reset memory manager" << endl;
}

//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string &option)
{
	// TODO
	vector<string> tokens;
	if (!CmdExec::lexOptions(option, tokens))
		return CMD_EXEC_ERROR;
	size_t n = tokens.size();
	bool doarray = false;
	size_t AIndex = 0;
	for (size_t i = 0; i < n; i++)
	{
		if (myStrNCmp("-Array", tokens[i], 2) == 0)
		{
			AIndex = i;
			doarray = true;
		}
	}
	try
	{
		if (!doarray)
		{
			if (n == 0)
			{
				errorOption(CMD_OPT_MISSING, "");
				return CMD_EXEC_ERROR;
			}
			else if (n > 1)
			{
				errorOption(CMD_OPT_EXTRA, tokens[1]);
				return CMD_EXEC_ERROR;
			}
			else
			{
				int newobjs = 0;
				if (!myStr2Int(tokens[0], newobjs) || newobjs < 1)
				{
					errorOption(CMD_OPT_ILLEGAL, tokens[0]);
					return CMD_EXEC_ERROR;
				}
				mtest.newObjs(newobjs);
			}
		}
		else
		{
			if (AIndex > 1)
			{
				errorOption(CMD_OPT_EXTRA, tokens[1]);
				return CMD_EXEC_ERROR;
			}
			else
			{
				if(AIndex == 0){
					if(n < 3){
						errorOption(CMD_OPT_MISSING,"");
						return CMD_EXEC_ERROR;
					}
					else if(n > 3){
						errorOption(CMD_OPT_EXTRA,tokens[3]);
						return CMD_EXEC_ERROR;
					}
					else{
						int newarrs = 0;
						if(!myStr2Int(tokens[AIndex+1],newarrs) || newarrs < 1){
							errorOption(CMD_OPT_ILLEGAL,tokens[AIndex+1]);
							return CMD_EXEC_ERROR;
						}
						int newobjs = 0;
						if(!myStr2Int(tokens[2],newobjs) || newobjs < 1){
							errorOption(CMD_OPT_ILLEGAL,tokens[2]);
							return CMD_EXEC_ERROR;
						}
						mtest.newArrs(newobjs,newarrs);
					}
				}
				else{
					if(n < 3){
						errorOption(CMD_OPT_MISSING,"");
						return CMD_EXEC_ERROR;
					}
					else if(n > 3){
						errorOption(CMD_OPT_EXTRA,tokens[3]);
						return CMD_EXEC_ERROR;
					}
					else{
						int newarrs = 0;
						if(!myStr2Int(tokens[AIndex+1],newarrs) || newarrs < 1){
							errorOption(CMD_OPT_ILLEGAL,tokens[AIndex+1]);
							return CMD_EXEC_ERROR;
						}
						int newobjs = 0;
						if(!myStr2Int(tokens[0],newobjs) || newobjs < 1){
							errorOption(CMD_OPT_ILLEGAL,tokens[0]);
							return CMD_EXEC_ERROR;
						}
						mtest.newArrs(newobjs,newarrs);
					}
				}
				//mtest.newObjs(newobjs);
			}
		}
		//mtest.newObjs(1);
		//mtest.newArrs(1,3);
		//mtest.newArrs(1,1);
	}
	catch (bad_alloc)
	{
		return CMD_EXEC_ERROR;
	}
	// Use try-catch to catch the bad_alloc exception
	return CMD_EXEC_DONE;
}

void MTNewCmd::usage(ostream &os) const
{
	os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void MTNewCmd::help() const
{
	cout << setw(15) << left << "MTNew: "
		 << "(memory test) new objects" << endl;
}

//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string &option)
{
	// TODO
	mtest.deleteObj(0);
	mtest.deleteArr(0);
	return CMD_EXEC_DONE;
}

void MTDeleteCmd::usage(ostream &os) const
{
	os << "Usage: MTDelete <-Index (size_t objId) | "
	   << "-Random (size_t numRandId)> [-Array]" << endl;
}

void MTDeleteCmd::help() const
{
	cout << setw(15) << left << "MTDelete: "
		 << "(memory test) delete objects" << endl;
}

//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string &option)
{
	// check option
	if (option.size())
		return CmdExec::errorOption(CMD_OPT_EXTRA, option);
	mtest.print();

	return CMD_EXEC_DONE;
}

void MTPrintCmd::usage(ostream &os) const
{
	os << "Usage: MTPrint" << endl;
}

void MTPrintCmd::help() const
{
	cout << setw(15) << left << "MTPrint: "
		 << "(memory test) print memory manager info" << endl;
}
