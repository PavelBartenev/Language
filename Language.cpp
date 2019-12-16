#define _CRT_SECURE_NO_WARNINGS                

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <string.h>

enum error_codes
{
	calloc_fail = 1313,
	arg_fail = 1414,
	file_fail = 1515,
};

enum commands
{
	Add = 1,
	Sub = 2,
	Mul = 3,
	Div = 4,
	var = 5,
	operation = 6,
	func = 7,
	num = 8,
	varx = 9,
	power = 10,
	noth = 11,
	operator_symbols = 12,
};

enum functions
{
	sin = 15,
	cos = 16,
	tan = 17,
	ln = 18,
};

struct NODE
{
	int data;
	char* letter_data;
	NODE* left;
	NODE* right;
	NODE* prev;
	int type;
};

int MAXSIZE_OF_VAR = 100;
int g_dump_work = 0;

#define DEBUG

#ifdef DEBUG
#define DUMP(code)																								  \
    {																											  \
       if (g_dump_work == 0)																				      \
	   {                                                                                                          \
               printf("FAILED  LINE %ld\nFUNCTION FAILED %s\n\n", __LINE__, __FUNCTION__);					      \
		       printf("CODE %d", code);                                                                           \
	   }																										  \
       g_dump_work++;                                                                                             \
    }                    																						  \

#else                                                                                                                                                        
#define DUMP(this_);      
#endif

const char* current_symbol = "";

int g_number = 0;

char* DeleteSpaces(char*);

NODE* BuildTree(FILE*);

NODE* OptimizedOut(NODE*, FILE*);

NODE* GetG(char*);

NODE* GetE();

NODE* GetT();

NODE* GetP();

NODE* GetN();

NODE* GetF();

NODE* GetOperator();

NODE* GetNextLine();

NODE* Create_node(char*, int);

NODE* Create_node(int, int);

int Draw(NODE*);

int Draw_param(NODE*, FILE*);

int Draw_tree(NODE*, FILE*, int, char*);

int Draw_node(NODE*, FILE*, char*);

int Graphviz_prepare(NODE*, int);

char* operations_array();

char** functions_array();

NODE* Differenciator(NODE*);

NODE* add_sub_diff(NODE*);

NODE* mul_diff(NODE*);

NODE* div_diff(NODE*);

NODE* power_diff(NODE*);

NODE* func_diff(NODE*);

int PrintTree(NODE*, FILE*, char*, char**);

int PrintNode(NODE*, FILE*, char*, char**);

int DetectFunction();

int priory(NODE*);

NODE* Copy(NODE*);

int Optimizator(NODE*);

int add_sub_optimize(NODE*);

int mul_optimize(NODE*);

int power_optimize(NODE*);

int second_link(NODE*, NODE*);


int main()
{

	FILE* input = fopen("input.txt", "r");

	NODE* head = BuildTree(input);

	//NODE* diff = Differenciator(head);

	//FILE* output = fopen("output.txt", "w");

	//OptimizedOut(diff, output);

	Draw(head);

	system("dot -Tpng D:\\vs_projects\\Language\\graph_code_test.txt -oD:\\vs_projects\\Language\\graph_image.png");


	return 0;
}

char* DeleteSpaces(char* str)
{
	int size = strlen(str);

	char* no_spaces_str = (char*)calloc(2*size, sizeof(char));

	//printf("%d", strlen(str));

	int j = 0;

	for (int i = 0; i < size; i++)
	{
		if (str[i] != ' ' && str[i] != '\n' && str[i] != '	')
		{
			no_spaces_str[j] = str[i];
			j++;
		}
	}

	return no_spaces_str;
}

NODE* BuildTree(FILE* input)
{
	if (!input)
	{
		DUMP(file_fail);
		return 0;
	}

	fseek(input, 0, SEEK_END);
	int size = ftell(input);
	fseek(input, 0, SEEK_SET);

	char* str = (char*)calloc(size, sizeof(char));

	fscanf(input, "%[^EOF]", str);

	str = DeleteSpaces(str);

	//str[strlen(str)] = '\0';

	NODE* head = GetG(str);

	return head;
}

char* operations_array()
{
	char* operations = (char*)calloc(20, sizeof(char));

	if (!operations)
	{
		DUMP(calloc_fail);
		return 0;
	}

	operations[Add] = '+';
	operations[Sub] = '-';
	operations[Mul] = '*';
	operations[Div] = '/';
	operations[power] = '^';
	operations[varx] = 'x';

	return operations;
}


NODE* GetG(char* str)
{
	current_symbol = str;
	NODE* head = GetNextLine();
	assert(*current_symbol == '\0');

	return head;
}

NODE* GetNextLine()
{
	NODE* left_node = GetOperator();

	while (*current_symbol == ';')
	{
		char* Operator = (char*)calloc(5, sizeof(char));

		sscanf(current_symbol, "%[;]", Operator);

		current_symbol += 1;

		NODE* new_node = Create_node(Operator, operator_symbols);

		new_node->left = left_node;
		new_node->right = GetOperator();

		left_node = new_node;
	}

	return left_node;
}

NODE* GetOperator()
{
	NODE* left_node = GetE();

	if (!strncmp(current_symbol, "==", 2) || !strncmp(current_symbol, "+=", 2) || !strncmp(current_symbol, "-=", 2))
	{
		char* Operator = (char*)calloc(5, sizeof(char));

		sscanf(current_symbol, "%[+=-]", Operator);

		current_symbol += 2;

		NODE* new_node = Create_node(Operator, operator_symbols);

		new_node->left = left_node;
		new_node->right = GetE();

		return new_node;
	}

	if (*current_symbol == '=')
	{
		char* Operator = (char*)calloc(5, sizeof(char));
			
		sscanf(current_symbol, "%[=]", Operator);

		current_symbol += 1;

		NODE* new_node = Create_node(Operator, operator_symbols);

		new_node->left = left_node;
		new_node->right = GetE();

		return new_node;
	}

	return left_node;
}

NODE* GetE()
{
	NODE* left_node = GetT();

	while ((*current_symbol == '+' || *current_symbol == '-') && strncmp(current_symbol, "+=", 2) && strncmp(current_symbol, "-=", 2))
	{
		char op = *current_symbol;
		current_symbol++;

		NODE* right_node = GetT();

		NODE* new_node = 0;

		if (op == '+')
			new_node = Create_node(Add, operation);
		else
			new_node = Create_node(Sub, operation);

		new_node->left = left_node;

		new_node->right = right_node;

		left_node = new_node;
	}

	return left_node;
}

NODE* GetT()
{
	NODE* left_node = GetP();

	while (*current_symbol == '*' || *current_symbol == '/')
	{
		char op = *current_symbol;
		current_symbol++;

		NODE* right_node = GetP();

		NODE* new_node = 0;

		if (op == '*')
			new_node = Create_node(Mul, operation);
		else
			new_node = Create_node(Div, operation);

		new_node->left = left_node;
		new_node->right = right_node;

		left_node = new_node;
	}

	return left_node;
}

NODE* GetP()
{
	if (*current_symbol == '(')
	{
		current_symbol++;
		NODE* inside = GetE();
		assert(*current_symbol == ')');
		current_symbol++;
		return inside;
	}

	else if (*current_symbol == '!')
	{
		char* variable = (char*)calloc(MAXSIZE_OF_VAR, sizeof(char));

		sscanf(current_symbol, "%[!QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm]", variable);

		current_symbol += strlen(variable);

		return Create_node(variable, var);
	}

	else
		return GetN();
}

NODE* GetN()
{
	int val = 0;

	while ('0' <= *current_symbol && *current_symbol <= '9')
	{
		val = 10 * val + (*current_symbol - '0');
		current_symbol++;
	}

	return Create_node(val, num);
}

int DetectFunction()
{
	char* current_function = (char*)calloc(20, sizeof(char));

	if (!current_function)
		return 1;

	memcpy(current_function, current_symbol, 10);

	if (!strncmp(current_function, "sin", 3))
		return sin;

	if (!strncmp(current_function, "cos", 3))
		return cos;

	if (!strncmp(current_function, "tan", 3))
		return tan;

	if (!strncmp(current_function, "ln", 2))
		return ln;

	return 0;
}

NODE* Create_node(int data, int type)
{
	NODE* new_node = (NODE*)calloc(1, sizeof(NODE));

	if (!new_node)
	{
		DUMP(calloc_fail);
		return 0;
	}

	new_node->data = data;

	new_node->type = type;

	return new_node;
}

NODE* Create_node(char* data, int type)
{
	NODE* new_node = (NODE*)calloc(1, sizeof(NODE));

	if (!new_node)
	{
		DUMP(calloc_fail);
		return 0;
	}

	new_node->letter_data = data;

	new_node->type = type;

	return new_node;
}


int Draw(NODE* node)
{
	FILE* output = fopen("graph_code_test.txt", "w");

	char* commands = operations_array();

	Draw_param(node, output);
	Draw_tree(node, output, 1, commands);
	fprintf(output, "}\n}");

	fclose(output);

	return 0;
}

int Draw_param(NODE* node, FILE* output)
{
	fprintf(output, "digraph MyPlan {\n");
	fprintf(output, "  node[shape = \"circle\", style = \"filled\", fillcolor = \"white\", fontcolor = \"#FFFFFF\", margin = \"0.01\"];\n");
	fprintf(output, "  edge [style=\"dashed\"];\n");

	fprintf(output, "{\n node[shape = \"plaintext\",style = \"invisible\"];\n edge[color = \"white\"];\n 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> 11 -> 12 -> 13 -> 14;\n}\n");

	fprintf(output, "subgraph tree {\n   node [shape=\"ellipse\", style=\"filled\", fillcolor=\"white\", fontcolor=\"black\", fontsize=\"15\"];\n");

	return 0;
}

int Draw_tree(NODE* node, FILE* output, int level, char* commands)
{
	if (node->left)
	{
		fprintf(output, "   { rank = \"same\"; \"%d\"; ", level);
		Draw_node(node, output, commands);
		fprintf(output, "  ;}\n");
		Draw_node(node, output, commands);
		fprintf(output, "->");
		Draw_node(node->left, output, commands);
		fprintf(output, "  ;\n");

		Draw_tree(node->left, output, (level + 1), commands);
	}


	if (node->right)
	{
		fprintf(output, "   { rank = \"same\"; \"%d\"; ", level);
		Draw_node(node, output, commands);
		fprintf(output, "  ;}\n");
		Draw_node(node, output, commands);
		fprintf(output, "->");
		Draw_node(node->right, output, commands);
		fprintf(output, "  ;\n");

		Draw_tree(node->right, output, (level + 1), commands);
	}

	return 0;
}

int Draw_node(NODE* node, FILE* output, char* commands)
{
	if (node->type == num)
		fprintf(output, "   \"%d   type %p\" ", node->data, &(node->data));

	if (node->type == operation)
		fprintf(output, "   \"%c   type %p\" ", commands[node->data], &(node->data));

	if (node->type == noth)
		fprintf(output, "   \"nothing   type %p\" ", &(node->data));

	if (node->type == var || node->type == operator_symbols)
		fprintf(output, "   \"%s   type %p\" ", node->letter_data, &(node->data));

	return 0;
}