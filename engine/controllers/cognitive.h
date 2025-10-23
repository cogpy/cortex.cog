#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <memory>
#include "extensions/opencog-engine/opencog_engine.h"

using namespace drogon;

class CognitiveController : public drogon::HttpController<CognitiveController, false> {
 public:
  METHOD_LIST_BEGIN
  
  // Add knowledge to atomspace
  METHOD_ADD(CognitiveController::AddKnowledge, "/knowledge", Options, Post);
  ADD_METHOD_TO(CognitiveController::AddKnowledge, "/v1/cognitive/knowledge", 
                Options, Post);
  
  // Query knowledge from atomspace
  METHOD_ADD(CognitiveController::QueryKnowledge, "/knowledge/query", 
             Options, Post);
  ADD_METHOD_TO(CognitiveController::QueryKnowledge, 
                "/v1/cognitive/knowledge/query", Options, Post);
  
  // Perform reasoning operations
  METHOD_ADD(CognitiveController::Reason, "/reason", Options, Post);
  ADD_METHOD_TO(CognitiveController::Reason, "/v1/cognitive/reason", 
                Options, Post);
  
  // Get atomspace statistics
  METHOD_ADD(CognitiveController::GetStats, "/stats", Get);
  ADD_METHOD_TO(CognitiveController::GetStats, "/v1/cognitive/stats", Get);
  
  // Fuse multiple engines
  METHOD_ADD(CognitiveController::FuseEngines, "/fuse", Options, Post);
  ADD_METHOD_TO(CognitiveController::FuseEngines, "/v1/cognitive/fuse", 
                Options, Post);
  
  METHOD_LIST_END

  explicit CognitiveController(
      std::shared_ptr<cortex::opencog::OpenCogEngine> engine)
      : opencog_engine_(engine) {}

  void AddKnowledge(const HttpRequestPtr& req,
                   std::function<void(const HttpResponsePtr&)>&& callback);

  void QueryKnowledge(const HttpRequestPtr& req,
                     std::function<void(const HttpResponsePtr&)>&& callback);

  void Reason(const HttpRequestPtr& req,
             std::function<void(const HttpResponsePtr&)>&& callback);

  void GetStats(const HttpRequestPtr& req,
               std::function<void(const HttpResponsePtr&)>&& callback);

  void FuseEngines(const HttpRequestPtr& req,
                  std::function<void(const HttpResponsePtr&)>&& callback);

 private:
  std::shared_ptr<cortex::opencog::OpenCogEngine> opencog_engine_;
};
