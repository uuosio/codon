#pragma once

#include <nlohmann/json.hpp>
#include "codon/parser/ast.h"

using namespace std;
using namespace codon::ast;

#define NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(Type, ...)  \
  inline void to_json(nlohmann::ordered_json& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, __VA_ARGS__)) } \
  inline void from_json(const nlohmann::ordered_json& nlohmann_json_j, Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, __VA_ARGS__)) }

struct ABITable {
	string name;
	string type;
	string index_type;
	vector<string> key_names;
	vector<string> key_types;
};

NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(ABITable,
	name,
	type,
	index_type,
	key_names,
	key_types);

// type ABIAction struct {
// 	Name              string `json:"name"`
// 	Type              string `json:"type"`
// 	RicardianContract string `json:"ricardian_contract"`
// }

// export class ABIAction {
// 	constructor(
// 		public name: string = "",
// 		public type: string = "",
// 		public ricardian_contract = ""
// 	) {}
// }

struct ABIAction {
  string name;
  string type;
  string ricardian_contract;
};

NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(ABIAction,
  name,
  type,
  ricardian_contract);

// type ABIStructField struct {
// 	Name string `json:"name"`
// 	Type string `json:"type"`
// }

// export class ABIStructField {
// 	name = "";
// 	type = "";
// }

// type ABIStruct struct {
// 	Name   string           `json:"name"`
// 	Base   string           `json:"base"`
// 	Fields []ABIStructField `json:"fields"`
// }
// export class ABIStruct {
// 	name = "";
// 	base = "";
// 	fields: ABIStructField[] = [];
// }

// type ABI struct {
// 	Version          string      `json:"version"`
// 	Structs          []ABIStruct `json:"structs"`
// 	Types            []string    `json:"types"`
// 	Actions          []ABIAction `json:"actions"`
// 	Tables           []ABITable  `json:"tables"`
// 	RicardianClauses []string    `json:"ricardian_clauses"`
// 	Variants         []string    `json:"variants"`
// 	AbiExtensions    []string    `json:"abi_extensions"`
// 	ErrorMessages    []string    `json:"error_messages"`
// }

// export class VariantDef {
// 	name = "";
// 	types: string[] = [];
// }

// export class ABIActionResult {
// 	constructor(
// 		public name: string = "",
// 		public result_type: string = ""
// 	) {}
// }

// export class ABI {
// 	version = "eosio::abi/1.2";
// 	structs: ABIStruct[] = [];
// 	types: string[] = [];
// 	actions: ABIAction[] = [];
// 	tables: ABITable[] = [];
// 	ricardian_clauses: string[] = [];
// 	variants: VariantDef[] = [];
// 	action_results: ABIActionResult[] = [];
// 	abi_extensions: string[] = [];
// 	error_messages: string[] = [];
// }


// type ABIStructField struct {
// 	Name string `json:"name"`
// 	Type string `json:"type"`
// }

struct ABIStructField {
  string name;
  string type;
};

NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(ABIStructField,
  name,
  type);

// type ABIStruct struct {
// 	Name   string           `json:"name"`
// 	Base   string           `json:"base"`
// 	Fields []ABIStructField `json:"fields"`
// }

struct ABIStruct {
  string name;
  string base;
  vector<ABIStructField> fields;
};

NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(ABIStruct,
  name,
  base,
  fields);

// export class ABI {
// 	version = "eosio::abi/1.2";
// 	structs: ABIStruct[] = [];
// 	types: string[] = [];
// 	actions: ABIAction[] = [];
// 	tables: ABITable[] = [];
// 	ricardian_clauses: string[] = [];
// 	variants: VariantDef[] = [];
// 	action_results: ABIActionResult[] = [];
// 	abi_extensions: string[] = [];
// 	error_messages: string[] = [];
// }

struct ABI {
  string version;
  vector<ABIStruct> structs;
  vector<ABIAction> actions;
  vector<ABITable> tables;
	vector<string> ricardian_clauses;
};

NLOHMANN_DEFINE_ORDERED_TYPE_NON_INTRUSIVE(ABI,
  version,
  structs,
  actions,
  tables,
  ricardian_clauses);

struct ABIGenerator {
  ABI abi;

  ABIGenerator();  
  void add_struct();
  void add_action(FunctionStmt *stmt);
  string generate();

  static ABIGenerator& instance();
};
