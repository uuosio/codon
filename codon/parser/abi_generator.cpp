#include "codon/parser/common.h"
#include "abi_generator.h"

string get_type_name(ExprPtr expr) {
  auto& ty = *expr.get();
  return typeid(ty).name();
}

string extract_attr_name(string attr, vector<ExprPtr>& decorators) {
  for (auto &a: decorators) {
    auto call = a->getCall();
    if (!call) {
      continue;
    }

    if (attr != call->expr->getId()->value) {
      continue;
    }

    if (call->args.size() == 0) {
      continue;
    }
    auto value = call->args[0].value->getString();
    if (!value) {
      continue;
    }

    return value->strings[0].first;
  }
  return "";
}

bool is_notify_action(vector<ExprPtr>& decorators) {
  for (auto &a: decorators) {
    auto call = a->getCall();
    if (!call) {
      continue;
    }

    if (Attr::Action != call->expr->getId()->value) {
      continue;
    }

    if (call->args.size() != 2) {
      continue;
    }

    if (call->args[1].name != "notify") {
      continue;
    }

    if (BoolExpr *value = dynamic_cast<BoolExpr*>(call->args[1].value.get())) {
      return value->value;
    }
  }
  return false;
}

ABIGenerator::ABIGenerator() {

}

string parse_abi_type(string id) {
  if (id == "Name") {
    return "name";
  } else if (id == "str") {
    return "string";
  } else if (id == "int") {
    return "int64";
  } else if (id == "u64") {
    return "uint64";
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

void ABIGenerator::addTable(ClassStmt *stmt, std::vector<Param>& args) {
  auto tableName = extract_attr_name(Attr::Table, stmt->decorators);
  auto structName = stmt->name;

  ABIStruct abi_struct = {structName, "", {}};
  ABITable abi_table = {tableName, structName, "i64", {}, {}};

  for (auto &a : args) {
    // .__vtable__.std.internal.builtin.object
    if (a.name.find(".__vtable__") == 0) {
      continue;
    }
    ABIStructField field = {a.name, ""};
    // LOG("++++ {} {} {} {}", a.name, a.type, get_type_name(a.type), a.defaultValue);
    if (auto* tp = dynamic_cast<InstantiateExpr*>(a.type.get())) {
      //std.internal.types.ptr.List
      if (auto *id = tp->typeExpr->getId()) {
        if (id->value == "std.internal.types.ptr.List") {
          if (auto *typeId = tp->typeParams[0]->getId()) {
            field.type = parse_abi_type(typeId->value) + "[]";
          }
        }
      }
    } else if (auto *id = a.type->getId()) {
      field.type = parse_abi_type(id->value);
    }
    abi_struct.fields.emplace_back(field);
  }
  abi.structs.emplace_back(abi_struct);
  abi.tables.emplace_back(abi_table);
}

bool ABIGenerator::addAction(FunctionStmt *stmt) {
  auto action_name = extract_attr_name(Attr::Action, stmt->decorators);
  if (action_name.empty()) {
    return false;
  }

  LOG("+++++++++=action name: {}", action_name);

  actionStmts.push_back(stmt);

  ABIStruct abi_struct = {action_name, "", {}};

  for (auto &a: stmt->args) {
    if (a.name == "self") {
      continue;
    }

    if (!a.type) {
      continue;
    }

    ABIStructField field = {a.name, ""};
    if (auto *value = a.type->getString()) {
      field.type = "string";
      // LOG("String: {}, prefix: {}", value->strings[0].first, value->strings[0].second);
    } else if (auto *expr = a.type->getInt()) {
        field.type = "int64";
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
  
  abi.structs.emplace_back(abi_struct);
  abi.actions.emplace_back(ABIAction{action_name, action_name, ""});
  return true;
}

string ABIGenerator::generate() {
  nlohmann::ordered_json j;
  this->abi.version = "eosio::abi/1.2";
  to_json(j, this->abi);
  return j.dump(4);
}
