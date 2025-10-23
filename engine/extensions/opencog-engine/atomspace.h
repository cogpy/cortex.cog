#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "atom.h"
#include "json/json.h"

namespace cortex::opencog {

// AtomSpace - hypergraph knowledge representation system
class AtomSpace {
 public:
  AtomSpace() = default;
  
  // Add atom to atomspace
  std::shared_ptr<Atom> AddAtom(std::shared_ptr<Atom> atom) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = GenerateAtomKey(atom);
    
    auto it = atoms_.find(key);
    if (it != atoms_.end()) {
      // Atom already exists, merge truth values
      auto existing = it->second;
      MergeTruthValues(existing, atom);
      return existing;
    }
    
    atoms_[key] = atom;
    return atom;
  }
  
  // Get atom by name
  std::shared_ptr<Atom> GetAtom(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& pair : atoms_) {
      if (pair.second->GetName() == name) {
        return pair.second;
      }
    }
    return nullptr;
  }
  
  // Query atoms by type
  std::vector<std::shared_ptr<Atom>> GetAtomsByType(AtomType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<Atom>> result;
    for (const auto& pair : atoms_) {
      if (pair.second->GetType() == type) {
        result.push_back(pair.second);
      }
    }
    return result;
  }
  
  // Get all atoms
  std::vector<std::shared_ptr<Atom>> GetAllAtoms() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<Atom>> result;
    result.reserve(atoms_.size());
    for (const auto& pair : atoms_) {
      result.push_back(pair.second);
    }
    return result;
  }
  
  // Clear atomspace
  void Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    atoms_.clear();
  }
  
  // Get atomspace size
  size_t Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return atoms_.size();
  }
  
  // Export to JSON
  Json::Value ToJson() const {
    std::lock_guard<std::mutex> lock(mutex_);
    Json::Value json;
    json["size"] = static_cast<Json::UInt64>(atoms_.size());
    Json::Value atoms(Json::arrayValue);
    for (const auto& pair : atoms_) {
      atoms.append(pair.second->ToJson());
    }
    json["atoms"] = atoms;
    return json;
  }
  
 private:
  std::string GenerateAtomKey(std::shared_ptr<Atom> atom) {
    // Simple key generation based on type and name
    return std::to_string(static_cast<int>(atom->GetType())) + ":" + 
           atom->GetName();
  }
  
  void MergeTruthValues(std::shared_ptr<Atom> existing, 
                       std::shared_ptr<Atom> incoming) {
    // Simple averaging merge - can be replaced with more sophisticated logic
    auto existing_tv = existing->GetTruthValue();
    auto incoming_tv = incoming->GetTruthValue();
    
    float new_strength = (existing_tv.strength + incoming_tv.strength) / 2.0f;
    float new_confidence = std::max(existing_tv.confidence, 
                                   incoming_tv.confidence);
    
    existing->SetTruthValue(TruthValue(new_strength, new_confidence));
  }
  
  mutable std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<Atom>> atoms_;
};

}  // namespace cortex::opencog
