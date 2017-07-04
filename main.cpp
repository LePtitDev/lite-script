#include <iostream>
#include <fstream>

#include "src/litescript.hpp"

using namespace LiteScript;

int main(int argc, char * argv[]) {
    Memory memory;
    Variable v1 = memory.Create(),
             v2 = memory.Create(Type::BOOLEAN),
             v3 = memory.Create(Type::NUMBER),
             v4 = memory.Create(Type::STRING),
             v5 = memory.Create();

    v2->GetData<bool>() = true;
    v3->GetData<Number>() = 32.0f;
    v4->GetData<String>() = "LOL";

    v5 = v4 + v3 + v2;

    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
    std::cout << v3 << std::endl;
    std::cout << v4 << std::endl;
    std::cout << v5 << std::endl;
    v3->GetData<Number>() = 2;
    v4[v3] = v4;
    std::cout << v4 << std::endl;

    Nullable<Variable> test1 = memory.GetVariable(10);
    if (test1.isNull)
        std::cout << "1. GetVariable correct" << std::endl;
    else
        std::cout << "1. GetVariable non correct" << std::endl;

    Nullable<Variable> test2 = memory.GetVariable(2);
    if (!test2.isNull)
        std::cout << "2. GetVariable correct" << std::endl;
    else
        std::cout << "2. GetVariable non correct" << std::endl;

    State state(memory);
    state.ExecuteSingle(Instruction(InstrCode::VALUE_UNDEFINED));
    state.ExecuteSingle(Instruction(InstrCode::DEFINE_VARIABLE, "ma-variable"));
    state.ExecuteSingle(Instruction(InstrCode::VALUE_NULL));
    state.ExecuteSingle(Instruction(InstrCode::DEFINE_VARIABLE, "autre-variable"));

    Namespace& nsp = state.GetCurrentNamespace()->GetData<Namespace>();
    std::cout << "variable count = " << nsp.Count() << std::endl;
    for (unsigned int i = 0, sz = nsp.Count(); i < sz; i++) {
        std::cout << "> " << nsp.GetKey(i) << " = " << nsp.GetVariable(i) << std::endl;
    }

    return 0;
}