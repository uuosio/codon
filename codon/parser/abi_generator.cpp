#include "codon/parser/common.h"
#include "abi_generator.h"

ABIGenerator::ABIGenerator() {

}

string get_type_name(ExprPtr expr) {
  auto& ty = *expr.get();
  return typeid(ty).name();
}

string parse_abi_type(string id) {
  if (id == "Name") {
    return "name";
  } else if (id == "str") {
    return "string";
  }
  return "";
}

ABIGenerator& ABIGenerator::instance() {
  static ABIGenerator *abi_generator = nullptr;
  if (abi_generator == nullptr) {
    abi_generator = new ABIGenerator();
  }
  return *abi_generator;
}

void ABIGenerator::add_action(FunctionStmt *stmt) {
  LOG("[global] {}", stmt->name);
  bool is_action = false;
  string action_name = "";
  //(call 'action  (string ("sayhello")))
  for (auto &a: stmt->decorators) {
    auto call = a->getCall();
    if (!call) {
      continue;
    }

    std::cout<<"    "<<get_type_name(a)<<":"<<a->toString()<<std::endl;
    if (Attr::Action != call->expr->getId()->value) {
      continue;
    }

    if (call->args.size() != 1) {
      continue;
    }
        
    auto value = call->args[0].value->getString();
    if (!value) {
      continue;
    }

    is_action = true;

    action_name = value->strings[0].first;
    LOG("++++++++action name: {}", action_name);
    this->abi.actions.emplace_back(ABIAction{action_name, action_name, ""});
    continue;

    for (auto &arg: call->args) {
      std::cout<<"      typeid:"<<get_type_name(arg.value)<<", name:"<<arg.name<<std::endl;
      if (auto *value = arg.value->getString()) {
          // std::cout<<"value: "<<value->getString()->getValue()<<std::endl;
          std::cout<<"String: "<<value->strings[0].first<<", prefix: "<<value->strings[0].second<<std::endl;
      } else if (auto *value = arg.value->getInt()) {
          std::cout<<"Int: "<<value->value<<", suffix: "<<value->suffix<<std::endl;
      } else if (auto *value = arg.value->getList()) {
          std::cout<<"List: "<<value->toString()<<std::endl;
      }
    }
  }

  if (!is_action) {
    return;
  }

// struct ABIStructField {
//   string name;
//   string type;
// };

// struct ABIStruct {
//   string name;
//   string base;
//   vector<ABIStructField> fields;
// };

  ABIStruct abi_struct = {action_name, "", {}};

  for (auto &a: stmt->args) {
    LOG("+++a.name: {}", a.name);
    LOG("    a.toString(): {} {}", a.toString(), (void *)a.type.get());
    if (a.name == "self") {
      continue;
    }

    if (!a.type) {
      continue;
    }

    LOG("    type: {}", get_type_name(a.type));
    ABIStructField field = {a.name, ""};
    if (auto *value = a.type->getString()) {
      field.type = "string";
      LOG("string type");
        // std::cout<<"value: "<<value->getString()->getValue()<<std::endl;
        LOG("String: {}, prefix: {}", value->strings[0].first, value->strings[0].second);
    } else if (auto *expr = a.type->getInt()) {
        field.type = "int64";
        LOG("Int type");
        LOG("Int: {}, suffix: {}", expr->value, expr->suffix);
    } else if (auto *expr = a.type->getId()) {
        field.type = parse_abi_type(expr->value);
    } else if (auto *expr = a.type->getList()) {
        LOG("List type");
        LOG("List: {}", expr->toString());
    } else if (auto *expr = a.type->getIndex()) {
        // ExprPtr expr, index;
        LOG("+++++++expr.expr.type: {}", get_type_name(expr->expr));
        LOG("+++++++expr.index.type: {}", get_type_name(expr->index));
        LOG("++++++++IndexExpr: {} {}", expr->expr->getId()->value, expr->index->getId()->value);
    }
    abi_struct.fields.emplace_back(field);
  }
  this->abi.structs.emplace_back(abi_struct);
}

string ABIGenerator::generate() {
  nlohmann::ordered_json j;
  this->abi.version = "eosio::abi/1.2";
  to_json(j, this->abi);
  return j.dump(4);
}

// nlohmann::ordered_json j;
// ABI abi;
// abi.version = "eosio::abi/1.2";
// abi.structs.emplace_back(ABIStruct{"test2", "", { {"b", "int"}, {"a", "int"}}});
// abi.structs.emplace_back(ABIStruct{"test", "", {{"a", "int"}, {"b", "int"}}});
// to_json(j, abi);

// std::cout << j.dump(4) << std::endl;
