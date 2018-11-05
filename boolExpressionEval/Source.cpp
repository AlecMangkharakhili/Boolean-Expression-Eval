#include <iostream>
#include <vector>
#include <string>

using namespace std;
/****************************************************************************************************/
//STRUCTS

typedef struct AST* pNODE;

struct AST
{
	string info;
	pNODE children[2];
};

struct parseRslt
{
	bool success;
	AST ast;
};

struct tokRslt
{
	bool success;
	vector<string> syms;
};

struct TPERslt
{
	bool val;
	string msg;
};

//Stack Struct
struct stackNode
{
	pNODE node;
	struct stackNode* next;
};

class Stack
{
	struct stackNode *head;
	public:
		Stack()
		{
			head = NULL;
		}
		void push(pNODE node)
		{
			struct stackNode* ptr;
			ptr = new stackNode;
			ptr->node = node;
			ptr->next = NULL;
			if (head != NULL)
			{
				ptr->next = head;
			}
			head = ptr;
		}
		void pop(void)
		{
			struct stackNode *temp;
			temp = head;
			head = head->next;
			delete temp;
		}
		bool isLastNode(void)
		{
			if (head->next != NULL)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool isEmpty(void)
		{
			if (head == NULL)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		pNODE returnAST(void)
		{
			return head->node;
		}
		string returnInfo(void)
		{
			return head->node->info;
		}
		void constructSubTreeBin(void)
		{
			pNODE opNode, node1, node2;
			opNode = head->node;
			pop();
			node2 = head->node;
			pop();
			node1 = head->node;
			pop();
			opNode->children[0] = node1;
			opNode->children[1] = node2;
			push(opNode);
		}
		void constructSubTreeUn(void)
		{
			pNODE opNode, node1, node2;
			opNode = head->node;
			pop();
			node1 = head->node;
			pop();
			opNode->children[0] = node1;
			opNode->children[1] = NULL;
			push(opNode);
		}
};

/*********************************************************************************************************/
//HELPER FUNCTIONS

bool isConst(vector<string> v, int loc)
{
	if ((v[loc] == "T" || v[loc] == "F") && ((loc == v.size() - 1) || (v[loc + 1] != "T" || v[loc + 1] != "F")))
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool isUEx(vector<string> v, int loc)
{
	if (isConst(v, loc) == true)
	{
		return true;
	}
	else if ((v[loc - 1] != "T" || v[loc - 1] != "F") && v[loc] == "(" && (v[loc + 1] == "T" || v[loc + 1] == "F" || v[loc + 1] == "~" || v[loc + 1] == "("))
	{
		return true;
	}
	else if ((v[loc - 1] == "T" || v[loc - 1] == "F" || v[loc - 1] == ")") && v[loc] == ")" && (v[loc + 1] != "T" || v[loc + 1] != "F"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isNeg(vector<string> v, int loc)
{
	if (isUEx(v, loc) == true || (v[loc] == "~" && isNeg(v, loc + 1) == true))
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool isConj(vector<string> v, int loc)
{
	if (isNeg(v, loc) == true || (isConj(v, loc - 1) == true && v[loc] == "^" && isNeg(v, loc + 1) == true))
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool isDisj(vector<string> v, int loc)
{
	if (isConj(v, loc) == true || (isDisj(v, loc - 1) == true && v[loc] == "v" && isConj(v, loc + 1) == true))
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool isImp(vector<string> v, int loc)
{
	if (isDisj(v, loc) == true || (isDisj(v, loc - 1) == true && v[loc] == "->" && isImp(v, loc + 1) == true))
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool isBEx(vector<string> v, int loc)
{
	if (isImp(v, loc) == true || (isImp(v, loc - 1) == true && v[loc] == "<->" && isBEx(v, loc + 1) == true))
	{
		return true;
	}

	else
	{
		return false;
	}
}

vector<string> comSym(vector<string> v)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == "-" && v[i + 1] == ">")
		{
			v[i] += v[i + 1];
			v.erase(v.begin() + i + 1);
		}
		if (v[i] == "<" && v[i + 1] == "-" && v[i + 2] == ">")
		{
			v[i] += v[i + 1] + v[i + 2];
			v.erase(v.begin() + i + 1);
			v.erase(v.begin() + i + 1);
		}
	}

	return v;
}

pNODE newNode(string s)
{ 
	pNODE node = new AST;
	node->info = s;
	node->children[0] = nullptr;
	node->children[1] = nullptr;
	return node;
}

int opPrecedence(string s)
{
	if (s == "<->"){
		return 1;
	}
	
	if (s == "->") {
		return 2;
	}

	if (s == "v") {
		return 3;
	}

	if (s == "^") {
		return 4;
	}

	if (s == "~") {
		return 5;
	}
	if (s == "(" || s == ")")
	{
		return 6;
	}
}

bool checkTokenGrammar(vector<string> v)
{
	int parenCount = 0;

	if (v[0] != "T" && v[0] != "F" && v[0] != "(" && v[0] != "~")
	{
		return false;
	}
	else if (v.back() != "T" && v.back() != "F" && v.back() !=")")
	{
		return false;
	}
	else
	{
		for (int i = 0; i < (v.size()); i++)
		{
			if (v[i] == "T" || v[i] == "F")
			{
				if (isConst(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == "(")
			{
				if (isUEx(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == ")")
			{
				continue;
			}
			else if (v[i] == "~")
			{
				if (isNeg(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == "^")
			{
				if (isConj(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == "v")
			{
				if (isDisj(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == "->")
			{
				if (isImp(v, i) == false)
				{
					return false;
				}
			}
			else if (v[i] == "<->")
			{
				if (isBEx(v, i) == false)
				{
					return false;
				}
			}
		}

		for (int i = 0; i < v.size(); i++)
		{
			if (v[i] == "(")
			{
				parenCount++;
			}
			else if (v[i] == ")")
			{
				parenCount--;
			}
		}

		if (parenCount == 0)
		{
			return true;
		}

		else
		{
			return false;
		}
	}
}

AST shuntTokens(vector<string> v)
{
	pNODE node, pushToNode;
	Stack op, out;

	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == "T" || v[i] == "F")
		{
			if (op.isEmpty() == false)
			{
				if (op.returnInfo() == "~")
				{
					node = newNode(v[i]);
					out.push(node);
					pushToNode = op.returnAST();
					op.pop();
					out.push(pushToNode);
					out.constructSubTreeUn();
				}
				else
				{
					out.push(newNode(v[i]));
				}
			}
			else
			{
				out.push(newNode(v[i]));
			}
		}
		else if (op.isEmpty() == true && isConst(v, i) == false)
		{
			node = newNode(v[i]);
			op.push(node);
		}
		else if (opPrecedence(v[i]) < opPrecedence(op.returnInfo()) && op.isEmpty() == false && op.returnInfo() != "(")
		{
			node = newNode(v[i]);
			pushToNode = op.returnAST();
			op.pop();
			out.push(pushToNode);
			out.constructSubTreeBin();
			op.push(node);
		}
		else if (v[i] == ")")
		{
		op.push(newNode(v[i]));
		op.pop();
		while (op.returnInfo() != "(")
		{
			if (op.returnInfo() == "~")
			{
				pushToNode = op.returnAST();
				op.pop();
				out.push(pushToNode);
				out.constructSubTreeUn();
			}
			else
			{
				pushToNode = op.returnAST();
				op.pop();
				out.push(pushToNode);
				out.constructSubTreeBin();
			}
		}
		op.pop();
		if (op.isEmpty() == false)
		{
			if (op.returnInfo() == "~")
			{
				pushToNode = op.returnAST();
				op.pop();
				out.push(pushToNode);
				out.constructSubTreeUn();
			}
		}
		}
		else
		{
			node = newNode(v[i]);
			op.push(node);
		}
	}

	while (out.isLastNode() == false)
	{
		node = op.returnAST();
		op.pop();
		out.push(node);
		out.constructSubTreeBin();
	}

	return *out.returnAST();
}

/**********************************************************************************************************/
//SPECIFICATION FUNCTIONS

tokRslt tokenize(string s) {
	tokRslt boolExpression;

	vector<int> charCompare = { 84, 70, 94, 118, 126, 45, 60, 62, 40, 41, 46 }; //ARRAY OF VALID CHARACTERS
	if (s.empty())
	{
		boolExpression.success = false;
		boolExpression.syms.clear();
		return boolExpression;
	}

	if (s[(s.size() - 1)] == 45)
	{
		boolExpression.success = false;
		boolExpression.syms.clear();
		return boolExpression;
	}


	if (s[s.size() - 1] != 46)
	{
		boolExpression.success = false;
		boolExpression.syms.clear();
		return boolExpression;
	}

	for (int i = 0; i < s.size(); i++) //LOOPS THROUGH STRING
	{
		for (int j = 0; j < charCompare.size(); j++) //ITERATES THROUGH charCompare
		{
			if (s[i] == 32)
			{
				break;
			}
			if (s[i] == charCompare[j])
			{
				std::string pushBackVar(1, s[i]);
				boolExpression.syms.push_back(pushBackVar);
				break;
			}
			if (j == (charCompare.size() - 1))
			{
				boolExpression.success = false;
				boolExpression.syms.clear();
				return boolExpression;
			}
		}
	}
	if (boolExpression.syms.empty())
	{
		boolExpression.success = false;
		return boolExpression;
	}
	boolExpression.syms = comSym(boolExpression.syms);
	boolExpression.syms.pop_back();
	boolExpression.success = true;
	return boolExpression;
}

parseRslt parse(vector<string> v)
{
	parseRslt rslt;
	if (checkTokenGrammar(v) == true)
	{
		rslt.ast = shuntTokens(v);
		rslt.success = true;

		return rslt;
	}

	rslt.success = false;

	return rslt;
}

bool eval(AST T)
{
	if (T.info == "T")
	{
		return 1;
	}
	else if (T.info == "F")
	{
		return 0;
	}
	else if (T.info == "~")
	{
		if (eval(*T.children[0]) == 1)
		{
			return 0;
		}
		if (eval(*T.children[0]) == 0)
		{
			return 1;
		}
	}
	else
	{
		bool evalLeftNode = eval(*T.children[0]);
		bool evalRightNode = eval(*T.children[1]);

		if (T.info == "^")
		{
			return evalLeftNode * evalRightNode;
		}
		if (T.info == "v")
		{
			return evalLeftNode + evalRightNode;
		}
		if (T.info == "->")
		{
			if (evalLeftNode == 1 && evalRightNode == 0)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
		if (T.info == "<->")
		{
			if (evalLeftNode == evalRightNode)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
}

TPERslt TPE(string s)
{
	TPERslt TPEOutReturn;

	if (tokenize(s).success == true)
	{
		tokRslt tokens = tokenize(s);
		if (parse(tokens.syms).success == true)
		{
			parseRslt parser = parse(tokens.syms);
			TPEOutReturn.val = eval(parser.ast);
			TPEOutReturn.msg = "success";
			return TPEOutReturn;
		}
		else
		{
			TPEOutReturn.val = NULL;
			TPEOutReturn.msg = "grammar error";
			return TPEOutReturn;
		}
	}
	else
	{
		TPEOutReturn.val = NULL;
		TPEOutReturn.msg = "symbol error";
		return TPEOutReturn;
	}
}

string TPEOut(string s)
{
	if (TPE(s).msg == "success")
	{
		if (TPE(s).val == 1)
		{
			return "true";
		}
		if (TPE(s).val == 0)
		{
			return "false";
		}
	}
	if (TPE(s).msg == "grammar error")
	{
		return "grammar error";
	}
	if (TPE(s).msg == "symbol error")
	{
		return "symbol error";
	}
}

/* 
	Input boolean expression to be checked
	T = True
	F = False
	^ = And
	v = Or
	-> = Implication
	All expressions must end with a period (.)
*/
int main()
{
	string str;
	cout << "Input a boolean expression: ";
	getline(cin, str);
	cout << TPEOut(str) << endl;
	system("pause");
	return 0;
}

