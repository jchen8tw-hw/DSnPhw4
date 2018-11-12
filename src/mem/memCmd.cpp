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
			break;
		}
	}
	try
	{
		if (!doarray)
		{
			if (n == 0)
			{
				return errorOption(CMD_OPT_MISSING, "");
			}
			else if (n > 1)
			{
				int tmp = 0;
				if (!myStr2Int(tokens[0], tmp))
					return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
				else
					return errorOption(CMD_OPT_EXTRA, tokens[1]);
			}
			else
			{
				int newobjs = 0;
				if (!myStr2Int(tokens[0], newobjs) || newobjs < 1)
				{
					return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
				}
				mtest.newObjs(newobjs);
			}
		}
		else
		{
			if (AIndex > 1)
			{
				int tmp = 0;
				if(!myStr2Int(tokens[0],tmp))
					return errorOption(CMD_OPT_ILLEGAL,tokens[0]);
				else
					return errorOption(CMD_OPT_EXTRA, tokens[1]);
			}
			else
			{
				if (AIndex == 0)
				{
					if (n < 3)
					{
						int tmp = 0;
						if(myStrNCmp("-Array",tokens[0],2) == 0  && myStr2Int(tokens[1],tmp) && tmp >0)
							return errorOption(CMD_OPT_MISSING,"");
						else if(!myStr2Int(tokens[1],tmp) || tmp < 1)
							return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
						else
							return errorOption(CMD_OPT_MISSING, "");
					}
					else if (n > 3)
					{
						int tmp = 0;
						if (!myStr2Int(tokens[1], tmp)|| tmp < 1)
							return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
						else if(!myStr2Int(tokens[2], tmp) || tmp < 1)
							return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
						else
							return errorOption(CMD_OPT_EXTRA, tokens[3]);
					}
					else
					{
						int newarrs = 0;
						if (!myStr2Int(tokens[AIndex + 1], newarrs) || newarrs < 1)
						{
							return errorOption(CMD_OPT_ILLEGAL, tokens[AIndex + 1]);
						}
						int newobjs = 0;
						if (!myStr2Int(tokens[2], newobjs) || newobjs < 1)
						{
							return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
						}
						mtest.newArrs(newobjs, newarrs);
					}
				}
				else
				{
					if (n < 3)
					{
						return errorOption(CMD_OPT_MISSING, "");
					}
					else if (n > 3)
					{
						int tmp = 0;
						if (!myStr2Int(tokens[0], tmp))
							return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
						else if(!myStrNCmp("-Array",tokens[1],2) == 0)
							return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
						else if(!myStr2Int(tokens[2],tmp)|| tmp < 1)
							return errorOption(CMD_OPT_ILLEGAL,tokens[2]);
						else
							return errorOption(CMD_OPT_EXTRA, tokens[3]);
					}
					else
					{
						int newobjs = 0;
						if (!myStr2Int(tokens[0], newobjs) || newobjs < 1)
						{
							return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
						}
						int newarrs = 0;
						if (!myStr2Int(tokens[AIndex + 1], newarrs) || newarrs < 1)
						{
							return errorOption(CMD_OPT_ILLEGAL, tokens[AIndex + 1]);
						}	
						mtest.newArrs(newobjs, newarrs);
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
	vector<string> tokens;
	if (!CmdExec::lexOptions(option, tokens))
		return CMD_EXEC_ERROR;
	size_t n = tokens.size();
	bool doarray = false;
	bool indexf, randomf;
	indexf = randomf = false;
	size_t AIndex = 0;
	size_t IIndex = 0;
	size_t RIndex = 0;
	for (size_t i = 0; i < n; i++)
	{
		if (myStrNCmp("-Array", tokens[i], 2) == 0 && !doarray)
		{
			AIndex = i;
			doarray = true;
		}
		else if (myStrNCmp("-Index", tokens[i], 2) == 0 && !indexf)
		{
			IIndex = i;
			indexf = true;
		}
		else if (myStrNCmp("-Random", tokens[i], 2) == 0 && !randomf)
		{
			RIndex = i;
			randomf = true;
		}
	}
	if (!doarray)
	{
		//mtd -i
		if (IIndex == 0 && indexf)
		{
			int index = 0;
			if (n == 1)
				return errorOption(CMD_OPT_MISSING, tokens[0]);
			if (!myStr2Int(tokens[IIndex + 1], index) || index < 0)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
			}
			if (randomf)
			{
				return errorOption(CMD_OPT_EXTRA, tokens[RIndex]);
			}
			if (n > 2)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
			}
			if (mtest.getObjListSize() <= index)
			{
				cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << index << "!!" << endl;
				return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
			}
			mtest.deleteObj(index);
		}
		//mtd -r
		else if (RIndex == 0 && randomf)
		{
			int randID = 0;
			if (n == 1)
				return errorOption(CMD_OPT_MISSING, tokens[0]);
			if (!myStr2Int(tokens[RIndex + 1], randID) || randID < 1)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex + 1]);
			}
			if (indexf)
			{
				return errorOption(CMD_OPT_EXTRA, tokens[IIndex]);
			}
			if (n > 2)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
			}
			if (mtest.getObjListSize() == 0)
			{
				cerr << "Size of object list is 0!!" << endl;
				return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex]);
			}
			for (size_t i = 0; i < randID; i++)
			{
				mtest.deleteObj(rnGen(mtest.getObjListSize()));
			}
		}
		//mtd *
		else
		{
			if (n == 0)
			{
				return errorOption(CMD_OPT_MISSING, "");
			}
			else
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
			}
		}
	}
	else
	{
		//do array
		//mtd -i
		if (IIndex == 0 && indexf)
		{
			int index = 0;
			if (!myStr2Int(tokens[IIndex + 1], index) || index < 0)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
			}
			if (AIndex != 2)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
			}
			if (randomf)
			{
				return errorOption(CMD_OPT_EXTRA, tokens[RIndex]);
			}
			if (n > 3)
			{
				if (AIndex == 2)
					return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
				else if (AIndex == 3)
					return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
			}
			if (mtest.getArrListSize() <= index)
			{
				cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << index << "!!" << endl;
				return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
			}
			mtest.deleteArr(index);
		}
		//mtd -r
		else if (RIndex == 0 && randomf)
		{
			int randID = 0;
			if (!myStr2Int(tokens[RIndex + 1], randID) || randID < 1)
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex + 1]);
			}
			if (indexf)
			{
				return errorOption(CMD_OPT_EXTRA, tokens[IIndex]);
			}
			if (n > 3)
			{
				if (AIndex == 2)
					return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
				else if (AIndex == 3)
					return errorOption(CMD_OPT_ILLEGAL, tokens[2]);
			}
			if (mtest.getArrListSize() == 0)
			{
				cerr << "Size of array list is 0!!" << endl;
				return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex]);
			}
			for (size_t i = 0; i < randID; i++)
			{
				mtest.deleteArr(rnGen(mtest.getArrListSize()));
			}
		}
		//mtd -a
		else if (AIndex == 0)
		{
			//mtd -a -i
			if ((randomf ^ indexf) && n == 2)
				return errorOption(CMD_OPT_MISSING, tokens[1]);
			if (n == 1)
				return errorOption(CMD_OPT_MISSING, "");
			//mtd -a -r
			if (RIndex == 1)
			{
				int randID = 0;
				if (!myStr2Int(tokens[RIndex + 1], randID) || randID < 1)
				{
					return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex + 1]);
				}
				if (indexf)
				{
					return errorOption(CMD_OPT_EXTRA, tokens[IIndex]);
				}
				if (n > 3)
				{
					return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
				}
				if (mtest.getArrListSize() == 0)
				{
					cerr << "Size of array list is 0!!" << endl;
					return errorOption(CMD_OPT_ILLEGAL, tokens[RIndex]);
				}
				for (size_t i = 0; i < randID; i++)
				{
					mtest.deleteArr(rnGen(mtest.getArrListSize()));
				}
			}
			//mtd -a -i
			else if (IIndex == 1)
			{
				int index = -1;
				if (!myStr2Int(tokens[IIndex + 1], index) || index < 0)
				{
					return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
				}
				if (randomf)
				{
					return errorOption(CMD_OPT_EXTRA, tokens[RIndex]);
				}
				if (n > 3)
				{
					return errorOption(CMD_OPT_ILLEGAL, tokens[3]);
				}
				if (mtest.getArrListSize() <= index)
				{
					cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << index << "!!" << endl;
					return errorOption(CMD_OPT_ILLEGAL, tokens[IIndex + 1]);
				}
				mtest.deleteArr(index);
			}
			//mtd -a *
			else
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[1]);
			}
		}
		//mtd *
		else
		{
			if (n == 0)
			{
				return errorOption(CMD_OPT_MISSING, "");
			}
			else
			{
				return errorOption(CMD_OPT_ILLEGAL, tokens[0]);
			}
		}
	}

	//mtest.deleteObj(0);
	//mtest.deleteArr(0);
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
