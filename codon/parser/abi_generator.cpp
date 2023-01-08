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

std::map<string, string> abi_types_map = {
  {"bool", "bool"},
  {"i8", "int8"},
  {"u8", "uint8"},
  {"i16", "int16"},
  {"u16", "uint16"},
  {"i32", "int32"},
  {"u32", "uint32"},
  {"i64", "int64"},
  {"u64", "uint64"},
  {"i128", "int128"},
  {"u128", "uint128"},
  {"VarInt32", "varint32"},
  {"VarUint32", "varuint32"},
  {"float32", "float32"},
  {"float", "float64"},
  {"Float128", "float128"},
  {"TimePoint", "time_point"},
  {"TimePointSec", "time_point_sec"},
  {"BlockTimestampType", "block_timestamp_type"},
  {"Name", "name"},
  {"Bytes", "bytes"},
  {"str", "string"},
  {"Checksum160", "checksum160"},
  {"Checksum256", "checksum256"},
  {"Checksum512", "checksum512"},
  {"PublicKey", "public_key"},
  {"Signature", "signature"},
  {"Symbol", "symbol"},
  {"SymbolCode", "symbol_code"},
  {"Asset", "asset"},
  {"ExtendedAsset", "extended_asset"}
};


string parse_abi_type(string id) {
  if (abi_types_map.find(id) != abi_types_map.end()) {
    return abi_types_map[id];
  }
  return id;
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
