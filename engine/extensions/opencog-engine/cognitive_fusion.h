#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "atomspace.h"
#include "json/json.h"

namespace cortex::opencog {

// Cognitive operation result
struct CognitiveResult {
  std::string operation;
  Json::Value result;
  float confidence;
  std::string engine_used;
  
  Json::Value ToJson() const {
    Json::Value json;
    json["operation"] = operation;
    json["result"] = result;
    json["confidence"] = confidence;
    json["engine_used"] = engine_used;
    return json;
  }
};

// Pattern matching for cognitive operations
class PatternMatcher {
 public:
  PatternMatcher(std::shared_ptr<AtomSpace> atomspace) 
    : atomspace_(atomspace) {}
  
  // Match pattern against atomspace
  std::vector<std::shared_ptr<Atom>> Match(const std::string& pattern) {
    // Simple name-based pattern matching
    std::vector<std::shared_ptr<Atom>> results;
    auto atoms = atomspace_->GetAllAtoms();
    
    for (const auto& atom : atoms) {
      if (atom->GetName().find(pattern) != std::string::npos) {
        results.push_back(atom);
      }
    }
    
    return results;
  }
  
  // Match by type
  std::vector<std::shared_ptr<Atom>> MatchByType(AtomType type) {
    return atomspace_->GetAtomsByType(type);
  }
  
 private:
  std::shared_ptr<AtomSpace> atomspace_;
};

// Cognitive fusion combines results from multiple engines
class CognitiveFusion {
 public:
  CognitiveFusion() = default;
  
  // Fuse multiple cognitive results
  CognitiveResult Fuse(const std::vector<CognitiveResult>& results) {
    if (results.empty()) {
      return CognitiveResult{"", Json::Value(), 0.0f, "none"};
    }
    
    // Simple confidence-weighted fusion
    CognitiveResult fused = results[0];
    float total_confidence = 0.0f;
    
    for (const auto& result : results) {
      total_confidence += result.confidence;
    }
    
    // Pick result with highest confidence
    float max_confidence = 0.0f;
    for (const auto& result : results) {
      if (result.confidence > max_confidence) {
        max_confidence = result.confidence;
        fused = result;
      }
    }
    
    fused.confidence = total_confidence / results.size();
    fused.engine_used = "fused";
    
    return fused;
  }
  
  // Combine knowledge from multiple sources
  void CombineKnowledge(std::shared_ptr<AtomSpace> target,
                       const std::vector<std::shared_ptr<AtomSpace>>& sources) {
    for (const auto& source : sources) {
      auto atoms = source->GetAllAtoms();
      for (const auto& atom : atoms) {
        target->AddAtom(atom);
      }
    }
  }
};

// Probabilistic Logic Network for reasoning
class PLN {
 public:
  PLN(std::shared_ptr<AtomSpace> atomspace) 
    : atomspace_(atomspace) {}
  
  // Deduction: If A→B and B→C, then A→C
  std::shared_ptr<Atom> Deduce(std::shared_ptr<Atom> premise1,
                              std::shared_ptr<Atom> premise2) {
    // Simplified deduction
    auto tv1 = premise1->GetTruthValue();
    auto tv2 = premise2->GetTruthValue();
    
    float strength = tv1.strength * tv2.strength;
    float confidence = std::min(tv1.confidence, tv2.confidence);
    
    auto conclusion = std::make_shared<Node>(
      AtomType::CONCEPT_NODE,
      "deduced_" + premise1->GetName() + "_" + premise2->GetName()
    );
    conclusion->SetTruthValue(TruthValue(strength, confidence));
    
    return atomspace_->AddAtom(conclusion);
  }
  
  // Induction: Generalize from examples
  std::shared_ptr<Atom> Induce(const std::vector<std::shared_ptr<Atom>>& examples) {
    if (examples.empty()) {
      return nullptr;
    }
    
    // Calculate average truth value
    float avg_strength = 0.0f;
    float avg_confidence = 0.0f;
    
    for (const auto& example : examples) {
      auto tv = example->GetTruthValue();
      avg_strength += tv.strength;
      avg_confidence += tv.confidence;
    }
    
    avg_strength /= examples.size();
    avg_confidence /= examples.size();
    
    auto generalization = std::make_shared<Node>(
      AtomType::CONCEPT_NODE,
      "induced_concept"
    );
    generalization->SetTruthValue(TruthValue(avg_strength, avg_confidence));
    
    return atomspace_->AddAtom(generalization);
  }
  
  // Abduction: Find best explanation
  std::shared_ptr<Atom> Abduce(std::shared_ptr<Atom> observation) {
    // Simplified abduction - find most similar concept
    auto concepts = atomspace_->GetAtomsByType(AtomType::CONCEPT_NODE);
    
    std::shared_ptr<Atom> best_explanation = nullptr;
    float best_score = 0.0f;
    
    for (const auto& concept : concepts) {
      auto tv = concept->GetTruthValue();
      float score = tv.strength * tv.confidence;
      
      if (score > best_score) {
        best_score = score;
        best_explanation = concept;
      }
    }
    
    return best_explanation;
  }
  
 private:
  std::shared_ptr<AtomSpace> atomspace_;
};

}  // namespace cortex::opencog
