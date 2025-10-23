#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "json/json.h"

namespace cortex::opencog {

// Atom types for hypergraph representation
enum class AtomType {
  NODE,
  LINK,
  CONCEPT_NODE,
  PREDICATE_NODE,
  VARIABLE_NODE,
  INHERITANCE_LINK,
  SIMILARITY_LINK,
  EVALUATION_LINK
};

// Truth value for probabilistic reasoning
struct TruthValue {
  float strength;      // Probability [0, 1]
  float confidence;    // Confidence [0, 1]
  
  TruthValue(float s = 0.5f, float c = 0.0f) 
    : strength(s), confidence(c) {}
    
  Json::Value ToJson() const {
    Json::Value json;
    json["strength"] = strength;
    json["confidence"] = confidence;
    return json;
  }
};

// Base Atom class - represents a node or link in hypergraph
class Atom {
 public:
  Atom(AtomType type, const std::string& name = "")
    : type_(type), name_(name), truth_value_(0.5f, 0.0f) {}
  
  virtual ~Atom() = default;
  
  AtomType GetType() const { return type_; }
  std::string GetName() const { return name_; }
  TruthValue GetTruthValue() const { return truth_value_; }
  
  void SetTruthValue(const TruthValue& tv) { truth_value_ = tv; }
  
  virtual Json::Value ToJson() const {
    Json::Value json;
    json["type"] = static_cast<int>(type_);
    json["name"] = name_;
    json["truth_value"] = truth_value_.ToJson();
    return json;
  }
  
 protected:
  AtomType type_;
  std::string name_;
  TruthValue truth_value_;
};

// Node - represents concepts, predicates, variables
class Node : public Atom {
 public:
  Node(AtomType type, const std::string& name) : Atom(type, name) {}
};

// Link - connects atoms in hypergraph
class Link : public Atom {
 public:
  Link(AtomType type, const std::vector<std::shared_ptr<Atom>>& outgoing)
    : Atom(type), outgoing_(outgoing) {}
  
  const std::vector<std::shared_ptr<Atom>>& GetOutgoing() const { 
    return outgoing_; 
  }
  
  Json::Value ToJson() const override {
    Json::Value json = Atom::ToJson();
    Json::Value outgoing(Json::arrayValue);
    for (const auto& atom : outgoing_) {
      outgoing.append(atom->ToJson());
    }
    json["outgoing"] = outgoing;
    return json;
  }
  
 private:
  std::vector<std::shared_ptr<Atom>> outgoing_;
};

}  // namespace cortex::opencog
