#include "cognitive.h"
#include "utils/cortex_utils.h"
#include "utils/logging_utils.h"

void CognitiveController::AddKnowledge(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  
  auto json_body = std::make_shared<Json::Value>();
  
  if (req->getJsonObject()) {
    *json_body = *(req->getJsonObject());
  }
  
  opencog_engine_->AddKnowledge(
    json_body,
    [callback](Json::Value&& result, Json::Value&& error) {
      if (!error.empty()) {
        auto resp = cortex_utils::CreateCortexHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
      }
      
      auto resp = cortex_utils::CreateCortexHttpJsonResponse(result);
      resp->setStatusCode(k200OK);
      callback(resp);
    }
  );
}

void CognitiveController::QueryKnowledge(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  
  auto json_body = std::make_shared<Json::Value>();
  
  if (req->getJsonObject()) {
    *json_body = *(req->getJsonObject());
  }
  
  opencog_engine_->QueryKnowledge(
    json_body,
    [callback](Json::Value&& result, Json::Value&& error) {
      if (!error.empty()) {
        auto resp = cortex_utils::CreateCortexHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
      }
      
      auto resp = cortex_utils::CreateCortexHttpJsonResponse(result);
      resp->setStatusCode(k200OK);
      callback(resp);
    }
  );
}

void CognitiveController::Reason(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  
  auto json_body = std::make_shared<Json::Value>();
  
  if (req->getJsonObject()) {
    *json_body = *(req->getJsonObject());
  }
  
  opencog_engine_->Reason(
    json_body,
    [callback](Json::Value&& result, Json::Value&& error) {
      if (!error.empty()) {
        auto resp = cortex_utils::CreateCortexHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
      }
      
      auto resp = cortex_utils::CreateCortexHttpJsonResponse(result);
      resp->setStatusCode(k200OK);
      callback(resp);
    }
  );
}

void CognitiveController::GetStats(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  
  auto stats = opencog_engine_->GetAtomSpaceStats();
  
  auto resp = cortex_utils::CreateCortexHttpJsonResponse(stats);
  resp->setStatusCode(k200OK);
  callback(resp);
}

void CognitiveController::FuseEngines(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
  
  auto json_body = std::make_shared<Json::Value>();
  
  if (req->getJsonObject()) {
    *json_body = *(req->getJsonObject());
  }
  
  if (!json_body->isMember("engines")) {
    Json::Value error;
    error["error"] = "Missing 'engines' field";
    auto resp = cortex_utils::CreateCortexHttpJsonResponse(error);
    resp->setStatusCode(k400BadRequest);
    callback(resp);
    return;
  }
  
  std::vector<std::string> engine_names;
  auto engines = (*json_body)["engines"];
  if (engines.isArray()) {
    for (const auto& engine : engines) {
      if (engine.isString()) {
        engine_names.push_back(engine.asString());
      }
    }
  }
  
  opencog_engine_->FuseEngines(
    engine_names,
    json_body,
    [callback](Json::Value&& result, Json::Value&& error) {
      if (!error.empty()) {
        auto resp = cortex_utils::CreateCortexHttpJsonResponse(error);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
      }
      
      auto resp = cortex_utils::CreateCortexHttpJsonResponse(result);
      resp->setStatusCode(k200OK);
      callback(resp);
    }
  );
}
