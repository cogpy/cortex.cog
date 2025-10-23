#include "opencog_engine.h"
#include <thread>
#include "utils/logging_utils.h"

namespace cortex::opencog {

void OpenCogEngine::HandleChatCompletion(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("messages")) {
    Json::Value error;
    error["error"] = "Invalid request: missing messages";
    callback(std::move(error), Json::Value());
    return;
  }
  
  // Extract user message
  std::string user_message;
  auto messages = (*json_body)["messages"];
  if (messages.isArray() && messages.size() > 0) {
    auto last_msg = messages[messages.size() - 1];
    if (last_msg.isMember("content")) {
      user_message = last_msg["content"].asString();
    }
  }
  
  // Process with cognitive reasoning
  auto result = ProcessWithCognition(user_message);
  
  // Build OpenAI-compatible response
  Json::Value response;
  response["id"] = "chatcmpl-opencog";
  response["object"] = "chat.completion";
  response["created"] = static_cast<Json::Int64>(time(nullptr));
  response["model"] = "opencog-cognitive-fusion";
  
  Json::Value choice;
  choice["index"] = 0;
  Json::Value message;
  message["role"] = "assistant";
  message["content"] = result.result.toStyledString();
  choice["message"] = message;
  choice["finish_reason"] = "stop";
  
  Json::Value choices(Json::arrayValue);
  choices.append(choice);
  response["choices"] = choices;
  
  Json::Value usage;
  usage["prompt_tokens"] = static_cast<int>(user_message.size() / 4);
  usage["completion_tokens"] = static_cast<int>(result.result.toStyledString().size() / 4);
  usage["total_tokens"] = usage["prompt_tokens"].asInt() + usage["completion_tokens"].asInt();
  response["usage"] = usage;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::HandleEmbedding(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("input")) {
    Json::Value error;
    error["error"] = "Invalid request: missing input";
    callback(std::move(error), Json::Value());
    return;
  }
  
  std::string input = (*json_body)["input"].asString();
  
  // Create knowledge representation in atomspace
  auto concept = std::make_shared<Node>(AtomType::CONCEPT_NODE, input);
  concept->SetTruthValue(TruthValue(0.8f, 0.7f));
  atomspace_->AddAtom(concept);
  
  // Generate embedding (simplified - would use actual embedding model)
  Json::Value embedding(Json::arrayValue);
  for (int i = 0; i < 768; i++) {
    embedding.append(0.0f);
  }
  
  Json::Value response;
  response["object"] = "embedding";
  response["model"] = "opencog-embedding";
  
  Json::Value data_item;
  data_item["object"] = "embedding";
  data_item["embedding"] = embedding;
  data_item["index"] = 0;
  
  Json::Value data(Json::arrayValue);
  data.append(data_item);
  response["data"] = data;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::LoadModel(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("model")) {
    Json::Value error;
    error["error"] = "Invalid request: missing model";
    callback(std::move(error), Json::Value());
    return;
  }
  
  std::string model_id = (*json_body)["model"].asString();
  loaded_models_[model_id] = "loaded";
  
  Json::Value response;
  response["status"] = "success";
  response["message"] = "Cognitive model loaded: " + model_id;
  response["model_id"] = model_id;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::UnloadModel(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("model")) {
    Json::Value error;
    error["error"] = "Invalid request: missing model";
    callback(std::move(error), Json::Value());
    return;
  }
  
  std::string model_id = (*json_body)["model"].asString();
  loaded_models_.erase(model_id);
  
  Json::Value response;
  response["status"] = "success";
  response["message"] = "Cognitive model unloaded: " + model_id;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::GetModelStatus(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  Json::Value response;
  response["engine"] = "opencog-cognitive-fusion";
  response["loaded"] = loaded_;
  response["atomspace_size"] = static_cast<Json::UInt64>(atomspace_->Size());
  response["hardware"] = hw_config_.compute_device;
  response["threads"] = hw_config_.num_threads;
  
  Json::Value models(Json::arrayValue);
  for (const auto& pair : loaded_models_) {
    Json::Value model;
    model["id"] = pair.first;
    model["status"] = pair.second;
    models.append(model);
  }
  response["models"] = models;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::GetModels(
    std::shared_ptr<Json::Value> jsonBody,
    http_callback&& callback) {
  
  Json::Value response;
  response["object"] = "list";
  
  Json::Value data(Json::arrayValue);
  
  Json::Value model;
  model["id"] = "opencog-cognitive-fusion";
  model["object"] = "model";
  model["created"] = static_cast<Json::Int64>(time(nullptr));
  model["owned_by"] = "cortex";
  model["capabilities"] = Json::arrayValue;
  model["capabilities"].append("reasoning");
  model["capabilities"].append("knowledge_representation");
  model["capabilities"].append("pattern_matching");
  model["capabilities"].append("cognitive_fusion");
  
  data.append(model);
  response["data"] = data;
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::AddKnowledge(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("knowledge")) {
    Json::Value error;
    error["error"] = "Invalid request: missing knowledge";
    callback(std::move(error), Json::Value());
    return;
  }
  
  auto knowledge = (*json_body)["knowledge"];
  
  // Add as concept node
  if (knowledge.isMember("concept")) {
    std::string concept_name = knowledge["concept"].asString();
    float strength = knowledge.get("strength", 0.5f).asFloat();
    float confidence = knowledge.get("confidence", 0.5f).asFloat();
    
    auto concept = std::make_shared<Node>(AtomType::CONCEPT_NODE, concept_name);
    concept->SetTruthValue(TruthValue(strength, confidence));
    atomspace_->AddAtom(concept);
  }
  
  Json::Value response;
  response["status"] = "success";
  response["atomspace_size"] = static_cast<Json::UInt64>(atomspace_->Size());
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::QueryKnowledge(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("query")) {
    Json::Value error;
    error["error"] = "Invalid request: missing query";
    callback(std::move(error), Json::Value());
    return;
  }
  
  std::string query = (*json_body)["query"].asString();
  
  // Pattern match in atomspace
  auto matches = pattern_matcher_->Match(query);
  
  Json::Value response;
  response["query"] = query;
  response["matches"] = Json::arrayValue;
  
  for (const auto& match : matches) {
    response["matches"].append(match->ToJson());
  }
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::Reason(
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  if (!json_body || !json_body->isMember("operation")) {
    Json::Value error;
    error["error"] = "Invalid request: missing operation";
    callback(std::move(error), Json::Value());
    return;
  }
  
  std::string operation = (*json_body)["operation"].asString();
  
  Json::Value response;
  response["operation"] = operation;
  
  if (operation == "induce") {
    // Induction reasoning
    auto concepts = atomspace_->GetAtomsByType(AtomType::CONCEPT_NODE);
    if (!concepts.empty()) {
      auto result = pln_->Induce(concepts);
      if (result) {
        response["result"] = result->ToJson();
      }
    }
  } else if (operation == "deduce") {
    // Deduction reasoning
    auto concepts = atomspace_->GetAtomsByType(AtomType::CONCEPT_NODE);
    if (concepts.size() >= 2) {
      auto result = pln_->Deduce(concepts[0], concepts[1]);
      if (result) {
        response["result"] = result->ToJson();
      }
    }
  }
  
  callback(std::move(response), Json::Value());
}

void OpenCogEngine::FuseEngines(
    const std::vector<std::string>& engine_names,
    std::shared_ptr<Json::Value> json_body,
    http_callback&& callback) {
  
  // Collect results from multiple engines
  std::vector<CognitiveResult> results;
  
  for (const auto& engine_name : engine_names) {
    // In a real implementation, we would call each engine
    // For now, create mock results
    CognitiveResult result;
    result.operation = "inference";
    result.confidence = 0.7f;
    result.engine_used = engine_name;
    result.result["response"] = "Result from " + engine_name;
    
    results.push_back(result);
  }
  
  // Fuse results
  auto fused = cognitive_fusion_->Fuse(results);
  
  Json::Value response = fused.ToJson();
  callback(std::move(response), Json::Value());
}

CognitiveResult OpenCogEngine::ProcessWithCognition(const std::string& input) {
  CognitiveResult result;
  result.operation = "cognitive_processing";
  result.engine_used = "opencog-cognitive-fusion";
  
  // Add input as knowledge
  auto concept = std::make_shared<Node>(AtomType::CONCEPT_NODE, input);
  concept->SetTruthValue(TruthValue(0.8f, 0.9f));
  atomspace_->AddAtom(concept);
  
  // Pattern match to find related concepts
  auto matches = pattern_matcher_->Match(input);
  
  // Build cognitive response
  Json::Value response_data;
  response_data["input"] = input;
  response_data["atomspace_size"] = static_cast<Json::UInt64>(atomspace_->Size());
  response_data["related_concepts"] = static_cast<int>(matches.size());
  response_data["reasoning"] = "Processed with OpenCog cognitive architecture";
  response_data["hardware"] = hw_config_.compute_device;
  
  result.result = response_data;
  result.confidence = 0.85f;
  
  return result;
}

}  // namespace cortex::opencog
