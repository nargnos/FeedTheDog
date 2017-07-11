#include <stdio.h>
#include <sstream>
#include "StringCommands.h"
#include "HashCommands.h"
#include "TransactionsCommands.h"
int main(void) {
	std::ostringstream out;
	HashCommands::Hset(out, "hash", "one", "value");
	HashCommands::Hget(out, "hash", "one");
	//StringCommands::Get(out, "a");	
	//TransactionsCommands::Watch(out, "asdas");
	auto str = out.str();
	auto x = str.data();
}