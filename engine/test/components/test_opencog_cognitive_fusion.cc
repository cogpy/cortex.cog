#include <gtest/gtest.h>
#include <memory>
#include "extensions/opencog-engine/atom.h"
#include "extensions/opencog-engine/atomspace.h"
#include "extensions/opencog-engine/cognitive_fusion.h"

using namespace cortex::opencog;

class AtomTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create test atoms
    concept_node = std::make_shared<Node>(
        AtomType::CONCEPT_NODE, "test_concept");
    predicate_node = std::make_shared<Node>(
        AtomType::PREDICATE_NODE, "test_predicate");
  }

  std::shared_ptr<Node> concept_node;
  std::shared_ptr<Node> predicate_node;
};

TEST_F(AtomTest, NodeCreation) {
  EXPECT_EQ(concept_node->GetType(), AtomType::CONCEPT_NODE);
  EXPECT_EQ(concept_node->GetName(), "test_concept");
}

TEST_F(AtomTest, TruthValueDefault) {
  auto tv = concept_node->GetTruthValue();
  EXPECT_FLOAT_EQ(tv.strength, 0.5f);
  EXPECT_FLOAT_EQ(tv.confidence, 0.0f);
}

TEST_F(AtomTest, SetTruthValue) {
  TruthValue new_tv(0.8f, 0.9f);
  concept_node->SetTruthValue(new_tv);
  
  auto tv = concept_node->GetTruthValue();
  EXPECT_FLOAT_EQ(tv.strength, 0.8f);
  EXPECT_FLOAT_EQ(tv.confidence, 0.9f);
}

TEST_F(AtomTest, LinkCreation) {
  std::vector<std::shared_ptr<Atom>> outgoing = {
    concept_node, predicate_node
  };
  
  auto link = std::make_shared<Link>(
      AtomType::INHERITANCE_LINK, outgoing);
  
  EXPECT_EQ(link->GetType(), AtomType::INHERITANCE_LINK);
  EXPECT_EQ(link->GetOutgoing().size(), 2);
  EXPECT_EQ(link->GetOutgoing()[0], concept_node);
  EXPECT_EQ(link->GetOutgoing()[1], predicate_node);
}

TEST_F(AtomTest, AtomToJson) {
  TruthValue tv(0.7f, 0.85f);
  concept_node->SetTruthValue(tv);
  
  auto json = concept_node->ToJson();
  
  EXPECT_EQ(json["name"].asString(), "test_concept");
  EXPECT_EQ(json["type"].asInt(), static_cast<int>(AtomType::CONCEPT_NODE));
  EXPECT_FLOAT_EQ(json["truth_value"]["strength"].asFloat(), 0.7f);
  EXPECT_FLOAT_EQ(json["truth_value"]["confidence"].asFloat(), 0.85f);
}

class AtomSpaceTest : public ::testing::Test {
 protected:
  void SetUp() override {
    atomspace = std::make_shared<AtomSpace>();
  }

  std::shared_ptr<AtomSpace> atomspace;
};

TEST_F(AtomSpaceTest, AddAtom) {
  auto atom = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "test_atom");
  
  auto added = atomspace->AddAtom(atom);
  
  EXPECT_NE(added, nullptr);
  EXPECT_EQ(atomspace->Size(), 1);
}

TEST_F(AtomSpaceTest, AddDuplicateAtom) {
  auto atom1 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "duplicate");
  auto atom2 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "duplicate");
  
  atom1->SetTruthValue(TruthValue(0.6f, 0.7f));
  atom2->SetTruthValue(TruthValue(0.8f, 0.9f));
  
  auto added1 = atomspace->AddAtom(atom1);
  auto added2 = atomspace->AddAtom(atom2);
  
  // Should merge, not add duplicate
  EXPECT_EQ(atomspace->Size(), 1);
  EXPECT_EQ(added1, added2);
  
  // Truth values should be merged (averaged)
  auto tv = added2->GetTruthValue();
  EXPECT_FLOAT_EQ(tv.strength, 0.7f);  // (0.6 + 0.8) / 2
  EXPECT_FLOAT_EQ(tv.confidence, 0.9f); // max(0.7, 0.9)
}

TEST_F(AtomSpaceTest, GetAtomByName) {
  auto atom = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "findme");
  atomspace->AddAtom(atom);
  
  auto found = atomspace->GetAtom("findme");
  
  EXPECT_NE(found, nullptr);
  EXPECT_EQ(found->GetName(), "findme");
}

TEST_F(AtomSpaceTest, GetAtomsByType) {
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "concept1"));
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "concept2"));
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::PREDICATE_NODE, "predicate1"));
  
  auto concepts = atomspace->GetAtomsByType(AtomType::CONCEPT_NODE);
  auto predicates = atomspace->GetAtomsByType(AtomType::PREDICATE_NODE);
  
  EXPECT_EQ(concepts.size(), 2);
  EXPECT_EQ(predicates.size(), 1);
}

TEST_F(AtomSpaceTest, Clear) {
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "test1"));
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "test2"));
  
  EXPECT_EQ(atomspace->Size(), 2);
  
  atomspace->Clear();
  
  EXPECT_EQ(atomspace->Size(), 0);
}

TEST_F(AtomSpaceTest, ToJson) {
  atomspace->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "test"));
  
  auto json = atomspace->ToJson();
  
  EXPECT_EQ(json["size"].asUInt64(), 1);
  EXPECT_TRUE(json["atoms"].isArray());
  EXPECT_EQ(json["atoms"].size(), 1);
}

class PatternMatcherTest : public ::testing::Test {
 protected:
  void SetUp() override {
    atomspace = std::make_shared<AtomSpace>();
    matcher = std::make_shared<PatternMatcher>(atomspace);
    
    // Add test atoms
    atomspace->AddAtom(std::make_shared<Node>(
        AtomType::CONCEPT_NODE, "artificial_intelligence"));
    atomspace->AddAtom(std::make_shared<Node>(
        AtomType::CONCEPT_NODE, "machine_learning"));
    atomspace->AddAtom(std::make_shared<Node>(
        AtomType::CONCEPT_NODE, "deep_learning"));
    atomspace->AddAtom(std::make_shared<Node>(
        AtomType::PREDICATE_NODE, "is_a"));
  }

  std::shared_ptr<AtomSpace> atomspace;
  std::shared_ptr<PatternMatcher> matcher;
};

TEST_F(PatternMatcherTest, MatchByName) {
  auto matches = matcher->Match("learning");
  
  EXPECT_EQ(matches.size(), 2);  // machine_learning and deep_learning
}

TEST_F(PatternMatcherTest, MatchByType) {
  auto concepts = matcher->MatchByType(AtomType::CONCEPT_NODE);
  auto predicates = matcher->MatchByType(AtomType::PREDICATE_NODE);
  
  EXPECT_EQ(concepts.size(), 3);
  EXPECT_EQ(predicates.size(), 1);
}

TEST_F(PatternMatcherTest, NoMatches) {
  auto matches = matcher->Match("nonexistent");
  
  EXPECT_EQ(matches.size(), 0);
}

class CognitiveFusionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    fusion = std::make_shared<CognitiveFusion>();
  }

  std::shared_ptr<CognitiveFusion> fusion;
};

TEST_F(CognitiveFusionTest, FuseSingleResult) {
  std::vector<CognitiveResult> results;
  
  CognitiveResult result;
  result.operation = "test";
  result.confidence = 0.8f;
  result.engine_used = "engine1";
  result.result["data"] = "test_data";
  
  results.push_back(result);
  
  auto fused = fusion->Fuse(results);
  
  EXPECT_EQ(fused.confidence, 0.8f);
  EXPECT_EQ(fused.engine_used, "fused");
}

TEST_F(CognitiveFusionTest, FuseMultipleResults) {
  std::vector<CognitiveResult> results;
  
  CognitiveResult result1;
  result1.confidence = 0.6f;
  result1.engine_used = "engine1";
  
  CognitiveResult result2;
  result2.confidence = 0.9f;
  result2.engine_used = "engine2";
  
  results.push_back(result1);
  results.push_back(result2);
  
  auto fused = fusion->Fuse(results);
  
  // Should pick highest confidence result
  EXPECT_EQ(fused.confidence, 0.75f);  // Average: (0.6 + 0.9) / 2
  EXPECT_EQ(fused.engine_used, "fused");
}

TEST_F(CognitiveFusionTest, FuseEmptyResults) {
  std::vector<CognitiveResult> results;
  
  auto fused = fusion->Fuse(results);
  
  EXPECT_EQ(fused.confidence, 0.0f);
  EXPECT_EQ(fused.engine_used, "none");
}

TEST_F(CognitiveFusionTest, CombineKnowledge) {
  auto target = std::make_shared<AtomSpace>();
  auto source1 = std::make_shared<AtomSpace>();
  auto source2 = std::make_shared<AtomSpace>();
  
  source1->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "knowledge1"));
  source2->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "knowledge2"));
  
  std::vector<std::shared_ptr<AtomSpace>> sources = {source1, source2};
  
  fusion->CombineKnowledge(target, sources);
  
  EXPECT_EQ(target->Size(), 2);
}

class PLNTest : public ::testing::Test {
 protected:
  void SetUp() override {
    atomspace = std::make_shared<AtomSpace>();
    pln = std::make_shared<PLN>(atomspace);
  }

  std::shared_ptr<AtomSpace> atomspace;
  std::shared_ptr<PLN> pln;
};

TEST_F(PLNTest, Deduction) {
  auto premise1 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "A");
  premise1->SetTruthValue(TruthValue(0.9f, 0.8f));
  
  auto premise2 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "B");
  premise2->SetTruthValue(TruthValue(0.8f, 0.7f));
  
  auto conclusion = pln->Deduce(premise1, premise2);
  
  EXPECT_NE(conclusion, nullptr);
  
  auto tv = conclusion->GetTruthValue();
  // Strength should be product: 0.9 * 0.8 = 0.72
  EXPECT_FLOAT_EQ(tv.strength, 0.72f);
  // Confidence should be minimum: min(0.8, 0.7) = 0.7
  EXPECT_FLOAT_EQ(tv.confidence, 0.7f);
}

TEST_F(PLNTest, Induction) {
  std::vector<std::shared_ptr<Atom>> examples;
  
  auto ex1 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "example1");
  ex1->SetTruthValue(TruthValue(0.9f, 0.8f));
  
  auto ex2 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "example2");
  ex2->SetTruthValue(TruthValue(0.7f, 0.6f));
  
  examples.push_back(ex1);
  examples.push_back(ex2);
  
  auto generalization = pln->Induce(examples);
  
  EXPECT_NE(generalization, nullptr);
  
  auto tv = generalization->GetTruthValue();
  // Strength should be average: (0.9 + 0.7) / 2 = 0.8
  EXPECT_FLOAT_EQ(tv.strength, 0.8f);
  // Confidence should be average: (0.8 + 0.6) / 2 = 0.7
  EXPECT_FLOAT_EQ(tv.confidence, 0.7f);
}

TEST_F(PLNTest, InductionEmpty) {
  std::vector<std::shared_ptr<Atom>> examples;
  
  auto generalization = pln->Induce(examples);
  
  EXPECT_EQ(generalization, nullptr);
}

TEST_F(PLNTest, Abduction) {
  // Add some concepts to atomspace
  auto concept1 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "cause1");
  concept1->SetTruthValue(TruthValue(0.9f, 0.8f));
  atomspace->AddAtom(concept1);
  
  auto concept2 = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "cause2");
  concept2->SetTruthValue(TruthValue(0.5f, 0.6f));
  atomspace->AddAtom(concept2);
  
  auto observation = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "observation");
  
  auto explanation = pln->Abduce(observation);
  
  EXPECT_NE(explanation, nullptr);
  // Should return concept with highest score (strength * confidence)
  EXPECT_EQ(explanation->GetName(), "cause1");
}

// Integration test
class CognitiveIntegrationTest : public ::testing::Test {
 protected:
  void SetUp() override {
    atomspace = std::make_shared<AtomSpace>();
    pattern_matcher = std::make_shared<PatternMatcher>(atomspace);
    cognitive_fusion = std::make_shared<CognitiveFusion>();
    pln = std::make_shared<PLN>(atomspace);
  }

  std::shared_ptr<AtomSpace> atomspace;
  std::shared_ptr<PatternMatcher> pattern_matcher;
  std::shared_ptr<CognitiveFusion> cognitive_fusion;
  std::shared_ptr<PLN> pln;
};

TEST_F(CognitiveIntegrationTest, KnowledgeAcquisitionAndReasoning) {
  // Add knowledge
  auto ai = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "artificial_intelligence");
  ai->SetTruthValue(TruthValue(0.9f, 0.85f));
  atomspace->AddAtom(ai);
  
  auto ml = std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "machine_learning");
  ml->SetTruthValue(TruthValue(0.8f, 0.9f));
  atomspace->AddAtom(ml);
  
  // Pattern match
  auto matches = pattern_matcher->Match("learning");
  EXPECT_EQ(matches.size(), 1);
  
  // Reasoning
  auto conclusion = pln->Deduce(ai, ml);
  EXPECT_NE(conclusion, nullptr);
  
  // Verify atomspace has all atoms
  EXPECT_GE(atomspace->Size(), 3);  // ai, ml, conclusion
}

TEST_F(CognitiveIntegrationTest, MultiEngineKnowledgeFusion) {
  auto space1 = std::make_shared<AtomSpace>();
  auto space2 = std::make_shared<AtomSpace>();
  
  space1->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "knowledge_from_engine1"));
  space2->AddAtom(std::make_shared<Node>(
      AtomType::CONCEPT_NODE, "knowledge_from_engine2"));
  
  std::vector<std::shared_ptr<AtomSpace>> sources = {space1, space2};
  cognitive_fusion->CombineKnowledge(atomspace, sources);
  
  EXPECT_EQ(atomspace->Size(), 2);
  
  auto atom1 = atomspace->GetAtom("knowledge_from_engine1");
  auto atom2 = atomspace->GetAtom("knowledge_from_engine2");
  
  EXPECT_NE(atom1, nullptr);
  EXPECT_NE(atom2, nullptr);
}
