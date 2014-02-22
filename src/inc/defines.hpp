#ifndef B98COMPILER_DEFINES_HPP
#define B98COMPILER_DEFINES_HPP

#define B98_NS_BEGIN namespace b98 {
#define B98_NS_END }

#define B98_PROG_NAME "b98toc"

#define B98_USAGE_MSG "Usage: \"" B98_PROG_NAME " <befunge source file>\"\n"
#define B98_NOFILE_MSG(file) "Error: Unable to open \"" + file + "\"\n"

#define B98_OUTPUT_HEADER_TOP \
	"#ifndef B98_C_OUTPUT_H\n" \
	"#define B98_C_OUTPUT_H\n"

#define B98_OUTPUT_HEADER_BOTTOM \
	"#endif\n"

#define B98_OUTPUT_HEADER_INCLUDES \
	"#include <stdio.h>\n" \
	"#include <stdlib.h>\n"

#define B98_OUTPUT_HEADER_STACK_DECL \
	"typedef int StackType;\n\n" \
	"struct StackNode {\n" \
	"	StackType data;\n" \
	"	struct StackNode *nextNode;\n\n" \
	"};\n\n" \
	"struct Stack {\n" \
	"	struct StackNode *top;\n\n" \
	"};\n\n" \
	"void stackInit(struct Stack *stackPtr);\n" \
	"void stackPush(struct Stack *stackPtr, StackType data);\n" \
	"StackType stackPop(struct Stack *stackPtr);\n" \
	"int stackisEmpty(struct Stack *stackPtr);\n"

#define B98_OUTPUT_SRC_TOP \
	"#include \"output.h\"\n"

#define B98_OUTPUT_SRC_STACK_IMP \
	"void stackInit(struct Stack *stackPtr) {\n" \
	"	stackPtr->top = NULL;\n" \
	"\n" \
	"} // void stackInit(struct Stack *stackPtr);\n" \
	"\n" \
	"void stackPush(struct Stack *stackPtr, StackType data) {\n" \
	"	struct StackNode *tmp = NULL;\n" \
	"	tmp = malloc(sizeof(struct StackNode));\n" \
	"\n" \
	"	tmp->data = data;\n" \
	"\n" \
	"	tmp->nextNode = stackPtr->top;\n" \
	"	stackPtr->top = tmp;\n" \
	"\n" \
	"} // void stackPush(struct Stack *stackPtr, StackType data);\n" \
	"\n" \
	"StackType stackPop(struct Stack *stackPtr) {\n" \
	"	if(stackIsEmpty(stackPtr)) {\n" \
	"		return 0;" \
	"	}\n\n" \
	"	struct StackNode *tmp = stackPtr->top;\n" \
	"\n" \
	"	StackType data = stackPtr->top->data;\n" \
	"	stackPtr->top = stackPtr->top->nextNode;\n" \
	"\n" \
	"	free(tmp);\n" \
	"\n" \
	"	return data;\n" \
	"\n" \
	"} // StackType stackPop(struct Stack *stackPtr);\n" \
	"\n" \
	"int stackIsEmpty(struct Stack *stackPtr) {\n" \
	"	return (stackPtr->top == NULL);\n" \
	"\n" \
	"} // int stackIsEmpty(*stackPtr);\n"

#define B98_OUTPUT_SRC_MAIN \
	"int main(void) {\n" \
	"	struct Stack stack;\n" \
	"	stackInit(&stack);\n" \
	"	return state0(&stack);\n\n" \
	"}\n"

#endif // B98COMPILER_DEFINES_HPP
