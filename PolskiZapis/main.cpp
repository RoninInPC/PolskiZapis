#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<stack>
#include <sstream>
#include<string.h>
using namespace std;
enum term_type {//типы термов: константа, переменна€, операци€ (между переменными, слева или справа), открывающа€ и закрывающа€ скобка, функци€
	t_const,	//t_zero нужно дл€ первой функции, дл€ второй нет
	t_var,
	t_opbl,
	t_clbl,
	t_opl,
	t_op,
	t_opr,
	t_fun,
	t_zero
};
struct term {
	string body;
	term_type type;
};
struct operation {
	string body;
	term_type type;
	int priority;
};
set<char> Oper = { '*','/','-','+','&','|','^','!','%' };
set<char> Opbl = { '<','(','[','{' };
set<char> Clbl = { '>',')',']','}' };
vector<operation> Operations = {{"^",t_op,8},{"-",t_opl,8}, {"*",t_op,7},{"/",t_op,7},{"%",t_op,7},
{"!",t_opr,8},{"+",t_op,5},{"-",t_op,5},{"&",t_op,4},{"|",t_op,4}, {"!",t_opl,8},{"||",t_op,3},{"&&",t_op,3} };// 10 балльна€ шкала, где приоритет 10 у функции
vector<term> MakeTerm(string Expression) {
	vector<term> Ans;
	term PartAns;
	string Body;
	term_type Type = t_zero;
	int i = 0;
	while (i < Expression.size()) {
		char c = Expression[i];
		if (c >= '0' && c <= '9' && Type!=t_op) {
			Body.push_back(c);
			Type = t_const;
			i++;
		}
		else {
			if (Body.size() != 0 && Type==t_const) {
				PartAns.body = Body;
				PartAns.type = Type;
				Ans.push_back(PartAns);
				Body.clear();
			}
		}
		if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
			if (Type != t_op) {
				Body.push_back(c);
				Type = t_var;
				i++;
			}
		}
		else {
			if (c=='(') {
				Type = t_fun;
			}
			if (Body.size() != 0 && (Type == t_var || Type == t_fun)) {
				PartAns.body = Body;
				PartAns.type = Type;
				Ans.push_back(PartAns);
				Body.clear();
			}
		}
		if (Oper.find(c)!= Oper.end()) {
			Body.push_back(c);
			Type = t_op;
			i++;
		}
		else {
			if (Body.size() != 0 && Type == t_op) {
				PartAns.body = Body;
				PartAns.type = Type;
				Ans.push_back(PartAns);
				Body.clear();
				Type = t_zero;
			}
		}
		if (Opbl.find(c) != Opbl.end()) {
			Body.push_back(c);
			Type = t_opbl;
			i++;
			PartAns.body = Body;
			PartAns.type = Type;
			Ans.push_back(PartAns);
			Body.clear();
		}
		if (Clbl.find(c) != Clbl.end()) {
			Body.push_back(c);
			Type = t_clbl;
			i++;
			PartAns.body = Body;
			PartAns.type = Type;
			Ans.push_back(PartAns);
			Body.clear();
		}
	}
	if (Body.size() != 0) {
		PartAns.body = Body;
		PartAns.type = Type;
		Ans.push_back(PartAns);
		Body.clear();
	}
	return Ans;
}
void split(string const& str, const char delim,	vector<std::string>& out)//мо€ функци€ разделени€ строки 
{
	stringstream ss(str);
	string s;
	while (getline(ss, s, delim)) {
		out.push_back(s);
	}
}
vector<term> MakeTerm2(string expression) {
	vector<term> Ans;
	vector<string> BodyAns;
	split(expression, ' ',BodyAns);//разбиение
	term PartAns;
	term_type Type;
	for (int i = 0; i < BodyAns.size(); i++) {
		//блок определени€ типа
		if (Oper.find(BodyAns[i][0]) != Oper.end()) {
			Type = t_op;
		}
		if (isalpha((unsigned char)BodyAns[i][0])) {
			Type = t_var;
		}
		if (isdigit(BodyAns[i][0])) {
			Type = t_const;
		}
		if (Opbl.find(BodyAns[i][0]) != Opbl.end()) { 
			if (i > 0 && Ans[i - 1].type == t_var) { //если перед скобкой переменна€ это функци€
				Ans[i - 1].type = t_fun;
			}
			if (i > 0 && Ans[i - 1].type == t_op) { // если перед скобкой операци€ то...
				Ans[i - 1].type = t_opl;
			}
			if (i > 0 && Ans[i - 1].type == t_opr) {
				Ans[i - 1].type = t_op;
			}
			Type = t_opbl;
		}
		if (Clbl.find(BodyAns[i][0]) != Clbl.end()) {
			Type = t_clbl;
		}
		//блок корректировки типа операций
		if (Type == t_op && i > 0) {
			if (Ans[i - 1].type == t_var || Ans[i - 1].type == t_const || Ans[i - 1].type == t_fun || Ans[i - 1].type == t_clbl) {
				Type = t_opr;
			}
		}
		if ((Type == t_var || Type == t_const || Type == t_fun ) && i > 0) {
			if (Ans[i - 1].type == t_op) {
				Ans[i - 1].type = t_opl;
			}
			if (Ans[i - 1].type == t_opr) {
				Ans[i - 1].type = t_op;
			}
		}
		//добавление
		PartAns.body = BodyAns[i];
		PartAns.type = Type;
		Ans.push_back(PartAns);
	}
	return Ans;
}
int InfoPriority(term a) {//функци€ дл€ возращени€ приоритета
	for (int i = 0; i < Operations.size(); i++) {
		if (a.body == Operations[i].body && a.type == Operations[i].type) {
			return Operations[i].priority;
		}
	}
	return 0;
}
int TopStackPriority(stack<int> a) {
	if (a.empty()) {
		return 0;
	}
	else {
		return a.top();
	}
}
vector<term> PolskaNotation(vector<term> Terms) {
	stack<int> SPriority;//стек приоритетов терм в конец стека  S соответсвует приоритету в конце данного стека
	stack<term> S;
	vector<term> out;
	for (int i = 0; i < Terms.size(); i++) {
		if (Terms[i].type == t_fun) {
			S.push(Terms[i]);
			SPriority.push(10); // приоритет у функции 10
		}
		if (Terms[i].type == t_opbl) {
			S.push(Terms[i]);
			SPriority.push(0); //у скобки 0
		}
		if (Terms[i].type == t_const || Terms[i].type == t_var) {
			out.push_back(Terms[i]); //вывод в out переменных и констант
		}
		if (Terms[i].type == t_op || Terms[i].type == t_opl || Terms[i].type == t_opr) {//если приоритет в строке больше 
			//то заносим в стеки
			int Priority = InfoPriority(Terms[i]);			//иначе убираем из стеков пока приоритет в строке не станет больше
			int TopStack = TopStackPriority(SPriority);
			while (Priority <= TopStack) {
				out.push_back(S.top());
				S.pop();
				SPriority.pop();
				TopStack = TopStackPriority(SPriority);
			}
			if (Priority > TopStack) {
				S.push(Terms[i]);
				SPriority.push(Priority);
			}
		}
		if (Terms[i].type == t_clbl) { //если закрывающа€ то переносим из стека в out, потом удал€ем скобку
			while (S.top().type != t_opbl) {
				out.push_back(S.top());
				S.pop();
				SPriority.pop();
			}
			S.pop();
			SPriority.pop();
		}
	}
	while (!S.empty()) { //после алгоритма выносим остатки из стека
		out.push_back(S.top());
		S.pop();
		SPriority.pop();
	}
	return out;
}
vector<term> WriteVar(vector<term> PolNot) { //замен€ем переменные на константы
	for (int i = 0; i < PolNot.size(); i++) {
		if (PolNot[i].type == t_var) {
			cout << "Write var " << PolNot[i].body<<endl;
			string change = PolNot[i].body;
			string changed;
			cin >> changed;
			for (int j = i; j < PolNot.size(); j++) {
				if (PolNot[j].body == change) {
					PolNot[j].body = changed;
					PolNot[j].type = t_const;
				}
			}
		}
	}
	return PolNot;
}
float CalculationPolskaNotation(vector<term> PolNot) {//вычисление
	stack<float> S;
	for (int i = 0; i < PolNot.size(); i++) { 
		if (PolNot[i].type == t_const) { //константа сразу заноситс€ в стек
			S.push(atof(PolNot[i].body.c_str()));
		}
		if (PolNot[i].type == t_opl) { //если операци€ левосторонн€€
			float Top = S.top();
			S.pop();
			if(PolNot[i].body=="!"){
				Top = !Top;
			}
			if (PolNot[i].body == "-") {
				Top = -Top;
			}
			S.push(Top);
		}
		if (PolNot[i].type == t_opr) { //если операци€ правосторонн€€
			float Top = S.top();
			S.pop();
			if (PolNot[i].body == "!") {
				int mul = 1;
				for (int i = 2; i < Top; i++) {
					mul *= i;
				}
				Top = mul;
			}
			S.push(Top);
		}
		if (PolNot[i].type == t_op) { //если операци€ между переменными
			float Top = S.top();
			S.pop();
			float Top2 = S.top();
			S.pop();
			float Ans = 0;
			if (PolNot[i].body == "+") {
				Ans = Top2 + Top;
			}
			if (PolNot[i].body == "-") {
				Ans = Top2 - Top;
			}
			if (PolNot[i].body == "*") {
				Ans = Top2 * Top;
			}
			if (PolNot[i].body == "/") {
				Ans = Top2 / Top;
			}
			if (PolNot[i].body == "^") {
				Ans = pow(Top2, Top);
			}
			if (PolNot[i].body == "%") {
				Ans = (int)Top2 % (int)Top;
			}
			if (PolNot[i].body == "|") {
				Ans = (int)Top2 | (int)Top;
			}
			if (PolNot[i].body == "&") {
				Ans = (int)Top2 & (int)Top;
			}
			if (PolNot[i].body == "||") {
				Ans = (int)Top2 || (int)Top;
			}
			if (PolNot[i].body == "&&") {
				Ans = (int)Top2 && (int)Top;
			}
			S.push(Ans);
		}
		if (PolNot[i].type == t_fun) { //если функци€
			if (PolNot[i].body == "sin") {
				float Top = S.top();
				S.pop();
				S.push(sin(Top));
			}
			if (PolNot[i].body == "cos") {
				float Top = S.top();
				S.pop();
				S.push(cos(Top));
			}
			if (PolNot[i].body == "tan") {
				float Top = S.top();
				S.pop();
				S.push(tan(Top));
			}
			if (PolNot[i].body == "exp") {
				float Top = S.top();
				S.pop();
				S.push(exp(Top));
			}
		}
	}
	return S.top();
}
int main()
{
	string s = "- abc - b ! * ! sin ( x - ( y / z - exp ( 3.15 * x ) ) )";
	string s1 = "( abc + 123 ) / 2";
	vector<term> Ans = MakeTerm2(s);
	for (term& t : Ans) {
		cout << t.body<<" "<<t.type<< endl;
	}
	cout << endl;
	Ans = PolskaNotation(Ans);
	for (term& t : Ans) {
		cout << t.body << " ";
	}
	cout << endl;
	Ans = WriteVar(Ans);
	for (term& t : Ans) {
		cout << t.body << " ";
	}
	float Answer = CalculationPolskaNotation(Ans);
	cout << endl << (float)Answer;
	return 0;
}